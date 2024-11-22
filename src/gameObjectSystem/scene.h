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
	std::array<DirectionalLight*, MAX_D_LIGHTS> directionalLights;
	std::array<SpotLight*, MAX_S_LIGHTS> spotLights;
	//std::array<DirectionalLight*, MAX_P_LIGHTS> pointLights;
	std::unordered_map<std::string, MeshRenderer*> meshRenderers;
	std::unordered_map<std::string, std::unique_ptr<GameObject>> gameObjects;

public: // Methods:
	Scene();
	~Scene();

	void AddGameObject(GameObject* gameObject);
	GameObject* GetGameObject(std::string name);
	void RemoveGameObject(std::string name);
	void SetActiveCamera(Camera* camera);

	void Load();
	void Unload();

	void PrintAllGameObjects() const;
	void PrintAllMeshRenderers() const;
};



#endif // __INCLUDE_GUARD_scene_h__