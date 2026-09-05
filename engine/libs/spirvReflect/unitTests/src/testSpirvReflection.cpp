#include "logger.h"
#include "shaderReflection.h"
#include "spirvImageFormatToVulkan.h"
#include <array>
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <stdexcept>
#include <utility>
#include <vulkan/vulkan.h>



TEST(SpirvImageFormatToVulkan, ConvertsFormats)
{
	LOG_WARN("Starting spirv image format to vulkan format test:");
	const std::array<std::pair<SpvImageFormat, VkFormat>, 42> expectedFormats =
	{
		std::pair{ SpvImageFormatUnknown, VK_FORMAT_UNDEFINED },
		std::pair{ SpvImageFormatRgba32f, VK_FORMAT_R32G32B32A32_SFLOAT },
		std::pair{ SpvImageFormatRgba16f, VK_FORMAT_R16G16B16A16_SFLOAT },
		std::pair{ SpvImageFormatR32f, VK_FORMAT_R32_SFLOAT },
		std::pair{ SpvImageFormatRgba8, VK_FORMAT_R8G8B8A8_UNORM },
		std::pair{ SpvImageFormatRgba8Snorm, VK_FORMAT_R8G8B8A8_SNORM },
		std::pair{ SpvImageFormatRg32f, VK_FORMAT_R32G32_SFLOAT },
		std::pair{ SpvImageFormatRg16f, VK_FORMAT_R16G16_SFLOAT },
		std::pair{ SpvImageFormatR11fG11fB10f, VK_FORMAT_B10G11R11_UFLOAT_PACK32 },
		std::pair{ SpvImageFormatR16f, VK_FORMAT_R16_SFLOAT },
		std::pair{ SpvImageFormatRgba16, VK_FORMAT_R16G16B16A16_UNORM },
		std::pair{ SpvImageFormatRgb10A2, VK_FORMAT_A2B10G10R10_UNORM_PACK32 },
		std::pair{ SpvImageFormatRg16, VK_FORMAT_R16G16_UNORM },
		std::pair{ SpvImageFormatRg8, VK_FORMAT_R8G8_UNORM },
		std::pair{ SpvImageFormatR16, VK_FORMAT_R16_UNORM },
		std::pair{ SpvImageFormatR8, VK_FORMAT_R8_UNORM },
		std::pair{ SpvImageFormatRgba16Snorm, VK_FORMAT_R16G16B16A16_SNORM },
		std::pair{ SpvImageFormatRg16Snorm, VK_FORMAT_R16G16_SNORM },
		std::pair{ SpvImageFormatRg8Snorm, VK_FORMAT_R8G8_SNORM },
		std::pair{ SpvImageFormatR16Snorm, VK_FORMAT_R16_SNORM },
		std::pair{ SpvImageFormatR8Snorm, VK_FORMAT_R8_SNORM },
		std::pair{ SpvImageFormatRgba32i, VK_FORMAT_R32G32B32A32_SINT },
		std::pair{ SpvImageFormatRgba16i, VK_FORMAT_R16G16B16A16_SINT },
		std::pair{ SpvImageFormatRgba8i, VK_FORMAT_R8G8B8A8_SINT },
		std::pair{ SpvImageFormatR32i, VK_FORMAT_R32_SINT },
		std::pair{ SpvImageFormatRg32i, VK_FORMAT_R32G32_SINT },
		std::pair{ SpvImageFormatRg16i, VK_FORMAT_R16G16_SINT },
		std::pair{ SpvImageFormatRg8i, VK_FORMAT_R8G8_SINT },
		std::pair{ SpvImageFormatR16i, VK_FORMAT_R16_SINT },
		std::pair{ SpvImageFormatR8i, VK_FORMAT_R8_SINT },
		std::pair{ SpvImageFormatRgba32ui, VK_FORMAT_R32G32B32A32_UINT },
		std::pair{ SpvImageFormatRgba16ui, VK_FORMAT_R16G16B16A16_UINT },
		std::pair{ SpvImageFormatRgba8ui, VK_FORMAT_R8G8B8A8_UINT },
		std::pair{ SpvImageFormatR32ui, VK_FORMAT_R32_UINT },
		std::pair{ SpvImageFormatRgb10a2ui, VK_FORMAT_A2B10G10R10_UINT_PACK32 },
		std::pair{ SpvImageFormatRg32ui, VK_FORMAT_R32G32_UINT },
		std::pair{ SpvImageFormatRg16ui, VK_FORMAT_R16G16_UINT },
		std::pair{ SpvImageFormatRg8ui, VK_FORMAT_R8G8_UINT },
		std::pair{ SpvImageFormatR16ui, VK_FORMAT_R16_UINT },
		std::pair{ SpvImageFormatR8ui, VK_FORMAT_R8_UINT },
		std::pair{ SpvImageFormatR64ui, VK_FORMAT_R64_UINT },
		std::pair{ SpvImageFormatR64i, VK_FORMAT_R64_SINT }
	};

	for (const auto& [spirvFormat, vulkanFormat] : expectedFormats)
		EXPECT_EQ(emberSpirvReflect::ImageFormatSpirvToVulkan(spirvFormat), vulkanFormat);
	EXPECT_THROW(emberSpirvReflect::ImageFormatSpirvToVulkan(SpvImageFormatMax), std::invalid_argument);
}



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
		emberSpirvReflect::ShaderReflection shaderReflection(5);
		shaderReflection.AddShaderStage(VK_SHADER_STAGE_VERTEX_BIT, vertexCode);
		shaderReflection.CreateDescriptorSetReflections();
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
		emberSpirvReflect::ShaderReflection shaderReflection(5);
		shaderReflection.AddShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, fragmentCode);
		shaderReflection.CreateDescriptorSetReflections();
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
		emberSpirvReflect::ShaderReflection shaderReflection(5);
		shaderReflection.AddShaderStage(VK_SHADER_STAGE_VERTEX_BIT, vertexCode);
		shaderReflection.AddShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, fragmentCode);
		shaderReflection.CreateDescriptorSetReflections();
		LOG_TRACE(shaderReflection.ToString());

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
		emberSpirvReflect::ShaderReflection shaderReflection(5);
		shaderReflection.AddShaderStage(VK_SHADER_STAGE_COMPUTE_BIT, computeCode);
		shaderReflection.CreateDescriptorSetReflections();
		LOG_TRACE(shaderReflection.ToString());

		// Check inputImage reflection:
		const emberSpirvReflect::DescriptorReflection* pInputImage = shaderReflection.GetDescriptorReflection(3, "inputImage");
		ASSERT_NE(pInputImage, nullptr);
		EXPECT_EQ(pInputImage->GetDescriptorType(), VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE);
		const emberSpirvReflect::ImageDescriptor* pInputImageDescriptor = pInputImage->GetImageDescriptor();
		ASSERT_NE(pInputImageDescriptor, nullptr);
		EXPECT_EQ(pInputImageDescriptor->imageViewType, VK_IMAGE_VIEW_TYPE_2D);
		EXPECT_EQ(pInputImageDescriptor->imageFormat, VK_FORMAT_UNDEFINED);
		EXPECT_TRUE(pInputImageDescriptor->isReadable);
		EXPECT_FALSE(pInputImageDescriptor->isWritable);

		// Check outputImage reflection:
		const emberSpirvReflect::DescriptorReflection* pOutputImage = shaderReflection.GetDescriptorReflection(3, "outputImage");
		ASSERT_NE(pOutputImage, nullptr);
		EXPECT_EQ(pOutputImage->GetDescriptorType(), VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
		const emberSpirvReflect::ImageDescriptor* pOutputImageDescriptor = pOutputImage->GetImageDescriptor();
		ASSERT_NE(pOutputImageDescriptor, nullptr);
		EXPECT_EQ(pOutputImageDescriptor->imageViewType, VK_IMAGE_VIEW_TYPE_2D);
		EXPECT_EQ(pOutputImageDescriptor->imageFormat, VK_FORMAT_R16G16B16A16_SFLOAT);
		EXPECT_TRUE(pOutputImageDescriptor->isReadable);
		EXPECT_TRUE(pOutputImageDescriptor->isWritable);
	}
	catch (const std::exception& e)
	{
		FAIL() << "Exception thrown:\n" << e.what();
	}
}