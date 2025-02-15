#include "computeShader.h"
#include "computePipeline.h"
#include "logger.h"
#include "spirvReflect.h"
#include "vulkanContext.h"



namespace emberEngine
{
	// Constructor/Destructor:
	ComputeShader::ComputeShader(VulkanContext* pContext, const std::string& name, const std::filesystem::path& computeSpv)
	{
		m_name = name;
		m_pContext = pContext;
		m_pDescriptorBoundResources = std::make_unique<DescriptorBoundResources>();

		// Load compute shader:
		std::vector<char> computeCode = ReadShaderCode(computeSpv);
		SpirvReflect computeShaderReflect(computeCode);
		computeShaderReflect.AddDescriptorBoundResources(m_pDescriptorBoundResources.get());

		m_pPipeline = std::make_unique<ComputePipeline>(m_pContext, computeCode, m_pDescriptorBoundResources->descriptorSetLayoutBindings);
	}
	ComputeShader::~ComputeShader()
	{

	}
}