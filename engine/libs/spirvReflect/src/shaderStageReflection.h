#pragma once
#include "vertexInputDescription.h"
#include "descriptor.h"
#include "emberMath.h"
#include <memory>
#include <string>
#include <vector>
#include <vulkan/vulkan.h>
#include <variant>



// Forward declerations:
struct SpvReflectShaderModule;



namespace emberSpirvReflect
{
	// Stage specific info:
	struct VertexStageInfo
	{
		std::vector<VertexInputDescription> inputs;
	};
	struct ComputeStageInfo
	{
		Uint3 blockSize;
	};



	class ShaderStageReflection
	{
	private: // Members:
		VkShaderStageFlagBits m_shaderStage;
		std::unique_ptr<SpvReflectShaderModule> m_pModule;
		std::variant<std::monostate, VertexStageInfo, ComputeStageInfo> m_stageSpecific;
		std::vector<Descriptor> m_descriptors;

	public: // Methods:
		// Constructor/Destructor:
		ShaderStageReflection(VkShaderStageFlagBits shaderStage, const std::vector<char>& code);
		~ShaderStageReflection();

		// Non-copyable:
		ShaderStageReflection(const ShaderStageReflection&) = delete;
		ShaderStageReflection& operator=(const ShaderStageReflection&) = delete;

		// Movable:
		ShaderStageReflection(ShaderStageReflection&& other) noexcept;
		ShaderStageReflection& operator=(ShaderStageReflection&& other) noexcept;

		// Getters:
		VkShaderStageFlagBits GetShaderStage() const;
		const VertexStageInfo* GetVertexInfo() const;
		const ComputeStageInfo* GetComputeInfo() const;
		const std::vector<Descriptor>& GetDescriptors() const;

		// Debugging:
		std::string ToString(int indent = 0) const;

	private: // Methods:
		Uint3 ExtractBlockSize() const;
		std::vector<VertexInputDescription> ExtractVertexInputDescriptions() const;
		std::vector<Descriptor> ExtractDescriptors() const;
	};
}