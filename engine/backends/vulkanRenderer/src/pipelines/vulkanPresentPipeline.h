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
	class PresentPipeline : public Pipeline
	{
	public: // Methods:
		PresentPipeline(const std::string& name, const std::vector<char>& vertexCode, const std::vector<char>& fragmentCode, const emberSpirvReflect::ShaderReflection& shaderReflection);
		~PresentPipeline();

	private: // Methods:
		void CreatePipelineLayout(const emberSpirvReflect::ShaderReflection& shaderReflection);
		void CreatePipeline(const VkShaderModule& vertexShaderModule, const VkShaderModule& fragmentShaderModule, const emberSpirvReflect::ShaderReflection& shaderReflection);
	};
}