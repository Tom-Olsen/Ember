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
	gameObject->transform->SetEulerAngles(eulerAngles + eulerAnglesPerSecond * Time::GetDeltaTime());
}
std::string Spin::ToString() const
{
	return "Spin";
}