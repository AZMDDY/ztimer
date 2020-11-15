# C++定时器

> 采用时间轮法设计的相对定时器;

首先Tick线程产生时钟滴答信号，定时转动时间轮，时间轮的刻度针指向某刻度时，挂载在上面的定时器计数减一，当计数为0时，异步执行超时函数。

在本项目中，使用数组作为时间轮构造的数据结构，映射作为定时器存储的数据结构，以定时器对象的指针作为关键索引。使用构造一个定时器很简单，只需要继承`Timer`类即可。通过`Start`函数可以构造`循环定时器(LOOP)`和`一次性定时器(ONCE)`。

```cpp
#include <iostream>
#include <thread>
#include <chrono>
#include "ztimer/Timer.h"

class ATimer : public ztimer::Timer {
public:
    ATimer() {}
    ~ATimer() {}
    void TimeOut() { std::cerr << "timeout" << std::endl; }

private:
};

int main(int argc, char** argv)
{
    ATimer a;
    a.Start(ztimer::LOOP, 1000);
    std::thread t([]() {
        while (1) {
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
    });
    t.join();
    return 0;
}
```

在本项目中影响定时器的主要因素是产生滴答信号的精度。

![](https://raw.githubusercontent.com/AZMDDY/imgs/master/20201115121300.png)
