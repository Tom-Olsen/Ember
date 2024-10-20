#pragma once
#ifndef __INCLUDE_GUARD_component_h__
#define __INCLUDE_GUARD_component_h__



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
	GameObject* gameObject = nullptr;

public: // Methods:
	virtual ~Component() = default;

	// Pure virtual method that must be implemented by derived classes:
    virtual void PrintType() const = 0;
};


#endif // __INCLUDE_GUARD_component_h__