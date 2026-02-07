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
	template<typename vertexLayout>
	class ForwardOpaquePipeline : public Pipeline
	{
	public: // Methods:
		ForwardOpaquePipeline(const std::string& name, const std::vector<char>& vertexCode, const std::vector<char>& fragmentCode, const emberSpirvReflect::ShaderReflection& shaderReflection);
		~ForwardOpaquePipeline();

	private: // Methods:
		void CreatePipelineLayout(const emberSpirvReflect::ShaderReflection& shaderReflection);
		void CreatePipeline(const VkShaderModule& vertexShaderModule, const VkShaderModule& fragmentShaderModule, const emberSpirvReflect::ShaderReflection& shaderReflection);
	};
}