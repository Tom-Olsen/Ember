#ifndef __INCLUDE_GUARD_component_h__
#define __INCLUDE_GUARD_component_h__
#include <string>



class GameObject;
class Scene;
class Transform;



/// <summary>
/// Components must be attached to GameObjects.
/// Each component can only be attached to one GameObject.
/// Each GameObject can have multiple components.
/// Each GameObject can only have one component of each type.
/// </summary>
class Component
{
public: // Members:
	bool isActive;

private: // Members:
	GameObject* m_pGameObject;
	Transform* m_pTransform;
	Scene* m_pScene;

public: // Methods:
	Component();
	virtual ~Component();

	// Getters:
	GameObject* const GetGameObject() const;
	Transform* const GetTransform() const;
	Scene* const GetScene() const;

	// Setters:
	void SetGameObject(GameObject* pGameObject);
	void SetTransform(Transform* pTransform);
	void SetScene(Scene* pScene);

	// Check if the Component and its parent GameObject is active.
	bool IsActive();
	virtual void Update();

	// Pure virtual method that must be implemented by derived classes:
    virtual const std::string ToString() const = 0;
};



#endif // __INCLUDE_GUARD_component_h__