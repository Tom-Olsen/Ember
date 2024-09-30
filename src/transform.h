#pragma once
#ifndef __INCLUDE_GUARD_transform_h__
#define __INCLUDE_GUARD_transform_h__
#include "glmTypes.h"



class Transform
{
private: // Members:
	Float3 position;
	Float3 eulerAngles;	// in degrees
	Float3 scale;
	Float4x4 localToWorldMatrix;
	Float4x4 worldToLocalMatrix;
	bool updateLocalToWorldMatrix;

public: // Methods:
	Transform(const Float3& position, const Float3& eulerAngles, const Float3& scale);
	~Transform();

	// Getters:
	Float3 GetPosition() const;
	Float3 GetEulerAngles() const;
	Float3 GetScale() const;
	Float4x4 GetLocalToWorldMatrix();
	Float4x4 GetWorldToLocalMatrix();

	// Setters:
	void SetPosition(const Float3& position);
	void SetEulerAngles(const Float3& eulerAngles);
	void SetScale(const Float3& scale);

private: // Methods:
	void UpdateLocalToWorldMatrix();
};



#endif // __INCLUDE_GUARD_transform_h__