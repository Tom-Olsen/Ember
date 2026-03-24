#pragma once
#include "descriptorReflection.h"
#include "emberMath.h"
#include <memory>
#include <string>
#include <vector>
#include <variant>



// Forward declerations:
struct SpvReflectShaderModule;



namespace emberSpirvReflect
{
	// ShaderStageInfo variant:
	struct ComputeStageInfo
	{
		Uint3 blockSize;
	};
	struct VertexAttributeInfo
	{
		std::string semantic;
		uint32_t location;
		uint32_t typeSize;
		uint32_t vectorSize;
		uint32_t format;
		std::string ToString() const;
	};
	struct VertexStageInfo
	{
		std::vector<VertexAttributeInfo> vertexAttributes;
	};
	using ShaderStageInfo = std::variant<std::monostate, ComputeStageInfo, VertexStageInfo>;



	class ShaderStageReflection
	{
	private: // Members:
		uint32_t m_shaderStage;
		std::unique_ptr<SpvReflectShaderModule> m_pModule;
		ShaderStageInfo m_shaderStageInfo;
		std::vector<DescriptorReflection> m_descriptorReflections;

	public: // Methods:
		// Constructor/Destructor:
		ShaderStageReflection(uint32_t shaderStage, const std::vector<char>& code);
		~ShaderStageReflection();

		// Non-copyable:
		ShaderStageReflection(const ShaderStageReflection&) = delete;
		ShaderStageReflection& operator=(const ShaderStageReflection&) = delete;

		// Movable:
		ShaderStageReflection(ShaderStageReflection&& other) noexcept;
		ShaderStageReflection& operator=(ShaderStageReflection&& other) noexcept;

		// Getters:
		uint32_t GetShaderStage() const;
		const VertexStageInfo* GetVertexInfo() const;
		const ComputeStageInfo* GetComputeInfo() const;
		const std::vector<DescriptorReflection>& GetDescriptorReflections() const;

		// Debugging:
		std::string ToString(int indent = 0) const;

	private: // Methods:
		ComputeStageInfo ExtractComputeStageInfo() const;
		VertexStageInfo ExtractVertexStageInfo() const;
		std::vector<DescriptorReflection> ExtractDescriptorReflections() const;
	};
}