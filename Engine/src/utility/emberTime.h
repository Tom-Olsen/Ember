#ifndef __INCLUDE_GUARD_timer_h__
#define __INCLUDE_GUARD_timer_h__
#include <chrono>



namespace emberEngine
{
    class Time
    {
    private: // Members
        static float s_time;
        static float s_deltaTime;
        static float s_fixedDeltaTime;
        static float s_fixedTimeAccumulator;
        static std::chrono::steady_clock::time_point s_start;
        static std::chrono::steady_clock::time_point s_end;

    public: // Methods
        static void Init();
        static void Update();
        static void SleepS(int seconds);
        static void SleepMs(int milliseconds);
        
        // Getters:
        static float GetTime();
        static float GetDeltaTime();
        static float GetFixedDeltaTime();
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