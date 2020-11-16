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