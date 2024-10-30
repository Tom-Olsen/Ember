#pragma once
#ifndef __INCLUDE_GUARD_time_h__
#define __INCLUDE_GUARD_time_h__
#include <chrono>
#include "mathf.h"



/// <summary>
/// Purely static class that takes care of lifetime of all static Mesh objects.
/// </summary>
class Time
{
private: // Members
    static float time;
    static float deltaTime;
    static bool isInitialized;
	static std::chrono::steady_clock::time_point start;
	static std::chrono::steady_clock::time_point end;

public: // Methods
    static void Init();
    static void Clear();
    static void Reset();
    static void Update();

    // Getters:
    static float GetTime();
    static float GetDeltaTime();
    static Float4 GetTime4();
	static Float4 GetDeltaTime4();

private: // Methods
    // Delete all constructors:
    Time() = delete;
    Time(const Time&) = delete;
    Time& operator=(const Time&) = delete;
    ~Time() = delete;
};



#endif // __INCLUDE_GUARD_time_h__