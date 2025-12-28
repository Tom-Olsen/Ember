#pragma once
#include "shaderStageReflection.h"
#include <string>
#include <vector>
#include <vulkan/vulkan.h>



namespace emberSpirvReflect
{
	class ShaderReflection
	{
	private: // Members:
		std::vector<ShaderStageReflection> m_shaderStageReflections;
		std::vector<Descriptor> m_descriptors;

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
		void AddShaderStage(VkShaderStageFlagBits shaderStage, const std::vector<char>& code);
		void MergeDescriptors();

		// Getters:
		const ShaderStageReflection* GetFragmentShaderReflection() const;
		const ShaderStageReflection* GetVertexShaderReflection() const;
		const ShaderStageReflection* GetComputeShaderReflection() const;
		const ShaderStageReflection* GetStageReflection(VkShaderStageFlagBits stage) const;

		// Debugging:
		std::string ToString() const;
		std::string ToStringAll() const;
	};
}