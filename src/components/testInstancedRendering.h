#ifndef __INCLUDE_GUARD_testInstancedRendering_h__
#define __INCLUDE_GUARD_testInstancedRendering_h__
#include "emberEngine.h"



namespace emberEngine
{
	// Forward declarations:
	class MeshRenderer;



	class TestInstancedRendering : public Component
	{
	private: // Members:
		VulkanContext* m_pVulkanContext;
		ComputeShader* m_pStartCS;
		ComputeShader* m_pUpdateCS;
		std::unique_ptr<StorageBuffer> m_pStorageBuffer;
		std::unique_ptr<ShaderProperties> m_pStartProperties;
		std::unique_ptr<ShaderProperties> m_pUpdateProperties;

	public: // Methods:
		TestInstancedRendering(VulkanContext* pVulkanContext, uint32_t instanceCount);
		~TestInstancedRendering();

		StorageBuffer* GetInstanceBuffer() const;

		// Overrides:
		void Start() override;
		void Update() override;
		const std::string ToString() const override;
	};
}



#endif // __INCLUDE_GUARD_testInstancedRendering_h__