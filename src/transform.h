#pragma once
#ifndef __INCLUDE_GUARD_transform_h__
#define __INCLUDE_GUARD_transform_h__
#include "glmTypes.h"
#include "component.h"



class Transform : public Component
{
private: // Members:
	Float3 position;
	Float3 eulerAngles;	// in degrees
	Float3 scale;
	Float4x4 localToWorldMatrix;
	Float4x4 worldToLocalMatrix;
	bool updateLocalToWorldMatrix;

public: // Methods:
	Transform();
	Transform(const Float3& position, const Float3& eulerAngles, const Float3& scale);
	~Transform();

	// Setters:
	void SetPosition(const Float3& position);
	void SetEulerAngles(const Float3& eulerAngles);
	void SetScale(const float& scale);
	void SetScale(const Float3& scale);

	// Getters:
	Float3 GetPosition() const;
	Float3 GetEulerAngles() const;
	Float3 GetScale() const;
	Float4x4 GetLocalToWorldMatrix();
	Float4x4 GetWorldToLocalMatrix();
	Float3 GetForward() const;
	Float3 GetRight() const;
	Float3 GetUp() const;

	// Overrides:
	void PrintType() const override;
private: // Methods:
	void UpdateLocalToWorldMatrix();
};



#endif // __INCLUDE_GUARD_transform_h__