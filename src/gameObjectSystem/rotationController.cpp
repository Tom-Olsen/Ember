#include "rotationController.h"



// Constructors/Destructor:
RotationController::RotationController(float degreesPerSecond, float translationSpeed)
	: m_degreesPerSecond(degreesPerSecond), m_translationSpeed (translationSpeed) {}
RotationController::~RotationController()
{

}



// Public methods:
void RotationController::Update()
{
	int movementX = 0;
	int movementY = 0;

	if (EventSystem::KeyDownOrHeld(SDLK_UP))
		movementX += 1;
	if (EventSystem::KeyDownOrHeld(SDLK_DOWN))
		movementX -= 1;
	if (EventSystem::KeyDownOrHeld(SDLK_RIGHT))
		movementY += 1;
	if (EventSystem::KeyDownOrHeld(SDLK_LEFT))
		movementY -= 1;

	if (EventSystem::KeyDownOrHeld(SDLK_LSHIFT))
	{
		Float3 direction = m_translationSpeed * Float3(movementY, movementX, 0.0f) * Timer::GetDeltaTime();
		m_pTransform->AddToPosition(direction);
	}
	else
	{
		if (movementX != 0.0f || movementY != 0.0f)
		{
			float angleX = movementX * m_degreesPerSecond * mathf::deg2rad * Timer::GetDeltaTime();
			float angleZ = -movementY * m_degreesPerSecond * mathf::deg2rad * Timer::GetDeltaTime();
			Float3x3 rotX = Float3x3::RotateX(angleX);
			Float3x3 rotZ = Float3x3::RotateZ(angleZ);
			m_pTransform->SetRotationMatrix(rotZ * m_pTransform->GetRotation3x3() * rotX);
		}
	}
}



// Overrides:
const std::string RotationController::ToString() const
{
	return "RotationController";
}