#include "transform.h"



// Constructor:
Transform::Transform(Float3 position, Float3 rotation, Float3 scale)
{
	this->position = position;
	this->rotation = rotation;
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
Float3 Transform::GetRotation() const
{
	return rotation;
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
void Transform::SetPosition(Float3 position)
{
	if (this->position == position)
		return;
	this->position = position;
	updateLocalToWorldMatrix = true;
}
void Transform::SetRotation(Float3 rotation)
{
	if (this->rotation == rotation)
		return;
	this->rotation = rotation;
	updateLocalToWorldMatrix = true;
}
void Transform::SetScale(Float3 scale)
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
	Float4x4 rotationMatrix = glm::rotate(Float4x4(1.0f), rotation.x, Float3(1.0f, 0.0f, 0.0f));
	rotationMatrix = glm::rotate(rotationMatrix, rotation.y, Float3(0.0f, 1.0f, 0.0f));
	rotationMatrix = glm::rotate(rotationMatrix, rotation.z, Float3(0.0f, 0.0f, 1.0f));

	// Scale:
	Float4x4 scaleMatrix = glm::scale(Float4x4(1.0f), scale);

	// Local to World Matrix & World to Local Matrix:
	localToWorldMatrix = translationMatrix * rotationMatrix * scaleMatrix;
	worldToLocalMatrix = glm::inverse(localToWorldMatrix);
}