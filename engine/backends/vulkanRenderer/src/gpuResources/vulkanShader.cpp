#include "vulkanShader.h"
#include "logger.h"
#include "spirvReflect.h"
#include "vulkanPipeline.h"
#include <fstream>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Protected methods:
	// Constructor:
	Shader::Shader()
	{

	}



	// Public methods:
	// Destructor:
	Shader::~Shader()
	{

	}



	// Movable:
	Shader::Shader(Shader&& other) noexcept = default;
	Shader& Shader::operator=(Shader&& other) noexcept = default;



	// Getters:
	const std::string& Shader::GetName() const
	{
		return m_name;
	}
	const Pipeline* const Shader::GetPipeline() const
	{
		return m_pPipeline.get();
	}
	const emberSpirvReflect::ShaderReflection& const Shader::GetShaderReflection() const
	{
		return m_shaderReflection;
	}



	// Debugging:
	void Shader::PrintShaderInfo() const
	{
		LOG_TRACE("ShaderInfo: {}\n{}", m_name, m_shaderReflection.ToString());
	}
}