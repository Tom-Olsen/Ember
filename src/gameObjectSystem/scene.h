#ifndef __INCLUDE_GUARD_scene_h__
#define __INCLUDE_GUARD_scene_h__
#include <array>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>



namespace emberEngine
{
	// Forward declarations:
	class Camera;
	class GameObject;



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
		std::unordered_map<std::string, std::unique_ptr<GameObject>> m_gameObjects;

	public: // Methods:
		Scene();
		~Scene();

		// Getters:
		Camera* const GetActiveCamera() const;
		GameObject* const GetGameObject(std::string name) const;
		const std::unordered_map<std::string, std::unique_ptr<GameObject>>& GetGameObjects() const;

		// Setters:
		void AddGameObject(GameObject* pGameObject);
		void SetActiveCamera(Camera* camera);

		void RemoveGameObject(std::string name);

		// Load and unload the scene:
		void Load();
		void Unload();

		// Debugging:
		void PrintGameObjects() const;
	};
}



#endif // __INCLUDE_GUARD_scene_h__