#include "spin.h"
#include "logger.h"
#include "gameObject.h"



// Constructor:
Spin::Spin(Float3 eulerAnglesPerSecond)
{
	this->eulerAnglesPerSecond = eulerAnglesPerSecond;
}



// Destructor:
Spin::~Spin()
{

}



// Overrides:
void Spin::Update()
{
	Float3 eulerAngles = gameObject->transform->GetEulerAngles();
	Float3 newEulerAngles = eulerAngles + eulerAnglesPerSecond * Time::GetDeltaTime();
	gameObject->transform->SetEulerAngles(newEulerAngles);

	if (logAngles)
		LOG_INFO(to_string(newEulerAngles));
}
std::string Spin::ToString() const
{
	return "Spin";
}