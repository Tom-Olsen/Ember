#pragma once
#include "shaderStageReflection.h"
#include "descriptor.h"
#include "descriptorSet.h"
#include <array>
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
		std::array<DescriptorSet, 5> m_descriptorSets;

	public: // Methods:
		// Constructor/Destructor:
		ShaderReflection();
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
		void CreateDescriptorSets();

		// Getters:
		const ShaderStageReflection* GetFragmentShaderReflection() const;
		const ShaderStageReflection* GetVertexShaderReflection() const;
		const ShaderStageReflection* GetComputeShaderReflection() const;
		const ShaderStageReflection* GetStageReflection(VkShaderStageFlagBits stage) const;

		// Debugging:
		std::string ToString() const;
		std::string ToStringAll() const;
		void PrintDescriptorSetLayoutBindings();
	};
}