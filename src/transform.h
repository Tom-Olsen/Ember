#pragma once
#ifndef __INCLUDE_GUARD_transform_h__
#define __INCLUDE_GUARD_transform_h__
#include "mathf.h"
#include "component.h"
#include "mathf.h"



/// <summary>
/// Righthanded system:
/// +X = (1,0,0) is right
/// +Y = (0,1,0) is forward
/// +Z = (0,0,1) is up
/// Default rotation order is YXZ = roll, pitch, yaw.
/// </summary>
class Transform : public Component
{
private: // Members:
	Float3 position;
	Float3 eulerRadians;
	Int3 rotationOrder;
	Float3 scale;
	Float4x4 localToWorldMatrix;
	Float4x4 worldToLocalMatrix;
	Float4x4 normalMatrix;
	bool updateLocalToWorldMatrix;

public: // Methods:
	Transform();
	Transform(const Float3& position, const Float3& eulerAngles, const Float3& scale);
	~Transform();

	// Setters:
	void SetPosition(const Float3& position);
	void SetEulerDegrees(const Float3& eulerDegrees);
	void SetEulerRadians(const Float3& eulerRadians);
	void SetRotationOrder(const Int3& rotationOrder);
	void SetScale(float scale);
	void SetScale(const Float3& scale);

	// Getters:
	Float3 GetPosition() const;
	Float3 GetEulerDegrees() const;
	Float3 GetEulerRadians() const;
	Float3 GetScale() const;
	Float4x4 GetLocalToWorldMatrix();
	Float4x4 GetWorldToLocalMatrix();
	Float4x4 GetNormalMatrix();
	Float3 GetForward();
	Float3 GetRight();
	Float3 GetUp();

	// Overrides:
	std::string ToString() const override;
private: // Methods:
	void UpdateLocalToWorldMatrix();
};



#endif // __INCLUDE_GUARD_transform_h__