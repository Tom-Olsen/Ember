#include "spinAroundOrigin.h"
#include "gameObject.h"
#include "logger.h"



// Constructor:
SpinAroundOrigin::SpinAroundOrigin(float degreesPerSecond)
{
	this->degreesPerSecond = degreesPerSecond;
}



// Destructor:
SpinAroundOrigin::~SpinAroundOrigin()
{

}



// Overrides:
void SpinAroundOrigin::Update()
{
	Float4x4 localTowroldMatrix = transform->GetLocalToWorldMatrix();
	Float4x4 rotateY = Float4x4::RotateY(mathf::ToRadians(degreesPerSecond) * Timer::GetDeltaTime());
	transform->SetLocalToWorldMatrix(rotateY * localTowroldMatrix);
}
std::string SpinAroundOrigin::ToString() const
{
	return "Spin";
}