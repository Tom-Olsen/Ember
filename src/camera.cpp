#include "camera.h"
#include "logger.h"
#include "gameObject.h"



//	Static members:
// rotation is applied after worldToLocal matrix => -90degrees instead of +90
Float4x4 Camera::rotateToY = Float4x4::RotateX(mathf::ToRadians(-90));



// Constructor:
Camera::Camera()
{
	this->fovRadians = mathf::ToRadians(60.0f);
	this->aspectRatio = 16.0f / 9.0f;	// 1920x1080
	this->nearClip = 0.1f;
	this->farClip = 100.0f;
	updateProjectionMatrix = true;
}



// Destructor:
Camera::~Camera()
{

}



// Setters:
void Camera::SetFovDegrees(const float& fovDegrees)
{
	this->fovRadians = mathf::ToRadians(fovDegrees);
	updateProjectionMatrix = true;
}
void Camera::SetFovRadians(const float& fovRadians)
{
	this->fovRadians = fovRadians;
	updateProjectionMatrix = true;
}
void Camera::SetAspectRatio(const float& aspectRatio)
{
	this->aspectRatio = aspectRatio;
	updateProjectionMatrix = true;
}
void Camera::SetNearClip(const float& nearClip)
{
	this->nearClip = nearClip;
	updateProjectionMatrix = true;
}
void Camera::SetFarClip(const float& farClip)
{
	this->farClip = farClip;
	updateProjectionMatrix = true;
}



// Getters:
Float4x4 Camera::GetViewMatrix()
{
	return rotateToY * gameObject->transform->GetWorldToLocalMatrix();
}
Float4x4 Camera::GetProjectionMatrix()
{
	if (updateProjectionMatrix && isActive && gameObject->isActive)
		UpdateProjectionMatrix();
	return projectionMatrix;
}



// Private:
void Camera::UpdateProjectionMatrix()
{
	updateProjectionMatrix = false;
	projectionMatrix = Float4x4::Perspective(fovRadians, aspectRatio, nearClip, farClip);
}



// Overrides:
std::string Camera::ToString() const
{
	return "Camera";
}