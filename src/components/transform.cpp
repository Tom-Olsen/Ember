#include "transform.h"



namespace emberEngine
{
	// Constructors/Destructor:
	Transform::Transform()
	{
		m_position = Float3(0.0f);
		m_rotationMatrix = Float3x3::identity;
		m_scale = Float3(1.0f);
		m_updateLocalToWorldMatrix = true;
	}
	Transform::Transform(const Float3& position, const Float3x3& rotationMatrix, const Float3& scale)
	{
		m_position = position;
		m_rotationMatrix = rotationMatrix;
		m_scale = scale;
		m_updateLocalToWorldMatrix = true;
	}
	Transform::~Transform()
	{

	}



	// Public methods:
	// Setters:
	void Transform::SetPosition(float x, float y, float z)
	{
		Float3 position(x, y, z);
		if (m_position == position)
			return;
		m_position = position;
		m_updateLocalToWorldMatrix = true;
	}
	void Transform::SetPosition(const Float3& position)
	{
		if (m_position == position)
			return;
		m_position = position;
		m_updateLocalToWorldMatrix = true;
	}
	void Transform::AddToPosition(float x, float y, float z)
	{
		Float3 translation(x, y, z);
		if (translation == Float3::zero)
			return;
		m_position += translation;
		m_updateLocalToWorldMatrix = true;
	}
	void Transform::AddToPosition(const Float3& translation)
	{
		if (translation == Float3::zero)
			return;
		m_position += translation;
		m_updateLocalToWorldMatrix = true;
	}
	void Transform::SetRotationMatrix(const Float3x3& rotationMatrix)
	{
		if (m_rotationMatrix == rotationMatrix)
			return;
		m_rotationMatrix = rotationMatrix;
		m_updateLocalToWorldMatrix = true;
	}
	void Transform::SetRotationEulerDegrees(float degreesX, float degreesY, float degreesZ, Uint3 rotationOrder, CoordinateSystem system)
	{
		Float3 eulerRadians = mathf::deg2rad * Float3(degreesX, degreesY, degreesZ);
		SetRotationMatrix(Float3x3::Rotate(eulerRadians, rotationOrder, system));
	}
	void Transform::SetRotationEulerRadians(float radiansX, float radiansY, float radiansZ, Uint3 rotationOrder, CoordinateSystem system)
	{
		Float3 eulerRadians = Float3(radiansX, radiansY, radiansZ);
		SetRotationMatrix(Float3x3::Rotate(eulerRadians, rotationOrder, system));
	}
	void Transform::SetRotationEulerDegrees(Float3 degrees, Uint3 rotationOrder, CoordinateSystem system)
	{
		Float3 eulerRadians = mathf::deg2rad * degrees;
		SetRotationMatrix(Float3x3::Rotate(eulerRadians, rotationOrder, system));
	}
	void Transform::SetRotationEulerRadians(Float3 radians, Uint3 rotationOrder, CoordinateSystem system)
	{
		SetRotationMatrix(Float3x3::Rotate(radians, rotationOrder, system));
	}
	void Transform::SetScale(float x, float y, float z)
	{
		Float3 scale(x, y, z);
		if (m_scale == scale)
			return;
		m_scale = scale;
		m_updateLocalToWorldMatrix = true;
	}
	void Transform::SetScale(const Float3& scale)
	{
		if (m_scale == scale)
			return;
		m_scale = scale;
		m_updateLocalToWorldMatrix = true;
	}
	void Transform::SetScale(float scale)
	{
		SetScale(Float3(scale));
	}
	void Transform::SetLocalToWorldMatrix(const Float4x4& localToWorldMatrix)
	{
		m_localToWorldMatrix = localToWorldMatrix;
		m_position = localToWorldMatrix.GetTranslation();
		m_scale = localToWorldMatrix.GetScale();
		m_rotationMatrix = localToWorldMatrix.GetRotation3x3(m_scale);
		m_worldToLocalMatrix = localToWorldMatrix.Inverse();
		m_localToWorldNormalMatrix = m_worldToLocalMatrix.Transpose();
		m_worldToLocalNormalMatrix = m_localToWorldMatrix.Transpose();
		m_updateLocalToWorldMatrix = false;
	}



	// Getters:
	Float3 Transform::GetPosition() const
	{
		return m_position;
	}
	Float3x3 Transform::GetRotation3x3() const
	{
		return m_rotationMatrix;
	}
	Float4x4 Transform::GetRotation4x4() const
	{
		return Float4x4(m_rotationMatrix);
	}
	Float3 Transform::GetScale() const
	{
		return m_scale;
	}
	Float4x4 Transform::GetLocalToWorldMatrix()
	{
		if (m_updateLocalToWorldMatrix)
			UpdateLocalToWorldMatrix();
		return m_localToWorldMatrix;
	}
	Float4x4 Transform::GetWorldToLocalMatrix()
	{
		if (m_updateLocalToWorldMatrix)
			UpdateLocalToWorldMatrix();
		return m_worldToLocalMatrix;
	}
	Float4x4 Transform::GetLocalToWorldNormalMatrix()
	{
		if (m_updateLocalToWorldMatrix)
			UpdateLocalToWorldMatrix();
		return m_localToWorldNormalMatrix;
	}
	Float4x4 Transform::GetWorldToLocalNormalMatrix()
	{
		if (m_updateLocalToWorldMatrix)
			UpdateLocalToWorldMatrix();
		return m_worldToLocalNormalMatrix;
	}
	Float3 Transform::GetRight()
	{// +x direction
		if (m_updateLocalToWorldMatrix)
			UpdateLocalToWorldMatrix();
		return Float3(m_localToWorldMatrix.GetColumn(0));
	}
	Float3 Transform::GetLeft()
	{// -x direction
		if (m_updateLocalToWorldMatrix)
			UpdateLocalToWorldMatrix();
		return -Float3(m_localToWorldMatrix.GetColumn(0));
	}
	Float3 Transform::GetForward()
	{// +y direction
		if (m_updateLocalToWorldMatrix)
			UpdateLocalToWorldMatrix();
		return Float3(m_localToWorldMatrix.GetColumn(1));
	}
	Float3 Transform::GetBack()
	{// -y direction
		if (m_updateLocalToWorldMatrix)
			UpdateLocalToWorldMatrix();
		return -Float3(m_localToWorldMatrix.GetColumn(1));
	}
	Float3 Transform::GetUp()
	{// +z direction
		if (m_updateLocalToWorldMatrix)
			UpdateLocalToWorldMatrix();
		return Float3(m_localToWorldMatrix.GetColumn(2));
	}
	Float3 Transform::GetDown()
	{// -z direction
		if (m_updateLocalToWorldMatrix)
			UpdateLocalToWorldMatrix();
		return -Float3(m_localToWorldMatrix.GetColumn(2));
	}



	// Private methods:
	void Transform::UpdateLocalToWorldMatrix()
	{
		m_updateLocalToWorldMatrix = false;
		m_localToWorldMatrix = Float4x4::Translate(m_position) * Float4x4(m_rotationMatrix) * Float4x4::Scale(m_scale);
		m_worldToLocalMatrix = m_localToWorldMatrix.Inverse();
		m_localToWorldNormalMatrix = m_worldToLocalMatrix.Transpose();
		m_worldToLocalNormalMatrix = m_localToWorldMatrix.Transpose();
	}



	// Overrides:
	const std::string Transform::ToString() const
	{
		return "Transform";
	}
}