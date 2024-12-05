#include "spinLocal.h"
#include "gameObject.h"
#include "logger.h"



// Constructor/Destructor:
SpinLocal::SpinLocal(Float3 eulerDegreesPerSecond, Uint3 rotationOrder)
{
	this->eulerDegreesPerSecond = eulerDegreesPerSecond;
	this->rotationOrder = rotationOrder;
}
SpinLocal::~SpinLocal()
{

}



// Overrides:
void SpinLocal::Update()
{
	Float3 eulerRadians = eulerDegreesPerSecond.ToRadians() * Timer::GetDeltaTime();
	Float3x3 rotation = Float3x3::Rotate(eulerRadians, rotationOrder);
	transform->SetRotationMatrix(transform->GetRotation3x3() * rotation);
}
std::string SpinLocal::ToString() const
{
	return "SpinLocal";
}