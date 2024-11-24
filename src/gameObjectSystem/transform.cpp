#include "transform.h"
#include "logger.h"



// Constructors:
Transform::Transform()
{
	this->position = Float3(0.0f);
	this->rotationMatrix = Float3x3::identity;
	this->scale = Float3(1.0f);
	updateLocalToWorldMatrix = true;
}
Transform::Transform(const Float3& position, const Float3x3& rotationMatrix, const Float3& scale)
{
	this->position = position;
	this->rotationMatrix = rotationMatrix;
	this->scale = scale;
	updateLocalToWorldMatrix = true;
}



// Destructor:
Transform::~Transform()
{

}



// Public:
// Setters:
void Transform::SetPosition(float x, float y, float z)
{
	Float3 position(x, y, z);
	if (this->position == position)
		return;
	this->position = position;
	updateLocalToWorldMatrix = true;
}
void Transform::SetPosition(const Float3& position)
{
	if (this->position == position)
		return;
	this->position = position;
	updateLocalToWorldMatrix = true;
}
void Transform::AddToPosition(float x, float y, float z)
{
	Float3 position(x, y, z);
	if (position == Float3::zero)
		return;
	this->position += position;
	updateLocalToWorldMatrix = true;
}
void Transform::AddToPosition(const Float3& position)
{
	if (position == Float3::zero)
		return;
	this->position += position;
	updateLocalToWorldMatrix = true;
}
void Transform::SetRotationMatrix(const Float3x3& rotationMatrix)
{
	if (this->rotationMatrix == rotationMatrix)
		return;
	this->rotationMatrix = rotationMatrix;
	updateLocalToWorldMatrix = true;
}
void Transform::SetRotationEulerDegrees(float degreesX, float degreesY, float degreesZ, Uint3 rotationOrder, CoordinateSystem system)
{
	Float3 eulerRadians = Float3(degreesX, degreesY, degreesZ).ToRadians();
	SetRotationMatrix(Float3x3::Rotate(eulerRadians, rotationOrder, system));
}
void Transform::SetRotationEulerRadians(float radiansX, float radiansY, float radiansZ, Uint3 rotationOrder, CoordinateSystem system)
{
	Float3 eulerRadians = Float3(radiansX, radiansY, radiansZ);
	SetRotationMatrix(Float3x3::Rotate(eulerRadians, rotationOrder, system));
}
void Transform::SetRotationEulerDegrees(Float3 degrees, Uint3 rotationOrder, CoordinateSystem system)
{
	Float3 eulerRadians = degrees.ToRadians();
	SetRotationMatrix(Float3x3::Rotate(eulerRadians, rotationOrder, system));
}
void Transform::SetRotationEulerRadians(Float3 radians, Uint3 rotationOrder, CoordinateSystem system)
{
	SetRotationMatrix(Float3x3::Rotate(radians, rotationOrder, system));
}
void Transform::SetScale(float x, float y, float z)
{
	Float3 scale(x, y, z);
	if (this->scale == scale)
		return;
	this->scale = scale;
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
void Transform::SetLocalToWorldMatrix(const Float4x4& localToWorldMatrix)
{
	this->localToWorldMatrix = localToWorldMatrix;
	position = localToWorldMatrix.GetTranslation();
	scale = localToWorldMatrix.GetScale();
	rotationMatrix = localToWorldMatrix.GetRotation3x3(scale);
	worldToLocalMatrix = localToWorldMatrix.Inverse();
	normalMatrix = worldToLocalMatrix.Transpose();
	updateLocalToWorldMatrix = false;
}



// Getters:
Float3 Transform::GetPosition() const
{
	return position;
}
Float3x3 Transform::GetRotation3x3() const
{
	return rotationMatrix;
}
Float4x4 Transform::GetRotation4x4() const
{
	return Float4x4(rotationMatrix);
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
{// +z direction
	if (updateLocalToWorldMatrix)
		UpdateLocalToWorldMatrix();
	return Float3(localToWorldMatrix.GetColumn(2));
}
Float3 Transform::GetBackward()
{// -z direction
	if (updateLocalToWorldMatrix)
		UpdateLocalToWorldMatrix();
	return -Float3(localToWorldMatrix.GetColumn(2));
}
Float3 Transform::GetRight()
{// +x direction
	if (updateLocalToWorldMatrix)
		UpdateLocalToWorldMatrix();
	return Float3(localToWorldMatrix.GetColumn(0));
}
Float3 Transform::GetLeft()
{// -x direction
	if (updateLocalToWorldMatrix)
		UpdateLocalToWorldMatrix();
	return -Float3(localToWorldMatrix.GetColumn(0));
}
Float3 Transform::GetUp()
{// +y direction
	if (updateLocalToWorldMatrix)
		UpdateLocalToWorldMatrix();
	return Float3(localToWorldMatrix.GetColumn(1));
}
Float3 Transform::GetDown()
{// -y direction
	if (updateLocalToWorldMatrix)
		UpdateLocalToWorldMatrix();
	return -Float3(localToWorldMatrix.GetColumn(1));
}



// Private:
void Transform::UpdateLocalToWorldMatrix()
{
	updateLocalToWorldMatrix = false;
	localToWorldMatrix = Float4x4::Translate(position) * Float4x4(rotationMatrix) * Float4x4::Scale(scale);
	worldToLocalMatrix = localToWorldMatrix.Inverse();
	normalMatrix = worldToLocalMatrix.Transpose();
}



// Overrides:
std::string Transform::ToString() const
{
	return "Transform";
}