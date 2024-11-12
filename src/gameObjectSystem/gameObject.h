#pragma once
#ifndef __INCLUDE_GUARD_gameObject_h__
#define __INCLUDE_GUARD_gameObject_h__
#include <memory>
#include <string>
#include <typeindex>
#include <unordered_map>

// Components:
#include "transform.h"
#include "camera.h"
#include "cameraController.h"
#include "meshRenderer.h"
#include "spin.h"
#include "spotLight.h"
#include "directionalLight.h"



// Forward declarations
class Component;



class GameObject
{
public: // Members:
	std::string name;
	bool isActive;
	Transform* transform;
	// Container for storing components, mapped by their type
	// => each type of component can only be added once
	std::unordered_map<std::type_index, std::unique_ptr<Component>> components;

private: // Members:

public: // Methods:
	GameObject(std::string name = "");
	~GameObject();

	template <typename T>
	void AddComponent(T* component);
	template <typename T>
	T* GetComponent();
	void PrintComponents() const;
};



#endif // __INCLUDE_GUARD_gameObject_h__