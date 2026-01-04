#pragma once
#include "shaderStageReflection.h"
#include "descriptor.h"
#include "descriptorSet.h"
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
		std::vector<DescriptorSet> m_descriptorSets;

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
		void CreateDescriptorSets();

		// Getters:
		const ShaderStageReflection* GetFragmentShaderReflection() const;
		const ShaderStageReflection* GetVertexShaderReflection() const;
		const ShaderStageReflection* GetComputeShaderReflection() const;
		const ShaderStageReflection* GetStageReflection(VkShaderStageFlagBits stage) const;
		const DescriptorSet& GetDescriptorSet(uint32_t set) const;
		const std::vector<DescriptorSet>& GetDescriptorSets() const;

		// Debugging:
		std::string ToString() const;
		std::string ToStringAll() const;
		void PrintDescriptorSetLayoutBindings();
	};
}