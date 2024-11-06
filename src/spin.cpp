#include "spin.h"
#include "gameObject.h"
#include "logger.h"



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
	Float3 addAngles = eulerDegreesPerSecond.ToRadians() * Time::GetDeltaTime();
	Float3x3 rotationPerSecond = Float3x3::Rotate(addAngles);
	Float3x3 rotation = gameObject->transform->GetRotation3x3();
	gameObject->transform->SetRotationMatrix(rotation * rotationPerSecond);
}
std::string Spin::ToString() const
{
	return "Spin";
}