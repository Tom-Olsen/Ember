#include "vulkanComputeShader.h"
#include "spirvReflect.h"
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
		std::vector<char> computeCode = ReadShaderCode(computeSpv);
		SpirvReflect computeShaderReflect(computeCode);
		m_blockSize = computeShaderReflect.GetBlockSize();
		computeShaderReflect.AddDescriptorBoundResources(m_pDescriptorBoundResources.get());
	
		m_pPipeline = std::make_unique<ComputePipeline>(computeCode, m_pDescriptorBoundResources->descriptorSetLayoutBindings);
	}
	ComputeShader::~ComputeShader()
	{
	
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
}