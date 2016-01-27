#ifndef _PHI_STOPWATCH_H_
#define _PHI_STOPWATCH_H_

#include "console.h"

#include <chrono>
#include <functional>


class stopwatch
{
private:
    std::chrono::time_point<std::chrono::steady_clock> _initial;
    std::chrono::time_point<std::chrono::steady_clock> _final;
    std::chrono::nanoseconds _currentNanoSeconds = std::chrono::nanoseconds::zero();
    bool _isRunning = false;
public:
    stopwatch() { };
    ~stopwatch() { };
    void Start();
    void Stop();
    void Reset();
    void Restart();
    const double GetElapsedSeconds();

    static const double MeasureInSeconds(const std::function<void(void)> &function)
    {
        auto watch = stopwatch();
        watch.Start();
        function();
        watch.Stop();

        return watch.GetElapsedSeconds();
    }

    static const double MeasureInSeconds(const std::function<void(void)> &function, const std::string &functionName)
    {
        auto watch = stopwatch();
        watch.Start();
        function();
        watch.Stop();

        auto msg = functionName + " took: ";
        auto elapsedSeconds = watch.GetElapsedSeconds();
        console::WriteLine(msg + std::to_string(elapsedSeconds));

        return elapsedSeconds;
    }

    static const double MeasureInMilliseconds(const std::function<void(void)> &function)
    {
        return MeasureInSeconds(function) * 1000;
    }

    static const double MeasureInMilliseconds(const std::function<void(void)> &function, const std::string &functionName)
    {
        auto watch = stopwatch();
        watch.Start();
        function();
        watch.Stop();

        auto msg = functionName + " took: ";
        auto elapsedMilliseconds = watch.GetElapsedSeconds() * 1000;
        console::WriteLine(msg + std::to_string(elapsedMilliseconds));

        return elapsedMilliseconds;
    }
};

#endif