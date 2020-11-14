#include <iostream>
#include <thread>
#include "ztimer/Timer.h"
#include <chrono>

using namespace std::chrono_literals;

std::chrono::time_point<std::chrono::steady_clock> start;
std::chrono::time_point<std::chrono::steady_clock> end;

class ATimer : public ztimer::Timer {
public:
    ATimer() {}
    ~ATimer() {}
    void TimeOut()
    {
        end = std::chrono::steady_clock::now();
        std::cerr << "out..." << std::endl;
        std::chrono::duration<double> diff = end - start;
        std::cout << diff.count() << "s\n";
    }

private:
};

int main(int argc, char** argv)
{
    ATimer a;
    start = std::chrono::steady_clock::now();
    a.Start(ztimer::ONCE, 1000);
    std::thread t([]() {
        while (1) {
            std::this_thread::sleep_for(1500ms);
            return;
        }
    });
    t.join();
    return 0;
}