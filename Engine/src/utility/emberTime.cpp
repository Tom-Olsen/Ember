#include "emberTime.h"
#include <thread>



namespace emberEngine
{
	// Static members:
	float Time::s_time;
	float Time::s_deltaTime;
	float Time::s_fixedDeltaTime;
	float Time::s_fixedTimeAccumulator;
	std::chrono::steady_clock::time_point Time::s_start = std::chrono::steady_clock::now();
	std::chrono::steady_clock::time_point Time::s_end = std::chrono::steady_clock::now();



	// Time management:
	void Time::Init()
	{
		s_time = 0.0f;
		s_deltaTime = 0.0f;
		s_fixedDeltaTime = 1.0f / 50.0f;
		s_fixedTimeAccumulator = 0.0f;
		s_start = std::chrono::steady_clock::now();
	}
	void Time::Update()
	{
		s_end = std::chrono::steady_clock::now();
		s_deltaTime = std::chrono::duration<float>(s_end - s_start).count();
		s_time += s_deltaTime;
		s_fixedTimeAccumulator += s_deltaTime;
		s_start = s_end;
	}
	void Time::SleepS(int milliseconds)
	{
		std::this_thread::sleep_for(std::chrono::seconds(milliseconds));
	}
	void Time::SleepMs(int milliseconds)
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
	float Time::GetFixedDeltaTime()
	{
		return s_fixedDeltaTime;
	}
	bool Time::UpdatePhysics()
	{
		if (s_fixedTimeAccumulator >= s_fixedDeltaTime)
		{
			s_fixedTimeAccumulator -= s_fixedDeltaTime;
			return true;
		}
		return false;
	}
}