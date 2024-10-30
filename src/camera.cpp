#include "camera.h"
#include "logger.h"
#include "gameObject.h"



//	Static members:
Float4x4 Camera::rotateToY = glm::rotate(glm::rotate(Float4x4(1.0f), glm::radians(90.0f), Float3(1.0f, 0.0f, 0.0f)), glm::radians(180.0f), Float3(0.0f, 0.0f, 1.0f));
//Float4x4 Camera::rotateToY = glm::rotate(glm::rotate(Float4x4(1.0f), glm::radians(0.0f), Float3(0.0f, 0.0f, 1.0f)), glm::radians(90.0f), Float3(1.0f, 0.0f, 0.0f));



// Constructor:
Camera::Camera()
{
	this->fov = 60.0f;
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
	return gameObject->transform->GetWorldToLocalMatrix() * rotateToY;
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
std::string Camera::ToString() const
{
	return "Camera";
}