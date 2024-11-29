#include "pointLight.h"
#include "shadowRenderPass.h"
#include "gameObject.h"
#include "logger.h"



// Static members:
bool PointLight::rotationMatricesInitialized = false;
Float4x4 PointLight::rotationMatrices[6];



// Constructor:
PointLight::PointLight()
{
	this->intensity = 1.0f;
	this->nearClip = 0.1f;
	this->farClip = 15.0f;
	updateProjectionMatrix = true;

	if (!rotationMatricesInitialized)
	{
		rotationMatricesInitialized = true;
		rotationMatrices[0] = Float4x4::identity;						// look backward
		rotationMatrices[1] = Float4x4::RotateY(mathf::PI_2);	// look right
		rotationMatrices[2] = Float4x4::RotateY(mathf::PI);		// look forward
		rotationMatrices[3] = Float4x4::RotateY(-mathf::PI_2);	// look left
		rotationMatrices[4] = Float4x4::RotateX(mathf::PI_2);	// look down
		rotationMatrices[5] = Float4x4::RotateX(-mathf::PI_2);	// look up
	}
}



// Destructor:
PointLight::~PointLight()
{

}



// Setters:
void PointLight::SetIntensity(const float& intensity)
{
	this->intensity = intensity;
}
void PointLight::SetColor(const Float3& color)
{
	this->color = color;
}
void PointLight::SetNearClip(const float& nearClip)
{
	this->nearClip = nearClip;
	updateProjectionMatrix = true;
}
void PointLight::SetFarClip(const float& farClip)
{
	this->farClip = farClip;
	updateProjectionMatrix = true;
}



// Getters:
Float3 PointLight::GetPosition() const
{
	return gameObject->transform->GetPosition();
}
float PointLight::GetIntensity() const
{
	return intensity;
}
Float3 PointLight::GetColor() const
{
	return color;
}
Float4 PointLight::GetColorIntensity() const
{
	return Float4(color, intensity);
}
float PointLight::GetNearClip() const
{
	return nearClip;
}
float PointLight::GetFarClip() const
{
	return farClip;
}
Float4x4 PointLight::GetViewMatrix(uint32_t faceIndex) const
{
	if (faceIndex >= 6)
		throw std::runtime_error("PointLight::GetViewMatrix(uint32_t) faceIndex out of range!");
	return rotationMatrices[faceIndex] * gameObject->transform->GetWorldToLocalMatrix();
}
Float4x4 PointLight::GetProjectionMatrix()
{
	if (updateProjectionMatrix && isActive && gameObject->isActive)
		UpdateProjectionMatrix();
	return projectionMatrix;
}



// Private:
void PointLight::UpdateProjectionMatrix()
{
	updateProjectionMatrix = false;
	constexpr float fovRadians = mathf::PI_2;
	constexpr float aspectRatio = 1.0f;
	projectionMatrix = Float4x4::Perspective(fovRadians, aspectRatio, nearClip, farClip);
}



// Overrides:
std::string PointLight::ToString() const
{
	return "PointLight";
}