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

		if (EventSystem::KeyDownOrHeld(SDLK_RIGHT))
			rotZ *= Float3x3::RotateZ(-m_speed * mathf::deg2rad * Timer::GetDeltaTime());
		if (EventSystem::KeyDownOrHeld(SDLK_LEFT))
			rotZ *= Float3x3::RotateZ(m_speed * mathf::deg2rad * Timer::GetDeltaTime());
		if (EventSystem::KeyDownOrHeld(SDLK_DOWN))
			rotX *= Float3x3::RotateX(-m_speed * mathf::deg2rad * Timer::GetDeltaTime());
		if (EventSystem::KeyDownOrHeld(SDLK_UP))
			rotX *= Float3x3::RotateX(m_speed * mathf::deg2rad * Timer::GetDeltaTime());

		m_pTransform->SetRotationMatrix(rotZ * m_pTransform->GetRotation3x3() * rotX);
	}
	const std::string SpinLocal::ToString() const
	{
		return "SpinLocal";
	}
}