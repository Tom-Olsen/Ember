#include "transform.h"
#include "logger.h"



// Constructors:
Transform::Transform()
{
	this->position = Float3(0.0f);
	this->eulerRadians = Float3(0.0f);
	this->rotationOrder = Int3(1, 0, 2);
	this->scale = Float3(1.0f);
	updateLocalToWorldMatrix = true;
}
Transform::Transform(const Float3& position, const Float3& eulerDegrees, const Float3& scale)
{
	this->position = position;
	this->eulerRadians = eulerDegrees.ToRadians();
	this->rotationOrder = Int3(1, 0, 2);
	this->scale = scale;
	updateLocalToWorldMatrix = true;
}



// Destructor:
Transform::~Transform()
{

}



// Public:
// Setters:
void Transform::SetPosition(const Float3& position)
{
	if (this->position == position)
		return;
	this->position = position;
	updateLocalToWorldMatrix = true;
}
void Transform::SetEulerDegrees(const Float3& eulerDegrees)
{
	Float3 eulerRadians = eulerDegrees.ToRadians();
	if (this->eulerRadians == eulerRadians)
		return;
	this->eulerRadians = eulerRadians;
	updateLocalToWorldMatrix = true;
}
void Transform::SetEulerRadians(const Float3& eulerRadians)
{
	if (this->eulerRadians == eulerRadians)
		return;
	this->eulerRadians = eulerRadians;
	updateLocalToWorldMatrix = true;
}
void Transform::SetRotationOrder(const Int3& rotationOrder)
{
	if (this->rotationOrder == rotationOrder)
		return;
	this->rotationOrder = rotationOrder;
	updateLocalToWorldMatrix = true;
}
void Transform::SetScale(const Float3& scale)
{
	if (this->scale == scale)
		return;
	this->scale = scale;
	updateLocalToWorldMatrix = true;
}
void Transform::SetScale(float scale)
{
	SetScale(Float3(scale));
}



// Getters:
Float3 Transform::GetPosition() const
{
	return position;
}
Float3 Transform::GetEulerDegrees() const
{
	return eulerRadians.ToDegrees();
}
Float3 Transform::GetEulerRadians() const
{
	return eulerRadians;
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
Float4x4 Transform::GetNormalMatrix()
{
	if (updateLocalToWorldMatrix)
		UpdateLocalToWorldMatrix();
	return normalMatrix;
}
Float3 Transform::GetForward()
{// +y direction
	if (updateLocalToWorldMatrix)
		UpdateLocalToWorldMatrix();
	return Float3(localToWorldMatrix.GetColumn(1));
}
Float3 Transform::GetRight()
{// +x direction
	if (updateLocalToWorldMatrix)
		UpdateLocalToWorldMatrix();
	return Float3(localToWorldMatrix.GetColumn(0));
}
Float3 Transform::GetUp()
{// +z direction
	if (updateLocalToWorldMatrix)
		UpdateLocalToWorldMatrix();
	return Float3(localToWorldMatrix.GetColumn(2));
}



// Private:
void Transform::UpdateLocalToWorldMatrix()
{
	updateLocalToWorldMatrix = false;

	// TRS matrizes:
	Float4x4 translationMatrix = Float4x4::Translate(position);
	Float4x4 rotationMatrix = Float4x4::Rotate(eulerRadians, rotationOrder);
	Float4x4 scaleMatrix = Float4x4::Scale(scale);

	// Final matrizes:
	localToWorldMatrix = translationMatrix * rotationMatrix * scaleMatrix;
	worldToLocalMatrix = localToWorldMatrix.Inverse();
	normalMatrix = worldToLocalMatrix.Transpose();
}



// Overrides:
std::string Transform::ToString() const
{
	return "Transform";
}