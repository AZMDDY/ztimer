#include <thread>
#include <chrono>
#include <mutex>
#include <vector>
#include <condition_variable>
#include "TimerManager.h"

namespace ztimer {
    namespace {
        using namespace std::chrono_literals;
        static std::condition_variable cv;
        static std::mutex mtx;
        static bool tick = false;
        static void StartTick()
        {
            static std::thread tickThread([]() {
                while (1) {
                    tick = false;
                    std::this_thread::sleep_for(1ms);
                    std::lock_guard<std::mutex> lk(mtx);
                    tick = true;
                    cv.notify_one();
                }
            });
        }
    }  // namespace

    TimerManager::TimerManager()
    {
        // 构造一个时间轮
        auto cmp = [](TimerAttr aTimer, TimerAttr bTimer) { return false; };
        std::priority_queue<TimerAttr, std::vector<TimerAttr>, decltype(cmp)> tmp(cmp);
        std::vector<decltype(tmp)> timeWheel(1000, tmp);
        unsigned int pin = 0;  // 刻度针
        std::thread turnThread([&]() {
            std::unique_lock<std::mutex> lk(mtx);
            cv.wait(lk, []() { return tick; });
            pin++;
            pin = (pin == timeWheel.size() ? 0 : pin);
        });
    }

    TimerManager::~TimerManager() {}

    TimerManager& TimerManager::Instance()
    {
        static TimerManager instance;
        return instance;
    }

    void TimerManager::RegisterRelTimer(unsigned long timerId, TimerMode mode, uint32_t duration) {}

    void TimerManager::UnRegisterTimer(unsigned long timerId) {}

}  // namespace ztimer