#include "computeShader.h"
#include "computeShaderManager.h"
#include "iComputeShader.h"
#include <stdexcept>



namespace emberCore
{
	// Public methods:
	// Constructor/Destructor:
	ComputeShader::ComputeShader()
		: Shader()
		, m_computeShaderId(emberCommon::invalidComputeShaderId)
	{

	}
	ComputeShader::~ComputeShader()
	{

	}



	// Getters:
	Uint3 ComputeShader::GetBlockSize() const
	{
		emberBackendInterface::IComputeShader* pIComputeShader = TryGetInterfaceHandle();
		if (pIComputeShader == nullptr)
			throw std::runtime_error("ComputeShader::GetBlockSize() failed. ComputeShader is invalid or expired.");
		return pIComputeShader->GetBlockSize();
	}
	const std::string& ComputeShader::GetName() const
	{
		const std::string* pName = ComputeShaderManager::TryGetComputeShaderName(m_computeShaderId);
		if (pName == nullptr)
			throw std::runtime_error("ComputeShader::GetName() failed. ComputeShader is invalid or expired.");
		return *pName;
	}
	bool ComputeShader::IsValid() const
	{
		return TryGetInterfaceHandle() != nullptr;
	}



	// Debugging:
	void ComputeShader::Print() const
	{
		emberBackendInterface::IComputeShader* pIComputeShader = TryGetInterfaceHandle();
		if (pIComputeShader == nullptr)
			throw std::runtime_error("ComputeShader::Print() failed. ComputeShader is invalid or expired.");
		pIComputeShader->Print();
	}



	// Private methods:
	ComputeShader::ComputeShader(emberCommon::ComputeShaderId computeShaderId)
		: Shader()
		, m_computeShaderId(computeShaderId)
	{

	}
	emberBackendInterface::IComputeShader* ComputeShader::TryGetInterfaceHandle() const
	{
		return ComputeShaderManager::TryGetComputeShaderInterface(m_computeShaderId);
	}
	emberBackendInterface::IDescriptorSetBinding* ComputeShader::TryGetShaderDescriptorSetBinding() const
	{
		if (!ComputeShaderManager::IsComputeShaderMutable(m_computeShaderId))
			return nullptr;
		if (emberBackendInterface::IComputeShader* pIComputeShader = TryGetInterfaceHandle())
			return pIComputeShader->GetShaderDescriptorSetBinding();
		return nullptr;
	}
}