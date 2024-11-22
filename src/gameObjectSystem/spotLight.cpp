#include "spotLight.h"
#include "shadowRenderPass.h"
#include "gameObject.h"
#include "logger.h"



// Constructor:
SpotLight::SpotLight()
{
	this->intensity = 1.0f;
	this->fovRadians = mathf::ToRadians(45.0f);
	this->aspectRatio = (float)ShadowRenderPass::shadowMapWidth / (float)ShadowRenderPass::shadowMapHeight;
	this->nearClip = 0.1f;
	this->farClip = 15.0f;
	updateProjectionMatrix = true;
}



// Destructor:
SpotLight::~SpotLight()
{

}



// Setters:
void SpotLight::SetIntensity(const float& intensity)
{
	this->intensity = std::clamp(intensity, 0.0f, 1.0f);
}
void SpotLight::SetColor(const Float3& color)
{
	this->color = color;
}
void SpotLight::SetFovDegrees(const float& fovDegrees)
{
	this->fovRadians = mathf::ToRadians(fovDegrees);
	updateProjectionMatrix = true;
}
void SpotLight::SetFovRadians(const float& fovRadians)
{
	this->fovRadians = fovRadians;
	updateProjectionMatrix = true;
}
void SpotLight::SetNearClip(const float& nearClip)
{
	this->nearClip = nearClip;
	updateProjectionMatrix = true;
}
void SpotLight::SetFarClip(const float& farClip)
{
	this->farClip = farClip;
	updateProjectionMatrix = true;
}



// Getters:
Float3 SpotLight::GetDirection() const
{
	return gameObject->transform->GetForward();
}
float SpotLight::GetIntensity() const
{
	return intensity;
}
Float3 SpotLight::GetColor() const
{
	return color;
}
Float4 SpotLight::GetColorIntensity() const
{
	return Float4(color, intensity);
}
float SpotLight::GetFovDegrees() const
{
	return mathf::ToDegrees(fovRadians);
}
float SpotLight::GetFovRadians() const
{
	return fovRadians;
}
float SpotLight::GetNearClip() const
{
	return nearClip;
}
float SpotLight::GetFarClip() const
{
	return farClip;
}
Float4x4 SpotLight::GetViewMatrix() const
{
	return gameObject->transform->GetWorldToLocalMatrix();
}
Float4x4 SpotLight::GetProjectionMatrix()
{
	if (updateProjectionMatrix && isActive && gameObject->isActive)
		UpdateProjectionMatrix();
	return projectionMatrix;
}



// Private:
void SpotLight::UpdateProjectionMatrix()
{
	updateProjectionMatrix = false;
	projectionMatrix = Float4x4::Perspective(fovRadians, aspectRatio, nearClip, farClip);
}



// Overrides:
std::string SpotLight::ToString() const
{
	return "SpotLight";
}