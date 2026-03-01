#pragma once
#include "shaderStageReflection.h"
#include "descriptorReflection.h"
#include "descriptorSetReflection.h"
#include <filesystem>
#include <string>
#include <vector>
#include <vulkan/vulkan.h>



namespace emberSpirvReflect
{
	class ShaderReflection
	{
	private: // Members:
		bool m_isInitialized;
		std::vector<ShaderStageReflection> m_shaderStageReflections;
		std::vector<DescriptorSetReflection> m_descriptorSetReflections;

	public: // Methods:
		// Constructor/Destructor:
		explicit ShaderReflection(size_t descriptorSetCount);
		~ShaderReflection();

		// Non-copyable:
		ShaderReflection(const ShaderReflection&) = delete;
		ShaderReflection& operator=(const ShaderReflection&) = delete;

		// Non-Movable:
		ShaderReflection(ShaderReflection&& other) noexcept = delete;
		ShaderReflection& operator=(ShaderReflection&& other) noexcept = delete;

		// Functionality:
		static std::vector<char> ReadShaderCode(const std::filesystem::path& spvFile);
		void AddShaderStage(VkShaderStageFlagBits shaderStage, const std::vector<char>& code);
		void CreateDescriptorSetReflections();

		// Getters:
		const ShaderStageReflection* GetFragmentShaderReflection() const;
		const ShaderStageReflection* GetVertexShaderReflection() const;
		const ShaderStageReflection* GetComputeShaderReflection() const;
		const ShaderStageReflection* GetStageReflection(VkShaderStageFlagBits stage) const;
		const DescriptorSetReflection& GetDescriptorSetReflection(uint32_t set) const;
		const std::vector<DescriptorSetReflection>& GetDescriptorSetReflections() const;

		// Debugging:
		std::string ToString() const;
		std::string ToStringAll() const;
		void PrintDescriptorSetLayoutBindings();
	};
}