#include "emberTime.h"
#include <thread>



namespace emberEngine
{
	// Static members:
	float Time::s_time = 0.0f;
	float Time::s_deltaTime = 0.0f;
	std::chrono::steady_clock::time_point Time::s_start = std::chrono::steady_clock::now();
	std::chrono::steady_clock::time_point Time::s_end = std::chrono::steady_clock::now();



	// Time management:
	void Time::Reset()
	{
		s_time = 0.0f;
		s_deltaTime = 0.0f;
		auto s_start = std::chrono::steady_clock::now();
	}
	void Time::Update()
	{
		s_end = std::chrono::steady_clock::now();
		s_deltaTime = std::chrono::duration<float>(s_end - s_start).count();
		s_time += s_deltaTime;
		s_start = s_end;
	}
	void Time::Sleep(int milliseconds)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
	}



	// Getters:
	float Time::GetTime()
	{
		return s_time;
	}
	float Time::GetDeltaTime()
	{
		return s_deltaTime;
	}
}