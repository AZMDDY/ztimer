#include <chrono>
#include <future>
#include <condition_variable>
#include "TimerManager.h"

namespace ztimer {
    namespace {
        using namespace std::chrono_literals;
        static std::condition_variable cv;
        static std::mutex tickMtx;  // 用于同步滴答
        static bool tick = false;
        const unsigned int tickPrecision = 1;  // ms
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
                    iter->second[2]--;
                    if (iter->second[2] == 0) {
                        Timer* timer = reinterpret_cast<Timer*>(iter->first);
                        std::async(std::launch::async, &Timer::TimeOut, timer);  // 异步执行超时函数
                        switch (static_cast<TimerMode>(iter->second[0])) {
                            case LOOP: {
                                iter->second[2] = iter->second[1];
                                iter++;
                            } break;
                            case ONCE: {
                                // 移除定时器
                                timerMap.erase(iter->first);
                                iter = timeWheel[pin].erase(iter);
                            } break;
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

    void TimerManager::UnRegisterTimer(unsigned long timerId)
    {
        std::lock_guard<std::mutex> lk(mtx);
        if (!TimerValid(timerId) || !TimerExist(timerId)) {
            return;
        }
        unsigned int pos = timerMap[timerId];
        timerMap.erase(timerId);
        timeWheel[pos].erase(timerId);
    }

}  // namespace ztimer