#ifndef __INCLUDE_GUARD_scene_h__
#define __INCLUDE_GUARD_scene_h__
#include "macros.h"
#include <array>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>



namespace emberEngine
{
	// Forward declarations:
	class Camera;
	class DirectionalLight;
	class GameObject;
	class MeshRenderer;
	class PointLight;
	class SpotLight;



	/// <summary>
	/// The scene owns all gameObjects and keeps track of important components:
	/// - Active camera
	/// - Directional lights, spot light, and Point lights
	/// When Drawing anything with VulkanRenderer->Render(pScene) a scene pointer is required.
	/// </summary>
	class Scene
	{
	private: // Members:
		Camera* m_pActiveCamera;
		int m_directionalLightsCount;
		int m_spotLightsCount;
		int m_pointLightsCount;
		std::unordered_map<std::string, std::unique_ptr<GameObject>> m_gameObjects;
		std::array<DirectionalLight*, MAX_D_LIGHTS> m_directionalLights;
		std::array<SpotLight*, MAX_S_LIGHTS> m_spotLights;
		std::array<PointLight*, MAX_P_LIGHTS> m_pointLights;

	public: // Methods:
		Scene();
		~Scene();

		// Getters:
		Camera* const GetActiveCamera() const;
		int GetDirectionalLightsCount();
		int GetSpotLightsCount();
		int GetPointLightsCount();
		GameObject* const GetGameObject(std::string name) const;
		const std::unordered_map<std::string, std::unique_ptr<GameObject>>& GetGameObjects() const;
		const std::array<DirectionalLight*, MAX_D_LIGHTS>& GetDirectionalLights() const;
		const std::array<SpotLight*, MAX_S_LIGHTS>& GetSpotLights() const;
		const std::array<PointLight*, MAX_P_LIGHTS>& GetPointLights() const;

		// Setters:
		void AddGameObject(GameObject* pGameObject);
		void SetActiveCamera(Camera* camera);

		void RemoveGameObject(std::string name);

		// Load and unload the scene:
		void Load();
		void Unload();

		// Debugging:
		void PrintGameObjects() const;
		void PrintLights() const;
	};
}



#endif // __INCLUDE_GUARD_scene_h__