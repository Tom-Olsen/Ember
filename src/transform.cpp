#include "transform.h"



// Constructor:
Transform::Transform(const Float3& position, const Float3& eulerAngles, const Float3& scale)
{
	this->position = position;
	this->eulerAngles = eulerAngles;
	this->scale = scale;
	UpdateLocalToWorldMatrix();
}



// Destructor:
Transform::~Transform()
{

}



// Public:
// Getters:
Float3 Transform::GetPosition() const
{
	return position;
}
Float3 Transform::GetEulerAngles() const
{
	return eulerAngles;
}
Float3 Transform::GetScale() const
{
	return scale;
}
Float4x4 Transform::GetLocalToWorldMatrix()
{
	if (updateLocalToWorldMatrix)
		UpdateLocalToWorldMatrix();
	return localToWorldMatrix;
}
Float4x4 Transform::GetWorldToLocalMatrix()
{
	if (updateLocalToWorldMatrix)
		UpdateLocalToWorldMatrix();
	return worldToLocalMatrix;
}



// Setters:
void Transform::SetPosition(const Float3& position)
{
	if (this->position == position)
		return;
	this->position = position;
	updateLocalToWorldMatrix = true;
}
void Transform::SetEulerAngles(const Float3& eulerAngles)
{
	if (this->eulerAngles == eulerAngles)
		return;
	this->eulerAngles = eulerAngles;
	updateLocalToWorldMatrix = true;
}
void Transform::SetScale(const Float3& scale)
{
	if (this->scale == scale)
		return;
	this->scale = scale;
	updateLocalToWorldMatrix = true;
}



// Private:
void Transform::UpdateLocalToWorldMatrix()
{
	updateLocalToWorldMatrix = false;

	// Translation:
	Float4x4 translationMatrix = glm::translate(Float4x4(1.0f), position);

	// Rotation:
	Float4x4 rotationMatrix = glm::rotate(Float4x4(1.0f), glm::radians(eulerAngles.x), Float3(1.0f, 0.0f, 0.0f));
	rotationMatrix = glm::rotate(rotationMatrix, glm::radians(eulerAngles.y), Float3(0.0f, 1.0f, 0.0f));
	rotationMatrix = glm::rotate(rotationMatrix, glm::radians(eulerAngles.z), Float3(0.0f, 0.0f, 1.0f));

	// Scale:
	Float4x4 scaleMatrix = glm::scale(Float4x4(1.0f), scale);

	// localToWorldMatrix & worldToLocalMatrix:
	localToWorldMatrix = translationMatrix * rotationMatrix * scaleMatrix;
	worldToLocalMatrix = glm::inverse(localToWorldMatrix);
}