#include "spinGlobal.h"



// Constructor/Destructor:
SpinGlobal::SpinGlobal(Float3 position, Float3 eulerDegreesPerSecond, Uint3 rotationOrder)
{
	m_position = position;
	m_eulerDegreesPerSecond = eulerDegreesPerSecond;
	m_rotationOrder = rotationOrder;
}
SpinGlobal::~SpinGlobal()
{

}



// Overrides:
void SpinGlobal::Update()
{
	Float3 eulerRadians = m_eulerDegreesPerSecond.ToRadians() * Timer::GetDeltaTime();
	Float4x4 rotation = Float4x4::Rotate(eulerRadians, m_rotationOrder);
	Float4x4 translate = Float4x4::Translate(m_position);
	Float4x4 translateInverse = Float4x4::Translate(-m_position);

	Float4x4 localToWorldMatrix = GetTransform()->GetLocalToWorldMatrix();
	GetTransform()->SetLocalToWorldMatrix(translate * rotation * translateInverse * localToWorldMatrix);
}
const std::string SpinGlobal::ToString() const
{
	return "SpinGlobal";
}