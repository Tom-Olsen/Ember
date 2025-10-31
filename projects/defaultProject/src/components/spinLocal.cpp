#include "spinLocal.h"



namespace emberEngine
{
	// Constructor/Destructor:
	SpinLocal::SpinLocal(float speed)
	{
		m_speed = speed;
	}
	SpinLocal::~SpinLocal()
	{

	}
	void SpinLocal::SetSpeed(float speed)
	{
		m_speed = speed;
	}
	float SpinLocal::GetSpeed() const
	{
		return m_speed;
	}



	// Overrides:
	void SpinLocal::Update()
	{
		Float3x3 rotX = Float3x3::identity;
		Float3x3 rotZ = Float3x3::identity;

		if (EventSystem::KeyDownOrHeld(Input::Key::Right))
			rotZ *= Float3x3::RotateZ(-m_speed * math::deg2rad * Time::GetDeltaTime());
		if (EventSystem::KeyDownOrHeld(Input::Key::Left))
			rotZ *= Float3x3::RotateZ(m_speed * math::deg2rad * Time::GetDeltaTime());
		if (EventSystem::KeyDownOrHeld(Input::Key::Down))
			rotX *= Float3x3::RotateX(-m_speed * math::deg2rad * Time::GetDeltaTime());
		if (EventSystem::KeyDownOrHeld(Input::Key::Up))
			rotX *= Float3x3::RotateX(m_speed * math::deg2rad * Time::GetDeltaTime());

		GetTransform()->SetRotationMatrix(rotZ * GetTransform()->GetRotation3x3() * rotX);
	}
}