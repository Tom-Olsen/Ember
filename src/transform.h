#pragma once
#ifndef __INCLUDE_GUARD_transform_h__
#define __INCLUDE_GUARD_transform_h__
#include "mathf.h"
#include "component.h"



/// <summary>
/// Righthanded system:
/// +X = (1,0,0) is right
/// +Y = (0,1,0) is up
/// +Z = (0,0,1) is forward
/// Default rotation order is Y->X->Z = yaw->pitch->roll = RotZ * RotX * RotY = 102.
/// </summary>
class Transform : public Component
{
private: // Members:
	Float3 position;
	Float3x3 rotationMatrix;
	Float3 scale;
	Float4x4 localToWorldMatrix;
	Float4x4 worldToLocalMatrix;
	Float4x4 normalMatrix;
	bool updateLocalToWorldMatrix;

public: // Methods:
	Transform();
	Transform(const Float3& position, const Float3x3& rotationMatrix = Float3x3::identity, const Float3& scale = Float3::one);
	~Transform();

	// Setters:
	void SetPosition(float x, float y, float z);
	void SetPosition(const Float3& position);
	void SetRotationMatrix(const Float3x3& rotationMatrix);
	void SetRotationEulerDegrees(float degreesX, float degreesY, float degreesZ, Uint3 rotationOrder = Uint3(1,0,2), CoordinateSystem system = CoordinateSystem::local);
	void SetRotationEulerRadians(float radiansX, float radiansY, float radiansZ, Uint3 rotationOrder = Uint3(1,0,2), CoordinateSystem system = CoordinateSystem::local);
	void SetRotationEulerDegrees(Float3 degrees, Uint3 rotationOrder = Uint3(1,0,2), CoordinateSystem system = CoordinateSystem::local);
	void SetRotationEulerRadians(Float3 radians, Uint3 rotationOrder = Uint3(1,0,2), CoordinateSystem system = CoordinateSystem::local);
	void SetScale(float scale);
	void SetScale(float x, float y, float z);
	void SetScale(const Float3& scale);

	// Getters:
	Float3 GetPosition() const;
	Float3x3 GetRotation3x3() const;
	Float4x4 GetRotation4x4() const;
	Float3 GetScale() const;
	Float4x4 GetLocalToWorldMatrix();
	Float4x4 GetWorldToLocalMatrix();
	Float4x4 GetNormalMatrix();
	Float3 GetForward();
	Float3 GetBackward();
	Float3 GetRight();
	Float3 GetLeft();
	Float3 GetUp();
	Float3 GetDown();

	// Overrides:
	std::string ToString() const override;
private: // Methods:
	void UpdateLocalToWorldMatrix();
};



#endif // __INCLUDE_GUARD_transform_h__