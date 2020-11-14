#include <chrono>
#include <future>
#include <condition_variable>
#include <iostream>
#include "TimerManager.h"

namespace ztimer {
    namespace {
        using namespace std::chrono_literals;
        static std::condition_variable cv;
        static std::mutex tickMtx;  // 用于同步滴答
        static bool tick = false;
        static void StartTick()
        {
            static std::thread tickThread([]() {
                while (true) {
                    std::this_thread::sleep_for(1ms);
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
                    if (iter->second[1]-- == 0) {
                        Timer* timer = reinterpret_cast<Timer*>(iter->first.timerId);
                        std::async(std::launch::async, &Timer::TimeOut, timer);  // 异步执行超时函数
                        switch (static_cast<TimerMode>(iter->second[0])) {
                            case LOOP: {
                                iter->second[0] = iter->first.turn;
                                iter++;
                            } break;
                            case ONCE: {
                                // 移除定时器
                                timerMap.erase(iter->first.timerId);
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

    bool TimerManager::TimerAttr::operator<(TimerAttr otherTimerAttr) const { return this->turn < otherTimerAttr.turn; }

    bool TimerManager::TimerValid(unsigned long timerId) { return timerId != 0; }

    bool TimerManager::TimerExist(unsigned long timerId) { return timerMap.find(timerId) != timerMap.end(); }

    void TimerManager::RegisterRelTimer(unsigned long timerId, TimerMode mode, unsigned int duration)
    {
        std::lock_guard<std::mutex> lk(mtx);
        if (!TimerValid(timerId) || TimerExist(timerId)) {
            return;
        }
        // 计算定时器挂载位置
        unsigned int pos = duration % timeWheelPeriod + pin;
        pos = pos >= timeWheelPeriod ? pos - timeWheelPeriod : pos;

        unsigned int turn = duration / timeWheelPeriod;

        timerMap[timerId] = {pos, turn};

        timeWheel[pos][{.timerId = timerId, .turn = turn}] = {static_cast<unsigned int>(mode), turn};
    }

    void TimerManager::UnRegisterTimer(unsigned long timerId)
    {
        std::lock_guard<std::mutex> lk(mtx);
        if (!TimerValid(timerId) || !TimerExist(timerId)) {
            return;
        }
        unsigned int pos = timerMap[timerId][0];
        unsigned int turn = timerMap[timerId][1];
        timerMap.erase(timerId);
        timeWheel[pos].erase({timerId, turn});
    }

}  // namespace ztimer