#ifndef __INCLUDE_GUARD_application_h__
#define __INCLUDE_GUARD_application_h__
#include "vulkanContext.h"
#include "vulkanRenderer.h"
#include "renderPassManager.h"
#include "materialManager.h"
#include "textureManager.h"
#include "samplerManager.h"
#include "meshManager.h"



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
	void Update(Scene* scene);
};



#endif // __INCLUDE_GUARD_application_h__