#include "spinGlobal.h"
using namespace emberCore;
using namespace emberEcs;
using namespace emberCommon;



namespace emberEngine
{
	// Constructor/Destructor:
	SpinGlobal::SpinGlobal(Float3 position, EulerDegrees speed, Uint3 rotationOrder)
	{
		m_position = position;
		m_speed = speed;
		m_rotationOrder = rotationOrder;
	}
	SpinGlobal::~SpinGlobal()
	{

	}



	// Overrides:
	void SpinGlobal::Update()
	{
		if (EventSystem::KeyDown(Input::Key::Enter))
			m_spin = !m_spin;

		if (m_spin == false)
			return;

		EulerRadians angles = (m_speed * Time::GetDeltaTime()).ToRadians();
		Float4x4 rotation = Float4x4::Rotate(angles, m_rotationOrder);
		Float4x4 translate = Float4x4::Translate(m_position);
		Float4x4 translateInverse = Float4x4::Translate(-m_position);

		Float4x4 localToWorldMatrix = GetTransform()->GetLocalToWorldMatrix();
		Float4x4 newLocalToWorldMatrix = translate * rotation * translateInverse * localToWorldMatrix;
		GetTransform()->SetLocalToWorldMatrix(newLocalToWorldMatrix);
	}
}