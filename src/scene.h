#pragma once
#ifndef __INCLUDE_GUARD_scene_h__
#define __INCLUDE_GUARD_scene_h__
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include "gameObject.h"



class Scene
{
public: // Members:
	std::unordered_map<std::string, MeshRenderer*> meshRenderers;

private: // Members:
	std::unordered_map<std::string, std::unique_ptr<GameObject>> gameObjects;

public: // Methods:
	Scene();
	~Scene();

	void AddGameObject(GameObject* gameObject);
	GameObject* GetGameObject(std::string name);
	void RemoveGameObject(std::string name);

	void Load();
	void Unload();

	void PrintAllGameObjects() const;
	void PrintAllMeshRenderers() const;
};



#endif // __INCLUDE_GUARD_scene_h__