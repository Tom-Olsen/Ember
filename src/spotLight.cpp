#include "spotLight.h"
#include "shadowRenderPass.h"
#include "logger.h"



// Constructor:
SpotLight::SpotLight()
{
	this->intensity = 1.0f;
	this->fov = 45.0f;
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
void SpotLight::SetFov(const float& fov)
{
	this->fov = fov;
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
	projectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip);
	projectionMatrix[1][1] *= -1;

	glm::mat4 light_projection = glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip);
}



// Overrides:
std::string SpotLight::ToString() const
{
	return "SpotLight";
}