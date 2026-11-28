#pragma once
#include "entityHandle.h"
#include <string>



namespace emberEcs
{
	// Forward declarations:
	class Entity;
	class Transform;



	class Component
	{
		// Friends:
		friend class Entity;

	private: // Members:
		std::string m_name;		// automatically set in Entity::AddComponent(...)
		bool m_isEnabled;
		EntityHandle m_entity;

	public: // Methods:
		// Constructor/Destructor:
		Component();
		virtual ~Component();

		// Non-copyable:
		Component(const Component&) = delete;
		Component& operator=(const Component&) = delete;

		// Movable:
		Component(Component&& other) noexcept = default;
		Component& operator=(Component&& other) noexcept = default;

		// Setters:
		void SetIsEnabled(bool isEnabled);
		
		// Getters:
		const std::string& GetName() const;
		bool GetIsEnabled() const;
		bool GetIsActive() const;
		EntityHandle GetEntity() const;
		Transform* GetTransform() const;

		// Templated methods:
		template<typename T>
		T* GetComponent() const;

		// ECS methods:
		virtual void Start() {};
		virtual void EarlyUpdate() {};
		virtual void Update() {};
		virtual void LateUpdate() {};
		virtual void FixedUpdate() {};
		virtual void OnDestroy() {};
	};
}