#pragma once
#include "component.h"
#include "logger.h"
#include "scene.h"
#include "transform.h"
#include <memory>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <vector>



namespace emberEngine
{
	/// <summary>
	/// GameObject owns all Components that are attached to it.
	/// For now every type of Component can only ever be attached once to a GameObject.
	/// For automatic template creation of Add/GetComponent this class is entirely implemented in a header.
	/// </summary>
	class GameObject
	{
	public: // Members:
		bool isActive;

	private: // Members:
		std::string m_name;
		Transform* m_pTransform;	// not smart pointer, as this is only a reference to the unique_ptr<Transform> in the m_components map.
		Scene* m_pScene;
		// Container for storing components, mapped by their type => each type of component can only be added once
		std::unordered_map<std::type_index, std::unique_ptr<Component>> m_components;

		// Parent/Child logic:
		GameObject* m_pParent = nullptr;
		std::vector<GameObject*> m_children;

	public: // Methods:
		GameObject(const std::string& name) : m_name(name), isActive(true), m_pScene(nullptr)
		{
			m_pTransform = new Transform(Float3(0.0f), Float3x3::identity, Float3(1.0f));
			AddComponent(m_pTransform);
		}
		~GameObject()
		{
			// Null parents of children:
			for (GameObject* pChild : m_children)
				pChild->SetParent(nullptr);
			m_children.clear();

			// Remove self from parent's children:
			if (m_pParent)
			{
				auto& siblings = m_pParent->m_children;
				siblings.erase(std::remove(siblings.begin(), siblings.end(), this), siblings.end());
				m_pParent = nullptr;
			}
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
		void SetParent(GameObject* pParent)
		{
			// Prevent same parent:
			if (m_pParent == pParent)
				return;

			// Prevent circular parenting (and self parenting):
			if (IsAncestorOf(pParent))
				return;

			// Remove this from old parent:
			if (m_pParent)
			{
				m_pParent->RemoveChildFromList(this);
				m_pParent->GetTransform()->RemoveChild(m_pTransform);
			}

			// Connect parent to this:
			m_pParent = pParent;

			// Connect this to parent:
			if (pParent)
			{
				pParent->AddChildToList(this);
				pParent->GetTransform()->AddChild(m_pTransform);
			}

			// Update hierarchy copy in transform:
			if (pParent)
				m_pTransform->SetParent(pParent->GetTransform());
			else
				m_pTransform->SetParent(nullptr);
		}
		void AddChild(GameObject* pChild)
		{
			if (pChild == nullptr || pChild == this)
				return;
			pChild->SetParent(this);
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
		GameObject* GetParent() const
		{
			return m_pParent;
		}
		const std::vector<GameObject*>& GetChildren() const
		{
			return m_children;
		}
		
		// Debugging:
		void PrintComponents() const
		{
			LOG_TRACE("Components attached to: {}", m_name);
			for (const auto& pair : m_components)
				LOG_TRACE(pair.second->ToString());
		}

	private: // Methods:
		void AddChildToList(GameObject* pChild)
		{
			if (pChild == nullptr)
				return;
			if (std::find(m_children.begin(), m_children.end(), pChild) == m_children.end())
				m_children.push_back(pChild);
		}
		void RemoveChildFromList(GameObject* pChild)
		{
			if (pChild == nullptr)
				return;
			m_children.erase(std::remove(m_children.begin(), m_children.end(), pChild), m_children.end());
		}
		bool IsAncestorOf(GameObject* potentialDescendant) const
		{
			if (potentialDescendant == nullptr)
				return false;
			for (GameObject* descendant = potentialDescendant; descendant != nullptr; descendant = descendant->m_pParent)
			{
				if (descendant == this)
					return true;
			}
			return false;
		}
		bool IsDescendantOf(GameObject* potentialAncestor) const
		{
			if (potentialAncestor == nullptr)
				return false;
			for (GameObject* ancestor = m_pParent; ancestor != nullptr; ancestor = ancestor->m_pParent)
			{
				if (ancestor == potentialAncestor)
					return true;
			}
			return false;
		}
	};
}