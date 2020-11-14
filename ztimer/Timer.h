#ifndef ZTIMER_TIMER_H_
#define ZTIMER_TIMER_H_

namespace ztimer {

    enum TimerMode {
        ONCE,  // do once
        LOOP   // do loop
    };

    class Timer {
    public:
        Timer();
        virtual ~Timer();
        virtual void TimeOut() = 0;
        void Start(TimerMode mode, unsigned int duration);
        void Stop();

    private:
        unsigned long timerId;  // 定时器id
        TimerMode mode;         //定时器模式
        unsigned int duration;  //定时器超时时长
    };

}  // namespace ztimer
#endif  // ZTIMER_TIMER_H_