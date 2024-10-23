#pragma once
#ifndef __INCLUDE_GUARD_component_h__
#define __INCLUDE_GUARD_component_h__
#include <string>
#include "time.h"



// Forward declarations
class GameObject;



/// <summary>
/// Components must be attached to GameObjects.
/// Each component can only be attached to one GameObject.
/// Each GameObject can have multiple components.
/// Each GameObject can only have one component of each type.
/// </summary>
class Component
{
public: // Members:
	GameObject* gameObject;
	bool isActive;

public: // Methods:
	Component();
	virtual ~Component();

	/// <summary>
	/// Check if the Component and its parent GameObject is active.
	/// </summary>
	/// <returns></returns>
	bool IsActive();
	virtual void Update();

	// Pure virtual method that must be implemented by derived classes:
    virtual std::string ToString() const = 0;
};


#endif // __INCLUDE_GUARD_component_h__