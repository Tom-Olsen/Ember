#include "vulkanComputeShader.h"
#include "descriptorSetMacros.h"
#include "vulkanComputePipeline.h"
#include "vulkanComputePushConstant.h"
#include "vulkanContext.h"
#include "vulkanDescriptorSetBinding.h"
#include "vulkanMacros.h"
#include <utility>



namespace vulkanRendererBackend
{
	// Public methods:
	// Constructor/Destructor:
	ComputeShader::ComputeShader(const std::filesystem::path& computeSpv, const std::string& debugName)
		: Shader(debugName)
	{
		// Load compute shader:
		std::vector<char> computeCode = emberSpirvReflect::ShaderReflection::ReadShaderCode(computeSpv);
		m_shaderReflection.AddShaderStage(VK_SHADER_STAGE_COMPUTE_BIT, computeCode);

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
		NAME_VK_OBJECT(m_vkPipelineLayout, "PipelineLayout_Compute_" + debugName);

		// Get block size:
		m_blockSize = m_shaderReflection.GetComputeStageInfo()->blockSize;

		// Create pipeline:
		m_pPipeline = std::make_unique<ComputePipeline>(m_vkPipelineLayout, computeCode, debugName);

		// Create shader descriptorSetBinding:
		m_pShaderDescriptorSetBinding = std::make_unique<DescriptorSetBinding>(static_cast<Shader*>(this), SHADER_SET_INDEX, debugName);
	}
	ComputeShader::~ComputeShader()
	{
	
	}

	// Movable:
	ComputeShader::ComputeShader(ComputeShader&& other) noexcept
		: Shader(std::move(other))
		, m_blockSize(other.m_blockSize)
		, m_pShaderDescriptorSetBinding(std::move(other.m_pShaderDescriptorSetBinding))
		, m_pPipeline(std::move(other.m_pPipeline))
	{
		if (m_pShaderDescriptorSetBinding)
			m_pShaderDescriptorSetBinding->RebindShader(this);
	}
	ComputeShader& ComputeShader::operator=(ComputeShader&& other) noexcept
	{
		if (this != &other)
		{
			Shader::operator=(std::move(other));
			m_blockSize = other.m_blockSize;
			m_pShaderDescriptorSetBinding = std::move(other.m_pShaderDescriptorSetBinding);
			m_pPipeline = std::move(other.m_pPipeline);
			if (m_pShaderDescriptorSetBinding)
				m_pShaderDescriptorSetBinding->RebindShader(this);
		}
		return *this;
	}
	
	
	
	// Getters:
	Uint3 ComputeShader::GetBlockSize() const
	{
		return m_blockSize;
	}
	emberBackendInterface::IDescriptorSetBinding* ComputeShader::GetShaderDescriptorSetBinding() const
	{
		return static_cast<emberBackendInterface::IDescriptorSetBinding*>(m_pShaderDescriptorSetBinding.get());
	}
	DescriptorSetBinding* ComputeShader::GetDescriptorSetBinding() const
	{
		return m_pShaderDescriptorSetBinding.get();
	}
	const Pipeline* ComputeShader::GetPipeline() const
	{
		return m_pPipeline.get();
	}



	// Debugging:
	void ComputeShader::Print() const
	{
		PrintShaderInfo();
	}
}