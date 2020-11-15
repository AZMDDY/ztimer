#ifndef ZTIMER_TIMER_MANAGER_H_
#define ZTIMER_TIMER_MANAGER_H_
#include <array>
#include <vector>
#include <mutex>
#include <unordered_map>
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
            unsigned int turn;  // 轮数
            bool operator<(TimerAttr otherTimerAttr) const;
        };
        TimerManager();
        inline bool TimerValid(unsigned long timerId);
        inline bool TimerExist(unsigned long timerId);

    private:
        unsigned int pin;                    // 刻度针
        const unsigned int timeWheelPeriod;  // 时间轮周期(刻度数)
        // 定时器ID: [定时器模式, 轮数, 轮数计数]]
        std::vector<std::unordered_map<unsigned long, std::array<unsigned int, 3>>> timeWheel;  // 时间轮
        // 定时ID : 定时器挂载点
        std::unordered_map<unsigned long, unsigned int> timerMap;

        std::mutex mtx;
    };
}  // namespace ztimer

#endif  // ZTIMER_TIMER_MANAGER_H_