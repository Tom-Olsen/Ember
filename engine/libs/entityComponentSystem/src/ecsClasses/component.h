#pragma once
#include <entt/entt.hpp>
#include <string>



namespace emberEngine
{
	// Forward decleration:
	class Entity;
	class Transform;
	class Scene;



	class Component
	{
		// Friends:
		friend class Entity;

	private: // Members:
		std::string m_name;	// automatically set in CoreEntity::AddComponent => friend CoreEntity.
		bool m_isEnabled;
		entt::entity m_entityHandle;	// can't store Entity reference as those are transient objects.
		Scene* m_pScene;				// needed to create Entity from entt::entity handle.

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
		Entity const GetEntity() const;
		Transform* GetTransform() const;

		// Templated methods:
		template<typename T>
		T* GetComponent() const;

		// ECS methods:
		virtual void Start() {};
		virtual void Update() {};
		virtual void LateUpdate() {};
		virtual void FixedUpdate() {};
		virtual void OnDestroy() {};
	};
}