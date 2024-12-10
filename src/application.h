#ifndef __INCLUDE_GUARD_application_h__
#define __INCLUDE_GUARD_application_h__
#include "vulkanContext.h"
#include "vulkanRenderer.h"
#include "materialProperties.h"
#include "component.h"
#include "eventSystem.h"
#include "renderPassManager.h"
#include "materialManager.h"
#include "textureManager.h"
#include "textureCube.h"
#include "samplerManager.h"
#include "meshManager.h"
#include "graphics.h"
#include "timer.h"
#include "gameObject.h"
#include "scene.h"



/// <summary>
/// Main application class.
/// </summary>
class Application
{
public: // Members:
	std::unique_ptr<VulkanContext> context;
	std::unique_ptr<VulkanRenderer> renderer;

private: // Members:
	Scene* activeScene;

public: // Methods:
	Application();
	~Application();
	void Run();
	void SetScene(Scene* scene);

private: // Methods:
	void Update();
};



#endif // __INCLUDE_GUARD_application_h__