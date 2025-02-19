#ifndef __INCLUDE_GUARD_computeUnit_h__
#define __INCLUDE_GUARD_computeUnit_h__
#include "component.h"
#include "mathf.h"
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
		Uint3 m_groupCount;
		uint32_t m_groupCountY;
		uint32_t m_groupCountZ;
		ComputeShader* m_pComputeShader;
		std::unique_ptr<ShaderProperties> m_pShaderProperties;

	public: // Methods:
		ComputeUnit();
		~ComputeUnit();
		
		// Setter:
		void SetComputeShader(ComputeShader* pComputeShader);
		void SetComputeCount(Uint3 groupCount);
		void SetComputeCount(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ);

		// Getters:
		ComputeShader* GetComputeShader();
		ShaderProperties* GetShaderProperties();
		Uint3 GetGroupCount();
		const VkDescriptorSet* const GetDescriptorSets(uint32_t frameIndex) const;
		const VkPipeline& GetPipeline() const;
		const VkPipelineLayout& GetPipelineLayout() const;
		
		// Overrides:
		const std::string ToString() const override;

	private: // Methods:
	};
}



#endif // __INCLUDE_GUARD_computeUnit_h__