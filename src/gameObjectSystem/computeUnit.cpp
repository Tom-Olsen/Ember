#include "computeUnit.h"
#include "computeShader.h"
#include "pipeline.h"
#include "shaderProperties.h"



namespace emberEngine
{
	// Constructor/Destructor:
	ComputeUnit::ComputeUnit()
	{
		m_pComputeShader = nullptr;
		m_pShaderProperties = nullptr;
	}
	ComputeUnit::~ComputeUnit()
	{

	}



	// Public methods:
	void ComputeUnit::SetComputeShader(ComputeShader* pComputeShader)
	{
		if (pComputeShader != nullptr && pComputeShader != m_pComputeShader)
		{
			m_pComputeShader = pComputeShader;
			m_pShaderProperties = std::make_unique<ShaderProperties>(m_pComputeShader);
		}
	}
	ComputeShader* ComputeUnit::GetComputeShader()
	{
		return m_pComputeShader;
	}
	ShaderProperties* ComputeUnit::GetShaderProperties()
	{
		return m_pShaderProperties.get();
	}
	const VkDescriptorSet* const ComputeUnit::GetDescriptorSets(uint32_t frameIndex) const
	{
		return &m_pShaderProperties->GetDescriptorSets()[frameIndex];
	}
	const VkPipeline& ComputeUnit::GetPipeline() const
	{
		return m_pComputeShader->GetPipeline()->GetVkPipeline();
	}
	const VkPipelineLayout& ComputeUnit::GetPipelineLayout() const
	{
		return m_pComputeShader->GetPipeline()->GetVkPipelineLayout();
	}



	// Overrides:
	const std::string ComputeUnit::ToString() const
	{
		return "ComputeUnit";
	}
}