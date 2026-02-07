#include "vulkanComputeShader.h"
#include "vulkanComputePipeline.h"



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

		// Create descriptor sets:
		m_shaderReflection.CreateDescriptorSets();

		// Get block size:
		m_blockSize = m_shaderReflection.GetComputeShaderReflection()->GetComputeInfo()->blockSize;

		// Create pipeline:
		m_pPipelines.emplace_back(std::make_unique<ComputePipeline>(m_name, computeCode, m_shaderReflection));
	}
	ComputeShader::~ComputeShader()
	{
	
	}



	// Movable:
	ComputeShader::ComputeShader(ComputeShader&& other) noexcept = default;
	ComputeShader& ComputeShader::operator=(ComputeShader&& other) noexcept = default;
	
	
	
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