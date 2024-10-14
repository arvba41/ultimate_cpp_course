#pragma once

#include <chrono>
#include <thread>

class Timer {
public:
    void reset() { _time = 0; };
    void tic() { start = std::chrono::high_resolution_clock::now(); };
    void toc()
    {
        _time +=
            std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start)
                .count();
    };
    double elapsed() const { return (double)(_time) * 1e-3; }; // Return in milliseconds

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start;

    long long _time{0};
};
