#include "computeShader.h"
#include "compute.h"
#include "iComputeShader.h"
#include "renderer.h"



namespace emberEngine
{
	// Private methods:
	emberBackendInterface::IComputeShader* ComputeShader::GetInterfaceHandle()
	{
		return m_pIComputeShader.get();
	}



	// Public methods:
	// Constructor/Destructor:
	ComputeShader::ComputeShader()
	{
		m_pIComputeShader = nullptr;
	}
	ComputeShader::ComputeShader(const std::string& name, const std::filesystem::path& computeSpv)
	{
		m_pIComputeShader = std::unique_ptr<emberBackendInterface::IComputeShader>(Renderer::CreateComputeShader(name, computeSpv));
	}
	ComputeShader::~ComputeShader()
	{

	}



	// Movable:
	ComputeShader::ComputeShader(ComputeShader&& other) noexcept = default;
	ComputeShader& ComputeShader::operator=(ComputeShader&& other) noexcept = default;



	// Public methods:
	Uint3 ComputeShader::GetBlockSize() const
	{
		return m_pIComputeShader->GetBlockSize();
	}
	const std::string& ComputeShader::GetName() const
	{
		return m_pIComputeShader->GetName();
	}
}