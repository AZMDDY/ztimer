#include <iostream>
#include <thread>
#include <chrono>
#include "Timer.h"
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
    c.Start("2020-11-23 22:35:00");
    std::thread t([]() {
        while (1) {
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
    });
    t.join();
    return 0;
}