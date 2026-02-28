#include "vulkanComputeShader.h"
#include "descriptorSetMacros.h"
#include "vulkanComputePipeline.h"
#include "vulkanDescriptorSetBinding.h"



namespace vulkanRendererBackend
{
	// Public methods:
	// Constructor/Destructor:
	ComputeShader::ComputeShader(const std::string& name, const std::filesystem::path& computeSpv)
	{
		m_name = name;
		m_pDescriptorBoundResources = std::make_unique<DescriptorBoundResources>();
	
		// Load compute shader:
		std::vector<char> computeCode = emberSpirvReflect::ShaderReflection::ReadShaderCode(computeSpv);
		m_shaderReflection.AddShaderStage(VK_SHADER_STAGE_VERTEX_BIT, vertexCode);

		// Prepare pipeline data:
		CreateDescriptorSetLayout();

		// Get block size:
		m_blockSize = m_shaderReflection.GetComputeShaderReflection()->GetComputeInfo()->blockSize;

		// Create pipeline:
		m_pPipelines.emplace_back(std::make_unique<ComputePipeline>(m_name, computeCode, m_vkDescriptorSetLayouts));

		// Create shader descriptorSetBinding:
		m_pShaderDescriptorSetBinding = std::make_unique<DescriptorSetBinding>((Shader*)this, SHADER_SET_INDEX);
	}
	ComputeShader::~ComputeShader()
	{
	
	}



	// Movable:
	ComputeShader::ComputeShader(ComputeShader&& other) noexcept : Shader(std::move(other))
	{
		m_blockSize = other.m_blockSize;
		m_pipelineLayout = other.m_pipelineLayout;
		other.m_pipelineLayout = VK_NULL_HANDLE;
	}
	ComputeShader& ComputeShader::operator=(ComputeShader&& other) noexcept
	{
		if (this != &other)
		{
			Shader::operator=(std::move(other));
			m_blockSize = other.m_blockSize;
			m_pipelineLayout = other.m_pipelineLayout;
			other.m_pipelineLayout = VK_NULL_HANDLE;
		}
		return *this;
	}
	
	
	
	// Getters:
	Uint3 ComputeShader::GetBlockSize() const
	{
		return m_blockSize;
	}
	const std::string& ComputeShader::GetName() const
	{
		return m_name;
	}
	const Pipeline* const ComputeShader::GetPipeline() const
	{
		return m_pPipelines[0];	// compute shaders only ever contain 1 pipeline.
	}



	// Debugging:
	void ComputeShader::Print() const
	{
		PrintShaderInfo();
	}
}