#ifndef ZTIMER_TIMER_H_
#define ZTIMER_TIMER_H_

namespace ztimer {

    enum TimerMode {
        ONCE,  // do once
        LOOP   // do loop
    };

    // 可能存在精度丢失！
    constexpr unsigned int operator""_h(long double h) { return static_cast<unsigned int>(60 * 1000 * 1000 * h); }
    constexpr unsigned int operator""_min(long double min) { return static_cast<unsigned int>(60 * 1000 * min); }
    constexpr unsigned int operator""_s(long double s) { return static_cast<unsigned int>(1000 * s); }
    constexpr unsigned int operator""_ms(long double ms) { return static_cast<unsigned int>(ms); }

    class Timer {
    public:
        Timer();
        virtual ~Timer();
        virtual void TimeOut() = 0;
        // duration: ms
        void Start(TimerMode mode, unsigned int duration);
        void Stop();

    private:
        unsigned long timerId;  // 定时器id
        TimerMode mode;         //定时器模式
        unsigned int duration;  //定时器超时时长
    };

}  // namespace ztimer
#endif  // ZTIMER_TIMER_H_