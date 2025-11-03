#pragma once
#include <entt/entt.hpp>
#include <string>
#include <vector>



namespace emberEngine
{
	// Forward decleration:
	class Component;
	class EntityState;
	class Transform;
	class Scene;



	// Slim wrapper around entt::entity for easier access.
	class Entity
	{
		// Friends:
		friend class Component;
		friend class Scene;

	private: // Members:
		entt::entity m_entityHandle;
		std::string m_name;
		Scene* m_pScene;

	private: // Methods:
		// Constructor/Destructor: (only accessable by Scene)
		Entity();	// for invalid Entities only.

	public: // Methods:
		~Entity();
		static Entity Create(const std::string& name);	// Registers the entity in the currently active scene.
		void Destroy();									// Properly disposed entity from ECS registry.

		// Copyable:
		Entity(const Entity&) = default;
		Entity& operator=(const Entity&) = default;

		// Movable:
		Entity(Entity&& other) noexcept = default;
		Entity& operator=(Entity&& other) noexcept = default;

		// Setters:
		void SetName(const std::string& name);
		void SetIsEnabled(bool isEnabled);

		// Basic Getters:
		const std::string& GetName() const;
		bool GetIsEnabled() const;
		bool GetIsActive() const;
		bool GetIsValid() const;
		Scene* const GetScene() const;

		// Component Getters:
		Transform* GetTransform() const;
		EntityState* GetEntityState() const;

		// Templated methods:
		template<typename T, typename... Args>
		T* AddComponent(Args&&... args);
		template<typename T>
		bool HasComponent() const;
		template<typename T>
		T* GetComponent() const;
		template<typename T>
		void RemoveComponent();
		
		// Debugging:
		void PrintComponents() const;

	private: // Methods:
		Entity(entt::entity handle, const std::string& name, Scene* pScene);
		entt::entity GetEntityHandle() const;
	};
}