#include "vulkanShaderHandle.h"
#include "logger.h"
#include "vulkanGpuResourceRegistry.h"
#include "vulkanShader.h"
#include <functional>
#include <stdexcept>



namespace vulkanRendererBackend
{
	// Public methods:
	// Constructor/Destructor:
	ShaderHandle::ShaderHandle()
	{

	}
	ShaderHandle::ShaderHandle(const Shader& shader)
		: m_resourceHandle(shader.m_registrationHandle)
	{
		if (!m_resourceHandle.IsValid())
			throw std::runtime_error("ShaderHandle::ShaderHandle(...) failed. Shader has been moved from.");
	}
	ShaderHandle::~ShaderHandle()
	{

	}



	// Getters:
	Shader* ShaderHandle::Get() const
	{
		Shader* pShader = Shader::s_resourceRegistry.TryGet(m_resourceHandle);
		if (pShader == nullptr)
			throw std::runtime_error("ShaderHandle::Get() failed. Shader is invalid or expired.");
		return pShader;
	}
	Shader* ShaderHandle::TryGet() const
	{
		Shader* pShader = Shader::s_resourceRegistry.TryGet(m_resourceHandle);
		if (pShader == nullptr)
			LOG_WARN("ShaderHandle::TryGet() failed. Shader is invalid or expired.");
		return pShader;
	}
	bool ShaderHandle::IsValid() const
	{
		return Shader::s_resourceRegistry.TryGet(m_resourceHandle) != nullptr;
	}
	bool ShaderHandle::IsEmpty() const
	{
		return !m_resourceHandle.IsValid();
	}



	// Comparison operators:
	bool ShaderHandle::operator==(const ShaderHandle& other) const
	{
		return m_resourceHandle == other.m_resourceHandle;
	}
	bool ShaderHandle::operator!=(const ShaderHandle& other) const
	{
		return !(*this == other);
	}



	// Hashing:
	size_t ShaderHandle::Hasher::operator()(const ShaderHandle& shaderHandle) const
	{
		size_t indexHash = std::hash<uint32_t>{}(shaderHandle.m_resourceHandle.index);
		size_t generationHash = std::hash<uint64_t>{}(shaderHandle.m_resourceHandle.generation);
		return indexHash ^ (generationHash << 1);
	}
}