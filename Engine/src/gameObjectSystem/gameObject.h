#ifndef __INCLUDE_GUARD_gameObject_h__
#define __INCLUDE_GUARD_gameObject_h__
#include "component.h"
#include "logger.h"
#include "scene.h"
#include "transform.h"
#include <memory>
#include <string>
#include <typeindex>
#include <unordered_map>



namespace emberEngine
{
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
		Transform* m_pTransform;	// not smart pointer, as this is only a reference to the unique_ptr<Transform> in the m_components container.
		Scene* m_pScene;
		// Container for storing components, mapped by their type => each type of component can only be added once
		std::unordered_map<std::type_index, std::unique_ptr<Component>> m_components;

	public: // Methods:
		GameObject(const std::string& name) : m_name(name), isActive(true), m_pScene(nullptr)
		{
			m_pTransform = new Transform(Float3(0.0f), Float3x3::identity, Float3(1.0f));
			AddComponent(m_pTransform);
		}
		~GameObject()
		{

		}

		// Setters:
		void SetScene(Scene* pScene)
		{
			m_pScene = pScene;
		}
		template <typename T>
		void AddComponent(T* pComponent)
		{
			// Check if T is a Component:
			static_assert(std::is_base_of<Component, T>::value, "T is not a Component.");

			// Check if component is already attached to a gameObject:
			if (pComponent->GetGameObject() != nullptr)
			{
				LOG_WARN("Component '{}' is already attached to a gameObject!", std::string(typeid(T).name()));
				return;
			}

			// Add component to gameObject if not already attached:
			if (m_components.find(typeid(T)) == m_components.end())
			{
				m_components.emplace(typeid(T), std::unique_ptr<T>(pComponent));
				pComponent->SetGameObject(this);
				pComponent->SetTransform(m_pTransform);
			}
			else
				LOG_WARN("GameObject '{}' already owns a '{}' component!", m_name, std::string(typeid(T).name()));
		}
		// AddComponent by forwarding args (construct in-place):
		template <typename T, typename... Args>
		T* AddComponent(Args&&... args)
		{
			static_assert(std::is_base_of<Component, T>::value, "T must derive from Component.");
			auto pComponent = new T(std::forward<Args>(args)...);
			AddComponent(pComponent);
			return pComponent;
		}

		// Getters:
		const std::string& GetName() const
		{
			return m_name;
		}
		Transform* const GetTransform() const
		{
			return m_pTransform;
		}
		Scene* const GetScene() const
		{
			return m_pScene;
		}
		template <typename T>
		T* GetComponent()
		{
			static_assert(std::is_base_of<Component, T>::value, "T is not a Component.");
			auto it = m_components.find(typeid(T));
			if (it != m_components.end())
				return dynamic_cast<T*>(it->second.get());
			return nullptr;
		}
		const std::unordered_map<std::type_index, std::unique_ptr<Component>>& GetComponents() const
		{
			return m_components;
		}

		// Debugging:
		void PrintComponents() const
		{
			LOG_TRACE("Components attached to: {}", m_name);
			for (const auto& pair : m_components)
				LOG_TRACE(pair.second->ToString());
		}
	};
}



#endif // __INCLUDE_GUARD_gameObject_h__