#include "directionalLight.h"
#include "shadowRenderPass.h"
#include "logger.h"



//	Static members:
Float4x4 DirectionalLight::rotateToY = Float4x4::RotateFromTo(Float3::down, Float3::forward);



// Constructor:
DirectionalLight::DirectionalLight(const Float4& color)
{
	this->intensity = 1.0f;
	this->color = Float4(color.x, color.y, color.z, 1.0f);
	this->nearClip = 0.1f;
	this->farClip = 100.0f;
	this->viewWidth = 20.0f;
	this->viewHeight = 20.0f;
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
	this->color = Float4(color, 1.0f);
}
void DirectionalLight::SetColor(const Float4& color)
{
	this->color = Float4(color.x, color.y, color.z, 1.0f);
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
Float4 DirectionalLight::GetColor() const
{
	return color;
}
Float4x4 DirectionalLight::GetViewMatrix()
{
	return rotateToY * gameObject->transform->GetWorldToLocalMatrix();
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