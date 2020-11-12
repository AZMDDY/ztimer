#ifndef ZTIMER_TIMER_MANAGER_H_
#define ZTIMER_TIMER_MANAGER_H_
#include <array>
#include <thread>
#include <vector>
#include <queue>
#include "Timer.h"

namespace ztimer {

    class TimerManager {
    public:
        static TimerManager& Instance();
        ~TimerManager();
        void RegisterRelTimer(unsigned long timerId, TimerMode mode, unsigned int duration);
        void UnRegisterTimer(unsigned long timerId);

    private:
        // 定时器属性
        struct TimerAttr {
            unsigned long timerId;
            unsigned int duration;
            TimerMode mode;
        };
        TimerManager();

    private:
    };
}  // namespace ztimer

#endif  // ZTIMER_TIMER_MANAGER_H_