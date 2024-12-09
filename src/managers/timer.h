#ifndef __INCLUDE_GUARD_time_h__
#define __INCLUDE_GUARD_time_h__
#include <chrono>



class Timer
{
private: // Members
    static float s_time;
    static float s_deltaTime;
    static bool s_isInitialized;
	static std::chrono::steady_clock::time_point s_start;
	static std::chrono::steady_clock::time_point s_end;

public: // Methods
    static void Init();
    static void Clear();
    static void Reset();
    static void Update();

    // Getters:
    static float GetTime();
    static float GetDeltaTime();

private: // Methods
    // Delete all constructors:
    Timer() = delete;
    Timer(const Timer&) = delete;
    Timer& operator=(const Timer&) = delete;
    ~Timer() = delete;
};



#endif // __INCLUDE_GUARD_time_h__