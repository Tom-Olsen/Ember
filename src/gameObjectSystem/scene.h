#pragma once
#ifndef __INCLUDE_GUARD_scene_h__
#define __INCLUDE_GUARD_scene_h__
#include <memory>
#include <vector>
#include <array>
#include <string>
#include <unordered_map>
#include "gameObject.h"
#include "macros.h"



class Scene
{
public: // Members:
	Camera* activeCamera = nullptr;
	int dLightsCount = 0;
	int pLightsCount = 0;
	int sLightsCount = 0;
	std::unordered_map<std::string, std::unique_ptr<GameObject>> gameObjects;
	std::array<DirectionalLight*, MAX_D_LIGHTS> directionalLights;
	std::array<SpotLight*, MAX_S_LIGHTS> spotLights;
	//std::array<PointLight*, MAX_P_LIGHTS> pointLights;

private: // Members:
	bool meshRenderersSortet = false;
	std::unordered_map<std::string, MeshRenderer*> meshRenderers;
	std::vector<MeshRenderer*> sortedMeshRenderers;

public: // Methods:
	Scene();
	~Scene();

	void AddGameObject(GameObject* gameObject);
	GameObject* GetGameObject(std::string name);
	void RemoveGameObject(std::string name);
	void SetActiveCamera(Camera* camera);
	std::vector<MeshRenderer*>& GetSortedMeshRenderers();

	void Load();
	void Unload();

	// Debugging:
	void PrintGameObjects() const;
	void PrintMeshRenderers() const;
	void PrintSortedMeshRenderers();
	void PrintLights() const;

private: // Methods:
	void SortMeshRenderers();
};



#endif // __INCLUDE_GUARD_scene_h__