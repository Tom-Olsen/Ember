#include "camera.h"
#include "macros.h"



// Constructor:
Camera::Camera()
{
	this->fov = 60.0f;
	UpdateViewMatrix();
}



// Destructor:
Camera::~Camera()
{

}



// Public:
Float4x4 Camera::GetViewMatrix()
{
	return viewMatrix;
}
Float4x4 Camera::GetProjectionMatrix()
{
	return projectionMatrix;
}
void Camera::PrintType() const
{
	LOG_TRACE("Camera");
}



// Private:
void Camera::UpdateViewMatrix()
{
	//// Calculate the view matrix
	//Float3 right = Float3(1.0f, 0.0f, 0.0f);
	//Float3 up = Float3(0.0f, 1.0f, 0.0f);
	//Float3 forward = Float3(0.0f, 0.0f, 1.0f);
	//viewMatrix = Float4x4(
	//	right.x, up.x, forward.x, position.x,
	//	right.y, up.y, forward.y, position.y,
	//	right.z, up.z, forward.z, position.z,
	//	0.0f, 0.0f, 0.0f, 1.0f
	//);
}