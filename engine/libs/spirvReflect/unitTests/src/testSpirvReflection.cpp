#include "logger.h"
#include "shaderReflection.h"
#include <fstream>
#include <filesystem>
#include <gtest/gtest.h>



// Helper functions:
std::vector<char> ReadShaderCode(const std::filesystem::path& spvFile)
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



TEST(ShaderReflection, VertexShaderReflection)
{
	// Load shader code:
	std::filesystem::path directoryPath = (std::filesystem::path(ENGINE_SHADERS_DIR) / "bin").make_preferred();
	std::filesystem::path vertexSpv = directoryPath / "test.vert.spv";
	std::vector<char> code = ReadShaderCode(vertexSpv);
	
	// Create reflection module:
	emberSpirvReflect::ShaderReflection reflection(code);
	reflection.GetVertexInputDescriptions();
	reflection.AddDescriptorBoundResources(nullptr);

    EXPECT_TRUE(true);
}



TEST(ShaderReflection, FragmentShaderReflection)
{
	// Load shader code:
	std::filesystem::path directoryPath = (std::filesystem::path(ENGINE_SHADERS_DIR) / "bin").make_preferred();
	std::filesystem::path fargmentSpv = directoryPath / "test.frag.spv";
	std::vector<char> code = ReadShaderCode(fargmentSpv);

	// Create reflection module:
	emberSpirvReflect::ShaderReflection reflection(code);
	reflection.GetVertexInputDescriptions();
	reflection.AddDescriptorBoundResources(nullptr);

	EXPECT_TRUE(true);
}



//TEST(ShaderReflection, ComputeShaderReflection)
//{
//	// Load shader code:
//	std::filesystem::path directoryPath = (std::filesystem::path(ENGINE_SHADERS_DIR) / "bin").make_preferred();
//	std::filesystem::path computeSpv = directoryPath / "test.comp.spv";
//	std::vector<char> code = ReadShaderCode(computeSpv);
//
//	// Create reflection module:
//	emberSpirvReflect::ShaderReflection reflection(code);
//
//	EXPECT_TRUE(true);
//}