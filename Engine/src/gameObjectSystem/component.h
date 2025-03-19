#ifndef __INCLUDE_GUARD_component_h__
#define __INCLUDE_GUARD_component_h__
#include <string>



namespace emberEngine
{
	// Forward declarations:
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

	protected: // Members:
		GameObject* m_pGameObject;
		Transform* m_pTransform;

	public: // Methods:
		Component();
		virtual ~Component();

		// Getters:
		GameObject* const GetGameObject() const;
		Transform* const GetTransform() const;

		// Setters:
		void SetGameObject(GameObject* pGameObject);
		void SetTransform(Transform* pTransform);

		// Check if the Component and its parent GameObject is active.
		bool IsActive();
		virtual void Start();
		virtual void Update();
		virtual void LateUpdate();

		// Pure virtual method that must be implemented by derived classes:
		virtual const std::string ToString() const = 0;
	};
}



#endif // __INCLUDE_GUARD_component_h__