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
		m_pIComputeShader = nullptr;
	}
	ComputeShader::ComputeShader(const std::string& name, const std::filesystem::path& computeSpv) : Shader()
	{
		m_pIComputeShader = std::unique_ptr<emberBackendInterface::IComputeShader>(Renderer::CreateComputeShader(name, computeSpv));
		m_pIShaderDescriptorSetBinding = m_pIComputeShader->GetShaderDescriptorSetBinding();
	}
	ComputeShader::~ComputeShader()
	{

	}



	// Movable:
	ComputeShader::ComputeShader(ComputeShader&& other) noexcept
		: Shader(std::move(other))
	{
		m_pIComputeShader = std::move(other.m_pIComputeShader);
	}
	ComputeShader& ComputeShader::operator=(ComputeShader&& other) noexcept
	{
		if (this != &other)
		{
			Shader::operator=(std::move(other));
			m_pIComputeShader = std::move(other.m_pIComputeShader);
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
		return m_pIComputeShader->GetName();
	}



	// Debugging:
	void ComputeShader::Print() const
	{
		m_pIComputeShader->Print();
	}
}