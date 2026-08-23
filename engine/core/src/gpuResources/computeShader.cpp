#include "computeShader.h"
#include "compute.h"
#include "iComputeShader.h"
#include "renderer.h"
#include <utility>



namespace emberCore
{
	// Private methods:
	emberBackendInterface::IComputeShader* ComputeShader::GetInterfaceHandle()
	{
		return m_pIComputeShader.get();
	}



	// Public methods:
	// Constructor/Destructor:
	ComputeShader::ComputeShader()
        : Shader()
	{
		m_name = "";
		m_pIComputeShader = nullptr;
	}
	ComputeShader::ComputeShader(const std::filesystem::path& computeSpv, const std::string& name)
		: Shader()
		, m_name(name)
	{
		m_pIComputeShader = std::unique_ptr<emberBackendInterface::IComputeShader>(Renderer::CreateComputeShader(computeSpv, name));
		m_pIShaderDescriptorSetBinding = m_pIComputeShader->GetShaderDescriptorSetBinding();
	}
	ComputeShader::~ComputeShader()
	{
		Renderer::DestroyComputeShader(m_pIComputeShader.release());
	}



	// Movable:
	ComputeShader::ComputeShader(ComputeShader&& other) noexcept
		: Shader(std::move(other))
		, m_name(std::move(other.m_name))
	{
		m_pIComputeShader = std::move(other.m_pIComputeShader);
	}
	ComputeShader& ComputeShader::operator=(ComputeShader&& other) noexcept
	{
	    if (this != &other)
	    {
	        emberBackendInterface::IComputeShader* pOldComputeShader = m_pIComputeShader.release();

	        Shader::operator=(std::move(other));
	        m_name = std::move(other.m_name);
	        m_pIComputeShader = std::move(other.m_pIComputeShader);

	        Renderer::DestroyComputeShader(pOldComputeShader);
	    }
	    return *this;
	}



	// Public methods:
	Uint3 ComputeShader::GetBlockSize() const
	{
		return m_pIComputeShader->GetBlockSize();
	}
	const std::string& ComputeShader::GetName() const
	{
		return m_name;
	}



	// Debugging:
	void ComputeShader::Print() const
	{
		m_pIComputeShader->Print();
	}
}