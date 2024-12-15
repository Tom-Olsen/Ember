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
	Float4x4 newLocalToWorldMatrix = translate * rotation * translateInverse * localToWorldMatrix;
	GetTransform()->SetLocalToWorldMatrix(newLocalToWorldMatrix);
}
const std::string SpinGlobal::ToString() const
{
	return "SpinGlobal";
}