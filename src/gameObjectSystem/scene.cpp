#include "scene.h"
#include "camera.h"
#include "directionalLight.h"
#include "gameObject.h"
#include "logger.h"
#include "material.h"
#include "pointLight.h"
#include "spotLight.h"



namespace emberEngine
{
	// Constructor/Destructor:
	Scene::Scene()
	{
		m_pActiveCamera = nullptr;
		m_directionalLightsCount = 0;
		m_spotLightsCount = 0;
		m_pointLightsCount = 0;
		for (uint32_t i = 0; i < MAX_D_LIGHTS; i++)
			m_directionalLights[i] = nullptr;
		for (uint32_t i = 0; i < MAX_S_LIGHTS; i++)
			m_spotLights[i] = nullptr;
		for (uint32_t i = 0; i < MAX_P_LIGHTS; i++)
			m_pointLights[i] = nullptr;
	}
	Scene::~Scene()
	{

	}



	// Public methods:
	// Getters:
	Camera* const Scene::GetActiveCamera() const
	{
		if (m_pActiveCamera == nullptr)
			LOG_WARN("No active camera in scene!");
		return m_pActiveCamera;
	}
	int Scene::GetDirectionalLightsCount()
	{
		return m_directionalLightsCount;
	}
	int Scene::GetSpotLightsCount()
	{
		return m_spotLightsCount;
	}
	int Scene::GetPointLightsCount()
	{
		return m_pointLightsCount;
	}
	GameObject* const Scene::GetGameObject(std::string name) const
	{
		auto it = m_gameObjects.find(name);
		if (it != m_gameObjects.end())
			return it->second.get();
		else
		{
			LOG_WARN("GameObject '{}' not found in scene!", name);
			return nullptr;
		}
	}
	const std::unordered_map<std::string, std::unique_ptr<GameObject>>& Scene::GetGameObjects() const
	{
		return m_gameObjects;
	}
	const std::array<DirectionalLight*, MAX_D_LIGHTS>& Scene::GetDirectionalLights() const
	{
		return m_directionalLights;
	}
	const std::array<SpotLight*, MAX_S_LIGHTS>& Scene::GetSpotLights() const
	{
		return m_spotLights;
	}
	const std::array<PointLight*, MAX_P_LIGHTS>& Scene::GetPointLights() const
	{
		return m_pointLights;
	}

	// Setters:
	void Scene::AddGameObject(GameObject* pGameObject)
	{
		// If scene aleady contains gameObject, do nothing.
		if (m_gameObjects.find(pGameObject->GetName()) != m_gameObjects.end())
		{
			LOG_WARN("GameObject '{}' is already in scene!", pGameObject->GetName());
			return;
		}
		// Add gameObject and associated pointers to scene.
		else
		{
			pGameObject->SetScene(this);
			m_gameObjects[pGameObject->GetName()] = std::unique_ptr<GameObject>(pGameObject);

			DirectionalLight* pDirectionalLight = pGameObject->GetComponent<DirectionalLight>();
			if (pDirectionalLight != nullptr)
			{
				for (uint32_t i = 0; i < MAX_D_LIGHTS; i++)
					if (m_directionalLights[i] == nullptr)
					{
						m_directionalLights[i] = pDirectionalLight;
						m_directionalLightsCount++;
						break;
					}
			}
			SpotLight* pSpotLight = pGameObject->GetComponent<SpotLight>();
			if (pSpotLight != nullptr)
			{
				for (uint32_t i = 0; i < MAX_S_LIGHTS; i++)
					if (m_spotLights[i] == nullptr)
					{
						m_spotLights[i] = pSpotLight;
						m_spotLightsCount++;
						break;
					}
			}
			PointLight* pPointLight = pGameObject->GetComponent<PointLight>();
			if (pPointLight != nullptr)
			{
				for (uint32_t i = 0; i < MAX_P_LIGHTS; i++)
					if (m_pointLights[i] == nullptr)
					{
						m_pointLights[i] = pPointLight;
						m_pointLightsCount++;
						break;
					}
			}
		}
	}
	void Scene::SetActiveCamera(Camera* camera)
	{
		if (m_gameObjects.find(camera->GetGameObject()->GetName()) == m_gameObjects.end())
		{
			LOG_WARN("Camera '{}' not found in scene!", camera->GetGameObject()->GetName());
			m_pActiveCamera = nullptr;
		}
		else
		{
			m_pActiveCamera = camera;
			for (DirectionalLight* pDirectionalLight : m_directionalLights)
				if (pDirectionalLight != nullptr)
					pDirectionalLight->SetActiveCamera(camera);
		}
	}

	void Scene::RemoveGameObject(std::string name)
	{
		auto it = m_gameObjects.find(name);
		if (it != m_gameObjects.end())
		{
			GameObject* pGameObject = it->second.get();
			DirectionalLight* pDirectionalLight = pGameObject->GetComponent<DirectionalLight>();
			if (pDirectionalLight != nullptr)
			{
				for (uint32_t i = 0; i < MAX_D_LIGHTS; i++)
					if (m_directionalLights[i] == pDirectionalLight)
					{
						m_directionalLights[i] = nullptr;
						m_directionalLightsCount--;
						break;
					}
			}
			SpotLight* pSpotLight = pGameObject->GetComponent<SpotLight>();
			if (pSpotLight != nullptr)
			{
				for (uint32_t i = 0; i < MAX_S_LIGHTS; i++)
					if (m_spotLights[i] == pSpotLight)
					{
						m_spotLights[i] = nullptr;
						m_spotLightsCount--;
						break;
					}
			}
			PointLight* pPointLight = pGameObject->GetComponent<PointLight>();
			if (pPointLight != nullptr)
			{
				for (uint32_t i = 0; i < MAX_P_LIGHTS; i++)
					if (m_pointLights[i] == pPointLight)
					{
						m_pointLights[i] = nullptr;
						m_pointLightsCount--;
						break;
					}
			}
			pGameObject->SetScene(nullptr);
			m_gameObjects.erase(it);
		}
		else
			LOG_WARN("GameObject '{}' not found in scene!", name);
	}



	// Load and unload the scene:
	void Scene::Load()
	{

	}
	void Scene::Unload()
	{

	}



	// Debugging:
	void Scene::PrintGameObjects() const
	{
		LOG_TRACE("GameObjects in scene:");
		for (const auto& pair : m_gameObjects)
			LOG_TRACE(pair.first);
	}
	void Scene::PrintLights() const
	{
		LOG_TRACE("Directional lights in scene: {}", m_directionalLightsCount);
		for (uint32_t i = 0; i < MAX_D_LIGHTS; i++)
			if (m_directionalLights[i] != nullptr)
				LOG_TRACE("{}", m_directionalLights[i]->GetGameObject()->GetName());

		LOG_TRACE("Spot lights in scene: {}", m_spotLightsCount);
		for (uint32_t i = 0; i < MAX_S_LIGHTS; i++)
			if (m_spotLights[i] != nullptr)
				LOG_TRACE("{}", m_spotLights[i]->GetGameObject()->GetName());

		LOG_TRACE("Point lights in scene: {}", m_pointLightsCount);
		for (uint32_t i = 0; i < MAX_P_LIGHTS; i++)
			if (m_pointLights[i] != nullptr)
				LOG_TRACE("{}", m_pointLights[i]->GetGameObject()->GetName());
	}
}