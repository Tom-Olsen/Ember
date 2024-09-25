#pragma once
#ifndef __INCLUDE_GUARD_gameObject_h__
#define __INCLUDE_GUARD_gameObject_h__
#include <memory>
#include <typeindex>
#include "transform.h"
#include "component.h"
#include "camera.h"



class GameObject
{
public: // Members:
	std::shared_ptr<Transform> transform;

private: // Members:
	// Container for storing components, mapped by their type
	// => each type of component can only be added once
	std::unordered_map<std::type_index, std::shared_ptr<Component>> components;

public: // Methods:
	GameObject();
	~GameObject();
	template <typename T>
	void AddComponent(std::shared_ptr<T> component);
	template <typename T>
	void AddComponent(T* component);
	template <typename T>
	std::shared_ptr<T> GetComponent();
	void PrintComponents() const;
};



#endif // __INCLUDE_GUARD_gameObject_h__