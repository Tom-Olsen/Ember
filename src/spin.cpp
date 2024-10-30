#include "spin.h"
#include "logger.h"
#include "gameObject.h"



// Constructor:
Spin::Spin(Float3 eulerDegreesPerSecond)
{
	this->eulerDegreesPerSecond = eulerDegreesPerSecond;
}



// Destructor:
Spin::~Spin()
{

}



// Overrides:
void Spin::Update()
{
	Float3 eulerDegrees = gameObject->transform->GetEulerDegrees();
	Float3 newEulerDegrees = eulerDegrees + eulerDegreesPerSecond * Time::GetDeltaTime();
	gameObject->transform->SetEulerDegrees(newEulerDegrees);

	if (logAngles)
	{
		LOG_INFO(newEulerDegrees.ToString());
	}
}
std::string Spin::ToString() const
{
	return "Spin";
}