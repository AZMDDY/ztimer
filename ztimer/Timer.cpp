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
        TimerManager::Instance().RegisterRelTimer(this->timerId, mode, duration);
    }

    void Timer::Stop()
    {
        TimerManager::Instance().UnRegisterTimer(timerId);
        timerId = 0;
    }
}  // namespace ztimer