#include "emberTime.h"
#include <thread>



namespace emberEngine
{
	// Static members:
	float Time::s_time = 0.0f;
	float Time::s_deltaTime = 0.0f;
	float Time::s_fixedDeltaTime = 1.0f / 50.0f;
	float Time::s_fixedTimeAccumulator = 0.0f;
	std::chrono::steady_clock::time_point Time::s_start = std::chrono::steady_clock::now();
	std::chrono::steady_clock::time_point Time::s_end = std::chrono::steady_clock::now();



	// Time management:
	void Time::Reset()
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
	void Time::Sleep(int time, TimeUnit unit)
	{
		switch (unit)
		{
		case emberEngine::TimeUnit::s:
			std::this_thread::sleep_for(std::chrono::seconds(time));
			break;
		case emberEngine::TimeUnit::ms:
			std::this_thread::sleep_for(std::chrono::milliseconds(time));
			break;
		case emberEngine::TimeUnit::us:
			std::this_thread::sleep_for(std::chrono::microseconds(time));
			break;
		default:
			break;
		}
	}



	// Getters:
	float Time::GetTime(TimeUnit unit)
	{
		switch (unit)
		{
		case emberEngine::TimeUnit::s:
			return s_time;
		case emberEngine::TimeUnit::ms:
			return s_time * 1'000;
		case emberEngine::TimeUnit::us:
			return s_time * 1'000'000;
		default:
			return 0;
		}
	}
	float Time::GetDeltaTime(TimeUnit unit)
	{
		switch (unit)
		{
		case emberEngine::TimeUnit::s:
			return s_deltaTime;
		case emberEngine::TimeUnit::ms:
			return s_deltaTime * 1'000;
		case emberEngine::TimeUnit::us:
			return s_deltaTime * 1'000'000;
		default:
			return 0;
		}
	}
	float Time::GetFixedDeltaTime(TimeUnit unit)
	{
		switch (unit)
		{
		case emberEngine::TimeUnit::s:
			return s_fixedDeltaTime;
		case emberEngine::TimeUnit::ms:
			return s_fixedDeltaTime * 1'000;
		case emberEngine::TimeUnit::us:
			return s_fixedDeltaTime * 1'000'000;
		default:
			return 0;
		}
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