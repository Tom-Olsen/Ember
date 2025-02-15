#ifndef __INCLUDE_GUARD_computeUnit_h__
#define __INCLUDE_GUARD_computeUnit_h__
#include "component.h"
#include <memory>
#include <string>
#include <vulkan/vulkan.h>



namespace emberEngine
{
	// Forward declarations:
	class ComputeShader;
	class ShaderProperties;



	class ComputeUnit : public Component
	{
	private: // Members:
		ComputeShader* m_pComputeShader;
		std::unique_ptr<ShaderProperties> m_pShaderProperties;

	public: // Methods:
		ComputeUnit();
		~ComputeUnit();
		
		// Setter:
		void SetComputeShader(ComputeShader* pComputeShader);

		// Getters:
		ComputeShader* GetComputeShader();
		ShaderProperties* GetShaderProperties();
		const VkDescriptorSet* const GetDescriptorSets(uint32_t frameIndex) const;
		const VkPipeline& GetPipeline() const;
		const VkPipelineLayout& GetPipelineLayout() const;
		
		// Overrides:
		const std::string ToString() const override;

	private: // Methods:
	};
}



#endif // __INCLUDE_GUARD_computeUnit_h__