#pragma once
#include "entityId.h"
#include <entt/entity/fwd.hpp>
#include <stdint.h>
#include <memory>
#include <string>



namespace emberEcs
{
	// Forward declarations:
	class Transform;
	class World;



	class Entity
	{
		// Friends:
		friend class World;
		friend struct std::default_delete<Entity>;	// gives default deleter access to private destructor.

	private: // Members:
		EntityId m_entityId;
		uint64_t m_runtimeId;
		entt::entity m_registryEntry;
		std::string m_name;
		bool m_isEnabled;

	public: // Methods:
		// Non-copyable:
		Entity(const Entity&) = delete;
		Entity& operator=(const Entity&) = delete;

		// None-movable:
		Entity(Entity&& other) noexcept = delete;
		Entity& operator=(Entity&& other) noexcept = delete;

		// Setters:
		void SetName(const std::string& name);
		void SetIsEnabled(bool isEnabled);

		// Getters:
		EntityId GetId() const;
		const std::string& GetName() const;
		bool GetIsEnabled() const;
		bool GetIsActive() const;
		Transform* GetTransform() const;

		// Entity management:
		void Destroy();

		// Components:
		template<typename T, typename... Args>
		T* AddComponent(Args&&... args);
		template<typename T>
		bool HasComponent() const;
		template<typename T>
		T* GetComponent() const;
		template<typename T>
		void RemoveComponent();

	private: // Methods:
		// Constructor/Destructor:
		Entity(EntityId entityId, uint64_t runtimeId, entt::entity registryEntry, const std::string& name);
		~Entity();
	};
}