#ifndef __INCLUDE_GUARD_gameObject_h__
#define __INCLUDE_GUARD_gameObject_h__
#include <memory>
#include <string>
#include <typeindex>
#include <unordered_map>



class Component;
class Scene;
class Transform;



/// <summary>
/// GameObject owns all Components that are attached to it.
/// For now every type of Component can only ever be attached once to a GameObject.
/// Every Component script need explicit instantiation of AddComponent and GetComponent at the end of the gameObject.cpp file.
/// Every Component script must be included in the emberEngine.h file.
/// </summary>
class GameObject
{
public: // Members:
	bool isActive;

private: // Members:
	std::string m_name;
	Transform* m_pTransform;
	Scene* m_pScene;
	// Container for storing components, mapped by their type
	// => each type of component can only be added once
	std::unordered_map<std::type_index, std::unique_ptr<Component>> m_components;

public: // Methods:
	GameObject(const std::string& name = "");
	~GameObject();

	// Setters:
	void SetScene(Scene* pScene);
	template <typename T>
	void AddComponent(T* pComponent);

	// Getters:
	const std::string& GetName() const;
	Transform* const GetTransform() const;
	Scene* const GetScene() const;
	template <typename T>
	T* GetComponent();
	const std::unordered_map<std::type_index, std::unique_ptr<Component>>& GetComponents() const;

	// Debugging:
	void PrintComponents() const;
};



#endif // __INCLUDE_GUARD_gameObject_h__