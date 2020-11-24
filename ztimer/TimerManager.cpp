#include <chrono>
#include <future>
#include <regex>
#include <ctime>
#include <condition_variable>
#include "TimerManager.h"

namespace ztimer {
    namespace {
        using namespace std::chrono_literals;
        static std::condition_variable cv;
        static std::mutex tickMtx;  // 用于同步滴答
        static bool tick = false;
        const unsigned int tickPrecision = 1;  // ms
        const unsigned int AbsMode = 2;        // 定时器模式: 绝对定时器
        enum Mode {
            REL_ONCE = ONCE,  // 相对定时器 一次性模式
            REL_LOOP = LOOP,  // 相对定时器 循环模式
            ABS_ONCE = 2      // 绝对定时器 一次性模式
        };
        static void StartTick()
        {
            static std::thread tickThread([]() {
                while (true) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(tickPrecision));
                    std::lock_guard<std::mutex> lk(tickMtx);
                    tick = true;
                    cv.notify_one();
                }
            });
            tickThread.detach();
        }
    }  // namespace

    TimerManager::TimerManager() : pin(0), timeWheelPeriod(1000), timeWheel(timeWheelPeriod)
    {
        StartTick();
        std::thread turnThread([&]() {
            while (1) {
                {
                    std::unique_lock<std::mutex> lk(tickMtx);
                    cv.wait(lk, []() { return tick; });
                    tick = false;
                }
                std::lock_guard<std::mutex> lk(mtx);
                pin++;
                pin = (pin == timeWheelPeriod ? 0 : pin);
                for (auto iter = timeWheel[pin].begin(); iter != timeWheel[pin].end();) {
                    if (iter->second[2]-- == 0) {
                        Timer* timer = reinterpret_cast<Timer*>(iter->first);
                        switch (static_cast<Mode>(iter->second[0])) {
                            case REL_LOOP: {
                                std::async(std::launch::async, &Timer::TimeOut, timer);  // 异步执行超时函数
                                iter->second[2] = iter->second[1];
                                iter++;
                            } break;
                            case REL_ONCE: {
                                std::async(std::launch::async, &Timer::TimeOut, timer);  // 异步执行超时函数
                                // 移除一次性相对定时器
                                timerMap.erase(iter->first);
                                iter = timeWheel[pin].erase(iter);
                            } break;
                            case ABS_ONCE: {
                                // TODO 防止篡改系统时间攻击
                                if (AbsTimerMap[iter->first] <= Now()) {
                                    std::async(std::launch::async, &Timer::TimeOut, timer);  // 异步执行超时函数
                                    // 移除绝对定时器
                                    timerMap.erase(iter->first);
                                    AbsTimerMap.erase(iter->first);
                                    iter = timeWheel[pin].erase(iter);
                                }
                                else {
                                    iter->second[2] = iter->second[1];
                                    iter++;
                                }
                            }
                            default: break;
                        }
                    }
                    else {
                        iter++;
                    }
                }
            }
        });
        turnThread.detach();
    }

    TimerManager::~TimerManager() {}

    TimerManager& TimerManager::Instance()
    {
        static TimerManager instance;
        return instance;
    }

    bool TimerManager::TimerValid(unsigned long timerId) { return timerId != 0; }

    bool TimerManager::TimerExist(unsigned long timerId) { return timerMap.find(timerId) != timerMap.end(); }

    std::vector<int> TimerManager::TimePointValid(const std::string& timePoint)
    {
        // 合法的系统时间点： char timePoint[20] = "xxxx-xx-xx xx:xx:xx";
        if (timePoint.size() != 19) {
            return {};
        }
        std::regex rule(
            "^\\d{4}-((0[1-9])|(1[0-2]))-((0[1-9])|([1-2]\\d)|(3[0-1])) (([0-1]\\d)|(2[0-3]))(:[0-5]\\d){2}$");
        if (regex_match(timePoint, rule) == false) {
            return {};
        }
        std::vector<int> ans(6);
        ans[0] = std::stoi(timePoint.substr(0, 4));   // year
        ans[1] = std::stoi(timePoint.substr(5, 2));   // month
        ans[2] = std::stoi(timePoint.substr(8, 2));   // day
        ans[3] = std::stoi(timePoint.substr(11, 2));  // hour
        ans[4] = std::stoi(timePoint.substr(14, 2));  // minute
        ans[5] = std::stoi(timePoint.substr(17, 2));  // second
        return ans;
    }

    bool TimerManager::TimePointValid(const std::chrono::system_clock::time_point& timePoint)
    {
        return timePoint != std::chrono::system_clock::time_point();
    }

    std::chrono::system_clock::time_point TimerManager::Str2Time(const std::string& timePoint)
    {
        auto futureTime = TimePointValid(timePoint);
        if (futureTime.empty()) {
            return {};
        }
        std::tm date;
        date.tm_year = futureTime[0] - 1900;
        date.tm_mon = futureTime[1] - 1;
        date.tm_mday = futureTime[2];
        date.tm_hour = futureTime[3];
        date.tm_min = futureTime[4];
        date.tm_sec = futureTime[5];
        return std::chrono::system_clock::from_time_t(std::mktime(&date));
    }

    std::chrono::system_clock::time_point TimerManager::Now() { return std::chrono::system_clock::now(); }

    void TimerManager::RegisterRelTimer(unsigned long timerId, TimerMode mode, unsigned int duration)
    {
        std::lock_guard<std::mutex> lk(mtx);
        if (!TimerValid(timerId) || TimerExist(timerId)) {
            return;
        }
        // 计算定时器挂载位置
        unsigned int period = timeWheelPeriod * tickPrecision;
        unsigned int pos = duration % period + pin;
        pos = pos >= period ? pos - period : pos;

        unsigned int turn = duration / period;
        timerMap[timerId] = pos;
        timeWheel[pos][timerId] = {static_cast<unsigned int>(mode), turn, turn};
    }

    void TimerManager::RegisterAbsTimer(unsigned long timerId, const std::string& timePoint)
    {
        auto futureTime = Str2Time(timePoint);
        if (futureTime < Now()) {
            return;
        }

        if (!TimePointValid(futureTime)) {
            return;
        }
        std::lock_guard<std::mutex> lk(mtx);
        if (!TimerValid(timerId) || TimerExist(timerId)) {
            return;
        }
        timerMap[timerId] = 0;
        AbsTimerMap[timerId] = futureTime;
        timeWheel[0][timerId] = {AbsMode, 0, 0};
    }

    void TimerManager::UnRegisterTimer(unsigned long timerId)
    {
        std::lock_guard<std::mutex> lk(mtx);
        if (!TimerValid(timerId) || !TimerExist(timerId)) {
            return;
        }
        unsigned int pos = timerMap[timerId];
        timerMap.erase(timerId);
        if (AbsTimerMap.find(timerId) != AbsTimerMap.end()) {
            AbsTimerMap.erase(timerId);
        }
        timeWheel[pos].erase(timerId);
    }

}  // namespace ztimer