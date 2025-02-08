#ifndef __INCLUDE_GUARD_application_h__
#define __INCLUDE_GUARD_application_h__
#include <memory>



namespace emberEngine
{
	// Forward declarations:
	class Scene;
	struct VulkanContext;
	class VulkanRenderer;



	class Application
	{
	private: // Members:
		std::unique_ptr<VulkanContext> m_pContext;
		std::unique_ptr<VulkanRenderer> m_pRenderer;
		Scene* m_pActiveScene;

	public: // Methods:
		Application();
		~Application();
		void Run();
		void SetScene(Scene* pScene);

	private: // Methods:
		void Start();
		void Update();
		void LateUpdate();
	};
}



#endif // __INCLUDE_GUARD_application_h__