#ifndef ZTIMER_TIMER_MANAGER_H_
#define ZTIMER_TIMER_MANAGER_H_
#include <array>
#include <vector>
#include <string>
#include <mutex>
#include <unordered_map>
#include "Timer.h"

namespace ztimer {

    class TimerManager {
    public:
        static TimerManager& Instance();
        ~TimerManager();
        void RegisterRelTimer(unsigned long timerId, TimerMode mode, unsigned int duration);
        void RegisterAbsTimer(unsigned long timerId, const std::string& timePoint);
        void UnRegisterTimer(unsigned long timerId);

    private:
        TimerManager();
        inline bool TimerValid(unsigned long timerId);
        inline bool TimerExist(unsigned long timerId);
        std::vector<int> TimePointValid(const std::string& timePoint);
        bool TimePointValid(const std::chrono::system_clock::time_point& timePoint);
        std::chrono::system_clock::time_point Str2Time(const std::string& timePoint);
        inline std::chrono::system_clock::time_point Now();

    private:
        unsigned int pin;                    // 刻度针
        const unsigned int timeWheelPeriod;  // 时间轮周期(刻度数)
        // 定时器ID: [定时器模式, 轮数, 轮数计数]]
        std::vector<std::unordered_map<unsigned long, std::array<unsigned int, 3>>> timeWheel;  // 时间轮
        // 定时器ID : 定时器挂载点
        std::unordered_map<unsigned long, unsigned int> timerMap;
        // 定时器ID : 时间点
        std::unordered_map<unsigned long, std::chrono::system_clock::time_point> AbsTimerMap;
        std::mutex mtx;
    };
}  // namespace ztimer

#endif  // ZTIMER_TIMER_MANAGER_H_