#include "camera.h"
#include "logger.h"



// Constructor:
Camera::Camera()
{
	this->fov = 60.0f;
	this->aspectRatio = 16.0f / 9.0f;	// 1920x1080
	this->nearClip = 0.1f;
	this->farClip = 10.0f;
	updateProjectionMatrix = true;
}



// Destructor:
Camera::~Camera()
{

}



// Setters:
void Camera::SetFov(const float& fov)
{
	this->fov = fov;
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
	return gameObject->transform->GetWorldToLocalMatrix();
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
	projectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip);
	projectionMatrix[1][1] *= -1;
}



// Overrides:
void Camera::PrintType() const
{
	LOG_TRACE("Camera");
}