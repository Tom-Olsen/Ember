#include "time.h"



// Static members:
float Time::time = 0.0f;
float Time::deltaTime = 0.0f;
bool Time::isInitialized = false;
std::chrono::steady_clock::time_point Time::start = std::chrono::steady_clock::now();
std::chrono::steady_clock::time_point Time::end = std::chrono::steady_clock::now();



// Time management:
void Time::Init()
{
	if (isInitialized)
		return;

	isInitialized = true;
}
void Time::Clear()
{
	time = 0.0f;
	deltaTime = 0.0f;
	isInitialized = false;
}
void Time::Update()
{
	end = std::chrono::steady_clock::now();
	deltaTime = std::chrono::duration<float>(end - start).count();
	time += deltaTime;
	start = end;
}
void Time::Reset()
{
	time = 0.0f;
	deltaTime = 0.0f;
	auto start = std::chrono::steady_clock::now();
}



// Getters:
float Time::GetTime()
{
	return time;
}
float Time::GetDeltaTime()
{
	return deltaTime;
}
Float4 Time::GetTime4()
{
	return Float4(time, 2.0f * time, 3.0f * time, 4.0f * time);
}
Float4 Time::GetDeltaTime4()
{
	return Float4(deltaTime, 2.0f * deltaTime, 3.0f * deltaTime, 4.0f * deltaTime);
}