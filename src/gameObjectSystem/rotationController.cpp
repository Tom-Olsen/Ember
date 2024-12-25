#include "rotationController.h"



// Constructors/Destructor:
RotationController::RotationController(float degreesPerSecond) : m_degreesPerSecond(degreesPerSecond) {}
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
		Float3 direction = 10.0f * Float3(movementY, 0.0f, -movementX) * Timer::GetDeltaTime();
		m_pTransform->AddToPosition(direction);
	}
	else
	{
		if (movementX != 0.0f || movementY != 0.0f)
		{
			float angleX = movementX * m_degreesPerSecond * mathf::DEG2RAD * Timer::GetDeltaTime();
			float angleY = -movementY * m_degreesPerSecond * mathf::DEG2RAD * Timer::GetDeltaTime();
			Float3x3 rotX = Float3x3::RotateX(angleX);
			Float3x3 rotY = Float3x3::RotateY(angleY);
			m_pTransform->SetRotationMatrix(rotY * m_pTransform->GetRotation3x3() * rotX);
		}
	}
}



// Overrides:
const std::string RotationController::ToString() const
{
	return "RotationController";
}