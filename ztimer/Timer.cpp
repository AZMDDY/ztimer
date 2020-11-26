#include "Timer.h"
#include "TimerManager.h"

namespace ztimer {

    Timer::Timer() : timerId(0) {}

    Timer::~Timer() { Stop(); }

    int Timer::Start(TimerMode mode, unsigned int duration)
    {
        this->mode = mode;
        this->duration = duration;
        this->timerId = reinterpret_cast<unsigned long>(this);
        return TimerManager::Instance().RegisterRelTimer(this->timerId, mode, duration);
    }

    int Timer::Start(const std::string& futureTime)
    {
        this->mode = ONCE;
        this->duration = 0;
        this->timerId = reinterpret_cast<unsigned long>(this);
        return TimerManager::Instance().RegisterAbsTimer(this->timerId, futureTime);
    }

    int Timer::Stop()
    {
        int ret = TimerManager::Instance().UnRegisterTimer(timerId);
        timerId = 0;
        return ret;
    }
}  // namespace ztimer