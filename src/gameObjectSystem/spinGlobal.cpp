#include "spinGlobal.h"
#include "gameObject.h"
#include "logger.h"



// Constructor/Destructor:
SpinGlobal::SpinGlobal(Float3 position, Float3 eulerDegreesPerSecond, Uint3 rotationOrder)
{
	this->position = position;
	this->eulerDegreesPerSecond = eulerDegreesPerSecond;
	this->rotationOrder = rotationOrder;
}
SpinGlobal::~SpinGlobal()
{

}



// Overrides:
void SpinGlobal::Update()
{
	Float3 eulerRadians = eulerDegreesPerSecond.ToRadians() * Timer::GetDeltaTime();
	Float4x4 rotation = Float4x4::Rotate(eulerRadians, rotationOrder);
	Float4x4 translate = Float4x4::Translate(position);
	Float4x4 translateInverse = Float4x4::Translate(-position);

	Float4x4 localToWorldMatrix = transform->GetLocalToWorldMatrix();
	transform->SetLocalToWorldMatrix(translate * rotation * translateInverse * localToWorldMatrix);
}
std::string SpinGlobal::ToString() const
{
	return "SpinGlobal";
}