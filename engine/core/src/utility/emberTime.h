#ifndef __INCLUDE_GUARD_timer_h__
#define __INCLUDE_GUARD_timer_h__
#include <chrono>
#include <string_view>



namespace emberEngine
{
    enum class TimeUnit
    {
        s,  // seconds       = 10^0
        ms, // milli seconds = 10^-3
        us  // micro seconds = 10^-6
    };

    constexpr std::string_view TimeUnitNames[] = { "s", "ms", "us" };

    class Time
    {
    private: // Members
        static float s_time;                    // in seconds.
        static float s_deltaTime;               // in seconds.
        static float s_fixedDeltaTime;          // in seconds.
        static float s_fixedTimeAccumulator;    // in seconds.
        static std::chrono::steady_clock::time_point s_start;
        static std::chrono::steady_clock::time_point s_end;

    public: // Methods
        static void Reset();
        static void Update();
        static void Sleep(int time, TimeUnit unit = TimeUnit::s);
        
        // Getters:
        static float GetTime(TimeUnit unit = TimeUnit::s);
        static float GetDeltaTime(TimeUnit unit = TimeUnit::s);
        static float GetFixedDeltaTime(TimeUnit unit = TimeUnit::s);
        static bool UpdatePhysics();

    private: // Methods
        // Delete all constructors:
        Time() = delete;
        Time(const Time&) = delete;
        Time& operator=(const Time&) = delete;
        ~Time() = delete;
    };
}



#endif // __INCLUDE_GUARD_timer_h__