#pragma once
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
	struct ComputeStageInfo
	{
		Uint3 blockSize;
	};
	struct VertexStageInfo
	{
		std::string semantic;
		uint32_t location;
		uint32_t typeSize;
		uint32_t vectorSize;
		VkFormat format;
		std::string ToString() const;
	};



	class ShaderStageReflection
	{
	private: // Members:
		VkShaderStageFlagBits m_shaderStage;
		std::unique_ptr<SpvReflectShaderModule> m_pModule;
		std::variant<std::monostate, std::vector<VertexStageInfo>, ComputeStageInfo> m_stageSpecific;
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
		const std::vector<VertexStageInfo>* GetVertexInfos() const;
		const ComputeStageInfo* GetComputeInfo() const;
		const std::vector<Descriptor>& GetDescriptors() const;

		// Debugging:
		std::string ToString(int indent = 0) const;

	private: // Methods:
		ComputeStageInfo ExtractComputeStageInfo() const;
		std::vector<VertexStageInfo> ExtractVertexStageInfo() const;
		std::vector<Descriptor> ExtractDescriptors() const;
	};
}