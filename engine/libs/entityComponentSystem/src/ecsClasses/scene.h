#pragma once
#include "entity.h"
#include <entt/entt.hpp>
#include <string>
#include <unordered_map>
#include <vector>



namespace emberEngine
{
	// Forward declerations:
	class Camera;



	class Scene
	{
		// Friends:
		friend class Entity;
		friend class Component;

	private: // Members:
		bool m_isEnabled;
		entt::registry m_registry;
		std::unordered_map<std::string, entt::entity> m_entityHandlesMap;
		std::unordered_map<entt::entity, std::string> m_entityNamesMap;
		Camera* m_pActiveCamera;
		static Scene* s_pActiveScene;

	public: // Methods:
        // Constructor/Destructor:
		Scene();
		~Scene();

		// Non-copyable:
		Scene(const Scene&) = delete;
		Scene& operator=(const Scene&) = delete;

		// Non-Movable:
		Scene(Scene&& other) noexcept = delete;
		Scene& operator=(Scene&& other) noexcept = delete;

		// Setters:
		void SetIsEnabled(bool isEnabled);
		void SetActiveCamera(Camera* pActiveCamera);

		// Getters:
		bool GetIsEnabled();
		Camera* GetActiveCamera();
		Entity GetEntity(const std::string& name); // no const as returned Entity holds scene* anc can modify scene (e.g. register new components).
		bool HasEntity(const std::string& name) const;
		std::size_t GetEntityCount() const;
		std::vector<std::string> GetEntityNames() const;

		// Entity management:
		Entity CreateEntity(const std::string& name);
		void DestroyEntity(const std::string& name);

		// ECS callbacks:
		void Start();
		void FixedUpdate();
		void Update();
		void LateUpdate();

		// Static Methods:
		static Scene* const GetActiveScene();

		// Debugging
		void PrintEntities() const;

	private: // Methods:
		// EnTT Getters:
		entt::registry& GetRegistry();
		const entt::registry& GetRegistry() const;
		std::unordered_map<std::string, entt::entity>& GetEntityHandlesMap();
		std::unordered_map<entt::entity, std::string>& GetEntityNamesMap();
	};
}