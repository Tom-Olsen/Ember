#include "directionalLight.h"
#include "gameObject.h"
#include "logger.h"



// Constructor:
DirectionalLight::DirectionalLight(const Float3& color)
{
	this->intensity = 1.0f;
	this->color = Float4(color.x, color.y, color.z);
	this->nearClip = 0.01f;
	this->farClip = 15.0f;
	this->viewWidth = 15.0f;
	this->viewHeight = 15.0f;
	updateProjectionMatrix = true;
}



// Destructor:
DirectionalLight::~DirectionalLight()
{

}



// Setters:
void DirectionalLight::SetIntensity(const float& intensity)
{
	this->intensity = std::clamp(intensity, 0.0f, 1.0f);
}
void DirectionalLight::SetColor(const Float3& color)
{
	this->color = color;
}
void DirectionalLight::SetNearClip(const float& nearClip)
{
	this->nearClip = nearClip;
	updateProjectionMatrix = true;
}
void DirectionalLight::SetFarClip(const float& farClip)
{
	this->farClip = farClip;
	updateProjectionMatrix = true;
}
void DirectionalLight::SetViewWidth(const float& viewWidth)
{
	this->viewWidth = viewWidth;
	updateProjectionMatrix = true;
}
void DirectionalLight::SetViewHeight(const float& viewHeight)
{
	this->viewHeight = viewHeight;
	updateProjectionMatrix = true;
}



// Getters:
float DirectionalLight::GetIntensity() const
{
	return intensity;
}
Float3 DirectionalLight::GetColor() const
{
	return color;
}
Float4x4 DirectionalLight::GetViewMatrix() const
{
	return gameObject->transform->GetWorldToLocalMatrix();
}
Float4x4 DirectionalLight::GetProjectionMatrix()
{
	if (updateProjectionMatrix && isActive && gameObject->isActive)
		UpdateProjectionMatrix();
	return projectionMatrix;
}



// Private:
void DirectionalLight::UpdateProjectionMatrix()
{
	updateProjectionMatrix = false;
	float left = -viewWidth / 2.0f;
	float right = viewWidth / 2.0f;
	float bottom = -viewHeight / 2.0f;
	float top = viewHeight / 2.0f;
	projectionMatrix = Float4x4::Orthographic(left, right, bottom, top, nearClip, farClip);
}



// Overrides:
std::string DirectionalLight::ToString() const
{
	return "DirectionalLight";
}