#include "vulkanComputeShader.h"
#include "descriptorSetMacros.h"
#include "vulkanComputePipeline.h"
#include "vulkanComputePushConstant.h"
#include "vulkanContext.h"
#include "vulkanDescriptorSetBinding.h"
#include "vulkanMacros.h"



namespace vulkanRendererBackend
{
	// Public methods:
	// Constructor/Destructor:
	ComputeShader::ComputeShader(const std::string& name, const std::filesystem::path& computeSpv)
		: Shader(name)
	{
		// Load compute shader:
		std::vector<char> computeCode = emberSpirvReflect::ShaderReflection::ReadShaderCode(computeSpv);
		m_shaderReflection.AddShaderStage(VK_SHADER_STAGE_VERTEX_BIT, computeCode);

		// Prepare pipeline data:
		CreateDescriptorSetLayout();

		// Push constants layout:
		VkPushConstantRange pushConstantRange = {};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(ComputePushConstant);

		// Pipeline layout:
		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
		pipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32_t>(m_vkDescriptorSetLayouts.size());
		pipelineLayoutCreateInfo.pSetLayouts = m_vkDescriptorSetLayouts.data();
		pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
		pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;
		VKA(vkCreatePipelineLayout(Context::GetVkDevice(), &pipelineLayoutCreateInfo,nullptr, &m_vkPipelineLayout));
		NAME_VK_OBJECT(m_vkPipelineLayout, m_name + "ComputePipelineLayout");

		// Get block size:
		m_blockSize = m_shaderReflection.GetComputeStageInfo()->blockSize;

		// Create pipeline:
		m_pPipelines.emplace_back(std::make_unique<ComputePipeline>(m_name, m_vkPipelineLayout, computeCode));

		// Create shader descriptorSetBinding:
		m_pShaderDescriptorSetBinding = std::make_unique<DescriptorSetBinding>((Shader*)this, SHADER_SET_INDEX);
	}
	ComputeShader::~ComputeShader()
	{
	
	}
	
	
	
	// Getters:
	const std::string& ComputeShader::GetName() const
	{
		return m_name;
	}
	Uint3 ComputeShader::GetBlockSize() const
	{
		return m_blockSize;
	}
	const Pipeline* ComputeShader::GetPipeline() const
	{
		return m_pPipelines[0].get();	// compute shaders only ever contain 1 pipeline.
	}



	// Debugging:
	void ComputeShader::Print() const
	{
		PrintShaderInfo();
	}
}