#include "timer.h"



// Static members:
float Timer::s_time = 0.0f;
float Timer::s_deltaTime = 0.0f;
std::chrono::steady_clock::time_point Timer::s_start = std::chrono::steady_clock::now();
std::chrono::steady_clock::time_point Timer::s_end = std::chrono::steady_clock::now();



// Timer management:
void Timer::Update()
{
	s_end = std::chrono::steady_clock::now();
	s_deltaTime = std::chrono::duration<float>(s_end - s_start).count();
	s_time += s_deltaTime;
	s_start = s_end;
}
void Timer::Reset()
{
	s_time = 0.0f;
	s_deltaTime = 0.0f;
	auto s_start = std::chrono::steady_clock::now();
}



// Getters:
float Timer::GetTime()
{
	return s_time;
}
float Timer::GetDeltaTime()
{
	return s_deltaTime;
}