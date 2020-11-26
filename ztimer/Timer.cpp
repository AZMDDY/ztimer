#include "Timer.h"
#include "TimerManager.h"

namespace ztimer {

    Timer::Timer() : timerId(0) {}

    Timer::~Timer() { Stop(); }

    void Timer::Start(TimerMode mode, unsigned int duration)
    {
        this->mode = mode;
        this->duration = duration;
        this->timerId = reinterpret_cast<unsigned long>(this);
        int ret = TimerManager::Instance().RegisterRelTimer(this->timerId, mode, duration);
        if (!ret) {
            printf("start timer failed. error code: %d", ret);
        }
    }

    void Timer::Start(const std::string& futureTime)
    {
        this->mode = ONCE;
        this->duration = 0;
        this->timerId = reinterpret_cast<unsigned long>(this);
        int ret = TimerManager::Instance().RegisterAbsTimer(this->timerId, futureTime);
        if (!ret) {
            printf("start timer failed. error code: %d", ret);
        }
    }

    void Timer::Stop()
    {
        int ret = TimerManager::Instance().UnRegisterTimer(timerId);
        if (!ret) {
            printf("stop timer failed. error code: %d", ret);
        }
        timerId = 0;
    }
}  // namespace ztimer