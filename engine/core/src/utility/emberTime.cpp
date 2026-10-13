#include "emberTime.h"
#include "emberMath.h"
#include <algorithm>
#include <thread>



namespace emberCore
{
	// Static members:
	float Time::s_time = 0.0f;
	float Time::s_deltaTime = 0.0f;
	float Time::s_fixedDeltaTime = 1.0f / 50.0f;
	float Time::s_fixedTimeAccumulator = 0.0f;
	std::chrono::steady_clock::time_point Time::s_start = std::chrono::steady_clock::now();
	std::chrono::steady_clock::time_point Time::s_end = std::chrono::steady_clock::now();
	float Time::s_frameTimes[fpsWindowSize] = {};
	int Time::s_frameTimeIndex = 0;
	int Time::s_frameTimeCount = 0;
	double Time::s_frameTimeSum = 0.0;



	// Time management:
	void Time::Reset()
	{
		s_time = 0.0f;
		s_deltaTime = 0.0f;
		s_fixedDeltaTime = 1.0f / 50.0f;
		s_fixedTimeAccumulator = 0.0f;
		std::fill_n(s_frameTimes, fpsWindowSize, 0.0f);
		s_frameTimeIndex = 0;
		s_frameTimeCount = 0;
		s_frameTimeSum = 0.0;
		s_start = std::chrono::steady_clock::now();
	}
	void Time::Update()
	{
		s_end = std::chrono::steady_clock::now();
		s_deltaTime = std::chrono::duration<float>(s_end - s_start).count();
		s_time += s_deltaTime;
		s_fixedTimeAccumulator += s_deltaTime;
		s_start = s_end;
		s_frameTimeSum -= s_frameTimes[s_frameTimeIndex];
		s_frameTimes[s_frameTimeIndex] = s_deltaTime;
		s_frameTimeSum += s_deltaTime;
		s_frameTimeIndex = (s_frameTimeIndex + 1) % fpsWindowSize;
		s_frameTimeCount = std::min(s_frameTimeCount + 1, fpsWindowSize);
	}
	void Time::Sleep(int time, TimeUnit unit)
	{
		switch (unit)
		{
		case TimeUnit::s:
			std::this_thread::sleep_for(std::chrono::seconds(time));
			break;
		case TimeUnit::ms:
			std::this_thread::sleep_for(std::chrono::milliseconds(time));
			break;
		case TimeUnit::us:
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
		case TimeUnit::s:
			return s_time;
		case TimeUnit::ms:
			return s_time * 1'000;
		case TimeUnit::us:
			return s_time * 1'000'000;
		default:
			return 0;
		}
	}
	float Time::GetDeltaTime(TimeUnit unit)
	{
		switch (unit)
		{
		case TimeUnit::s:
			return s_deltaTime;
		case TimeUnit::ms:
			return s_deltaTime * 1'000;
		case TimeUnit::us:
			return s_deltaTime * 1'000'000;
		default:
			return 0;
		}
	}
	float Time::GetFixedDeltaTime(TimeUnit unit)
	{
		switch (unit)
		{
		case TimeUnit::s:
			return s_fixedDeltaTime;
		case TimeUnit::ms:
			return s_fixedDeltaTime * 1'000;
		case TimeUnit::us:
			return s_fixedDeltaTime * 1'000'000;
		default:
			return 0;
		}
	}
	float Time::GetFps()
	{
		return s_frameTimeSum > 0.0 ? static_cast<float>(s_frameTimeCount / s_frameTimeSum) : 0.0f;
	}
	bool Time::ShouldUpdatePhysics()
	{
		return s_fixedTimeAccumulator >= s_fixedDeltaTime;
	}
	bool Time::UpdatePhysics(bool skipDelayedUpdates)
	{
		if (ShouldUpdatePhysics())
		{
			s_fixedTimeAccumulator -= s_fixedDeltaTime;
			if (skipDelayedUpdates && s_fixedTimeAccumulator >= s_fixedDeltaTime)
				s_fixedTimeAccumulator = math::Fmod(s_fixedTimeAccumulator, s_fixedDeltaTime);
			return true;
		}
		return false;
	}
}