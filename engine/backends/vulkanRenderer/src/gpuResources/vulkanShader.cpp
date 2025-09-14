#include "vulkanShader.h"
#include "logger.h"
#include "spirvReflect.h"
#include "vulkanPipeline.h"
#include <fstream>



namespace vulkanRendererBackend
{
	// Virtual Destructor:
	Shader::~Shader()
	{

	}

	// Getters:
	const std::string& Shader::GetName() const
	{
		return m_name;
	}
	const Pipeline* const Shader::GetPipeline() const
	{
		return m_pPipeline.get();
	}
	const DescriptorBoundResources* const Shader::GetDescriptorBoundResources() const
	{
		return m_pDescriptorBoundResources.get();
	}

	// Debugging:
	void Shader::PrintShaderInfo() const
	{
		LOG_TRACE(m_pDescriptorBoundResources->ToString());
	}

	// Protected methods:
	std::vector<char> Shader::ReadShaderCode(const std::filesystem::path& spvFile)
	{
		// Open shader file:
		std::ifstream file(spvFile, std::ios::binary);
		if (!file.is_open())
		{
			LOG_CRITICAL("Error opening shader file: {}", spvFile.string());
			throw std::runtime_error("Failed to open shader file: " + spvFile.string());
		}

		// Get file size:
		file.seekg(0, std::ios::end);
		size_t fileSize = static_cast<size_t>(file.tellg());
		file.seekg(0, std::ios::beg);

		// Copy code:
		std::vector<char> code(fileSize);
		file.read(code.data(), fileSize);
		file.close();

		return code;
	}
}