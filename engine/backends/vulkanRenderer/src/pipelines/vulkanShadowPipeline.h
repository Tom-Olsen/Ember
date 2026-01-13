#pragma once
#include "vulkanPipeline.h"
#include <vector>



// Forward declarations:
typedef struct VkShaderModule_T* VkShaderModule;
namespace emberSpirvReflect
{
	class ShaderReflection;
}



namespace vulkanRendererBackend
{
	// Forward declarations:
	struct VertexInputDescriptions;



	template<typename vertexLayout>
	class ShadowPipeline : public Pipeline
	{
	public: // Methods:
		ShadowPipeline(const std::string& name, uint32_t shadowMapResolution, const std::vector<char>& vertexCode, const emberSpirvReflect::ShaderReflection& shaderReflection);
		~ShadowPipeline();

	private: // Methods:
		void CreatePipelineLayout(const emberSpirvReflect::ShaderReflection& shaderReflection);
		void CreatePipeline(uint32_t shadowMapResolution, const VkShaderModule& vertexShaderModule, const emberSpirvReflect::ShaderReflection& shaderReflection);
	};
}