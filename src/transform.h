#pragma once
#ifndef __INCLUDE_GUARD_transform_h__
#define __INCLUDE_GUARD_transform_h__
#include "glmTypes.h"



class Transform
{
private: // Members:
	Float3 position;
	Float3 rotation;
	Float3 scale;
	Float4x4 localToWorldMatrix;
	Float4x4 worldToLocalMatrix;
	bool updateLocalToWorldMatrix;

public: // Methods:
	Transform(Float3 position, Float3 rotation, Float3 scale);
	~Transform();

	// Getters:
	Float3 GetPosition() const;
	Float3 GetRotation() const;
	Float3 GetScale() const;
	Float4x4 GetLocalToWorldMatrix();
	Float4x4 GetWorldToLocalMatrix();

	// Setters:
	void SetPosition(Float3 position);
	void SetRotation(Float3 rotation);
	void SetScale(Float3 scale);

private: // Methods:
	void UpdateLocalToWorldMatrix();
};



#endif // __INCLUDE_GUARD_transform_h__