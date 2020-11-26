#ifndef ZTIMER_TIMER_H_
#define ZTIMER_TIMER_H_

#include <string>

constexpr unsigned int operator""_h(long double h)
{
    return static_cast<unsigned int>(60 * 1000 * 1000 * h);
}
constexpr unsigned int operator""_min(long double min)
{
    return static_cast<unsigned int>(60 * 1000 * min);
}
constexpr unsigned int operator""_s(long double s)
{
    return static_cast<unsigned int>(1000 * s);
}
constexpr unsigned int operator""_ms(long double ms)
{
    return static_cast<unsigned int>(ms);
}

namespace ztimer {

    enum TimerMode {
        ONCE = 0,  // do once
        LOOP = 1   // do loop
    };

    class Timer {
    public:
        Timer();
        virtual ~Timer();
        virtual void TimeOut() = 0;

        // 注意：如果两个Start()都调用了，只有第一个调用的会生效!!!
        int Start(TimerMode mode, unsigned int duration);  // 调用此函数，表明是相对定时器，duration: ms
        // futureTime的格式："xxxx-xx-xx xx:xx:xx"
        int Start(const std::string& futureTime);          // 调用此函数，表明是绝对定时器

        int Stop();

    private:
        unsigned long timerId;  // 定时器id
        TimerMode mode;         //定时器模式
        unsigned int duration;  //定时器超时时长
    };

}  // namespace ztimer
#endif  // ZTIMER_TIMER_H_