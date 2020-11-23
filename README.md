# C++定时器

> 采用时间轮法设计的定时器;

首先Tick线程产生时钟滴答信号，定时转动时间轮，时间轮的刻度针指向某刻度时，挂载在上面的定时器计数减一，当计数为0时，异步执行超时函数。

在本项目中，使用数组作为时间轮构造的数据结构，映射作为定时器存储的数据结构，以定时器对象的指针作为关键索引。使用构造一个定时器很简单，只需要继承`Timer`类即可。通过`Start`函数可以构造`循环定时器(LOOP)`、`一次性定时器(ONCE)`以及`绝对定时器`。

```cpp
#include <iostream>
#include <thread>
#include <chrono>
#include "ztimer/Timer.h"
using namespace ztimer;
class ATimer : public ztimer::Timer {
public:
    ATimer() {}
    ~ATimer() {}
    void TimeOut() { std::cerr << "a timeout" << std::endl; }

private:
};

class BTimer : public ztimer::Timer {
public:
    BTimer() {}
    ~BTimer() {}
    void TimeOut() { std::cerr << "b timeout" << std::endl; }

private:
};

class CTimer : public ztimer::Timer {
public:
    CTimer() {}
    ~CTimer() {}
    void TimeOut() { std::cerr << "c timeout" << std::endl; }

private:
};

int main(int argc, char** argv)
{
    ATimer a;
    a.Start(ztimer::ONCE, 0.8_s);
    BTimer b;
    b.Start(ztimer::ONCE, 2.5_s);
    CTimer c;
    c.Start("2020-11-23 22:35:00"); // 绝对定时器，基于过去时间无法成功注册定时器
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

## 编译

```bash
mkdir build && cd build
cmake ..
make
make install
```

## 示例

```bash
# 项目的根目录ROOT_DIR
cd ${ROOT_DIR}
mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=${ROOT_DIR}/examples ..
make
make install

cd examples
mkdir build && cd build
cmake ..
make
./example_ztimer
```

## 下一步计划

- [ ] 采用更精确的时钟滴答
- [ ] 绝对定时器防止篡改系统时间攻击
- [ ] 增加提示错误码
- [ ] 待补充。。。