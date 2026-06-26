#include "transform.h"



namespace emberEcs
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
	void Transform::SetRotationEulerDegrees(float degreesX, float degreesY, float degreesZ, Uint3 rotationOrder, CoordinateSpace space)
	{
		Float3 eulerRadians = math::deg2rad * Float3(degreesX, degreesY, degreesZ);
		SetRotationMatrix(Float3x3::Rotate(eulerRadians, rotationOrder, space));
	}
	void Transform::SetRotationEulerRadians(float radiansX, float radiansY, float radiansZ, Uint3 rotationOrder, CoordinateSpace space)
	{
		Float3 eulerRadians = Float3(radiansX, radiansY, radiansZ);
		SetRotationMatrix(Float3x3::Rotate(eulerRadians, rotationOrder, space));
	}
	void Transform::SetRotationEulerDegrees(Float3 degrees, Uint3 rotationOrder, CoordinateSpace space)
	{
		Float3 eulerRadians = math::deg2rad * degrees;
		SetRotationMatrix(Float3x3::Rotate(eulerRadians, rotationOrder, space));
	}
	void Transform::SetRotationEulerRadians(Float3 radians, Uint3 rotationOrder, CoordinateSpace space)
	{
		SetRotationMatrix(Float3x3::Rotate(radians, rotationOrder, space));
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
	Float4x4 Transform::GetPosition4x4() const
    {
        return Float4x4::Translate(m_position);
    }
    Float3 Transform::GetRotation() const
    {
        // R = RotY(y) * RotX(x) * RotZ(z), column-major: data[col*3 + row]
        // R[1][2] = data[7] = -sin(x)
        float sinX = math::Clamp(-m_rotationMatrix.data[7], -1.0f, 1.0f);
        float x = math::Asin(sinX);
        float cosX = math::Sqrt(1.0f - sinX * sinX);

        float y, z;
        if (cosX > math::absEpsilon)
        {
            // R[0][2]=data[6]=sy*cx, R[2][2]=data[8]=cy*cx -> y
            // R[1][0]=data[1]=cx*sz, R[1][1]=data[4]=cx*cz -> z
            y = math::Atan2(m_rotationMatrix.data[6], m_rotationMatrix.data[8]);
            z = math::Atan2(m_rotationMatrix.data[1], m_rotationMatrix.data[4]);
        }
        else
        {
            // Gimbal lock: set z=0, recover y from R[0][0]=data[0] and R[0][1]=data[3]
            // sx= 1 (x= pi/2): y-z = atan2(data[3], data[0])
            // sx=-1 (x=-pi/2): y+z = atan2(-data[3], data[0])
            float sign = (sinX >= 0.0f) ? 1.0f : -1.0f;
            y = math::Atan2(sign * m_rotationMatrix.data[3], m_rotationMatrix.data[0]);
            z = 0.0f;
        }

        return math::rad2deg * Float3(x, y, z);
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
    Float3x3 Transform::GetScale3x3() const
    {
        return Float3x3::Scale(m_scale);
    }
    Float4x4 Transform::GetScale4x4() const
    {
        return Float4x4::Scale(m_scale);
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



	// Hierarchy:
	void Transform::SetParent(Transform* pParent)
	{
		m_parent = pParent;
	}
	Transform* Transform::GetParent()
	{
		return m_parent;
	}
	void Transform::AddChild(Transform* pChild)
	{
		m_children.push_back(pChild);
	}
	void Transform::RemoveChild(Transform* pChild)
	{
		m_children.erase(std::remove(m_children.begin(), m_children.end(), pChild), m_children.end());
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
}