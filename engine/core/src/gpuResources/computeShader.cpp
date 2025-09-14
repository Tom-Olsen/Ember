#include "computeShader.h"
#include "vulkanComputeShader.h"



namespace emberEngine
{
	// Constructor/Destructor:
	ComputeShader::ComputeShader(const std::string& name, const std::filesystem::path& computeSpv)
	{
		m_pIComputeShader = std::make_unique<vulkanRendererBackend::ComputeShader>(name, computeSpv);
	}
	ComputeShader::~ComputeShader()
	{
		m_pIComputeShader.release();
	}



	// Public methods:
	Uint3 ComputeShader::GetBlockSize() const
	{
		return m_pIComputeShader->GetBlockSize();
	}
}