#include "logger.h"
#include "shaderReflection.h"
#include <fstream>
#include <filesystem>
#include <gtest/gtest.h>



TEST(ShaderReflection, VertexShaderReflection)
{
	try
	{
		// Load shader code:
		std::filesystem::path directoryPath = (std::filesystem::path(ENGINE_SHADERS_DIR) / "bin").make_preferred();
		std::filesystem::path vertexSpv = directoryPath / "test.vert.spv";
		std::vector<char> vertexCode = emberSpirvReflect::ShaderReflection::ReadShaderCode(vertexSpv);

		// Create reflection module:
		LOG_WARN("Starting vertex shader reflection test:");
		emberSpirvReflect::ShaderReflection shaderReflection;
		shaderReflection.AddShaderStage(VK_SHADER_STAGE_VERTEX_BIT, vertexCode);
		shaderReflection.CreateDescriptorSets();
		LOG_TRACE(shaderReflection.ToString());

		EXPECT_TRUE(true);
	}
	catch (const std::exception& e)
	{
		FAIL() << "Exception thrown:\n" << e.what();
	}
}



TEST(ShaderReflection, FragmentShaderReflection)
{
	try
	{
		// Load shader code:
		std::filesystem::path directoryPath = (std::filesystem::path(ENGINE_SHADERS_DIR) / "bin").make_preferred();
		std::filesystem::path fargmentSpv = directoryPath / "test.frag.spv";
		std::vector<char> fragmentCode = emberSpirvReflect::ShaderReflection::ReadShaderCode(fargmentSpv);

		// Create reflection module:
		LOG_WARN("Starting fragment shader reflection test:");
		emberSpirvReflect::ShaderReflection shaderReflection;
		shaderReflection.AddShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, fragmentCode);
		shaderReflection.CreateDescriptorSets();
		LOG_TRACE(shaderReflection.ToString());

		EXPECT_TRUE(true);
	}
	catch (const std::exception& e)
	{
		FAIL() << "Exception thrown:\n" << e.what();
	}
}



TEST(ShaderReflection, VertexAndFragmentShaderReflection)
{
	try
	{
		// Load shader code:
		std::filesystem::path directoryPath = (std::filesystem::path(ENGINE_SHADERS_DIR) / "bin").make_preferred();
		std::filesystem::path vertexSpv = directoryPath / "test.vert.spv";
		std::filesystem::path fargmentSpv = directoryPath / "test.frag.spv";
		std::vector<char> vertexCode = emberSpirvReflect::ShaderReflection::ReadShaderCode(vertexSpv);
		std::vector<char> fragmentCode = emberSpirvReflect::ShaderReflection::ReadShaderCode(fargmentSpv);

		// Create reflection module:
		LOG_WARN("Starting fragment+vertex shader reflection test:");
		emberSpirvReflect::ShaderReflection shaderReflection;
		shaderReflection.AddShaderStage(VK_SHADER_STAGE_VERTEX_BIT, vertexCode);
		shaderReflection.AddShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, fragmentCode);
		shaderReflection.CreateDescriptorSets();
		LOG_TRACE(shaderReflection.ToString());
		shaderReflection.PrintDescriptorSetLayoutBindings();

		EXPECT_TRUE(true);
	}
	catch (const std::exception& e)
	{
		FAIL() << "Exception thrown:\n" << e.what();
	}
}



TEST(ShaderReflection, ComputeShaderReflection)
{
	try
	{
		// Load shader code:
		std::filesystem::path directoryPath = (std::filesystem::path(ENGINE_SHADERS_DIR) / "bin").make_preferred();
		std::filesystem::path computeSpv = directoryPath / "test.comp.spv";
		std::vector<char> computeCode = emberSpirvReflect::ShaderReflection::ReadShaderCode(computeSpv);

		// Create reflection module:
		LOG_WARN("Starting compute shader reflection test:");
		emberSpirvReflect::ShaderReflection shaderReflection;
		shaderReflection.AddShaderStage(VK_SHADER_STAGE_COMPUTE_BIT, computeCode);
		shaderReflection.CreateDescriptorSets();
		LOG_TRACE(shaderReflection.ToString());

		EXPECT_TRUE(true);
	}
	catch (const std::exception& e)
	{
		FAIL() << "Exception thrown:\n" << e.what();
	}
}