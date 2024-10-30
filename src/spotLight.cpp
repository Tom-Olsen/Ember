#include "spotLight.h"
#include "shadowRenderPass.h"
#include "logger.h"



// Constructor:
SpotLight::SpotLight()
{
	this->intensity = 1.0f;
	this->fovRadians = mathf::ToRadians(45.0f);
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
void SpotLight::SetFovDegrees(const float& fovDegrees)
{
	this->fovRadians = mathf::ToRadians(fovDegrees);
	this->aspectRatio = (float)ShadowRenderPass::shadowMapWidth / (float)ShadowRenderPass::shadowMapHeight;
	this->nearClip = 0.1f;
	this->farClip = 10.0f;
	updateProjectionMatrix = true;
}
void SpotLight::SetFovRadians(const float& fovRadians)
{
	this->fovRadians = fovRadians;
	this->aspectRatio = (float)ShadowRenderPass::shadowMapWidth / (float)ShadowRenderPass::shadowMapHeight;
	this->nearClip = 0.1f;
	this->farClip = 10.0f;
	updateProjectionMatrix = true;
}



// Getters:
Float4x4 SpotLight::GetViewMatrix()
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