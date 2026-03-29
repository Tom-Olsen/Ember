#pragma once
#include "shaderStageReflection.h"
#include "descriptorReflection.h"
#include "descriptorSetReflection.h"
#include <filesystem>
#include <string>
#include <vector>



namespace emberSpirvReflect
{
	/// <summary>
	/// Build shader reflection by adding individual shader stages: <para/>
	/// -shaderReflection.AddShaderStage(VK_SHADER_STAGE_VERTEX_BIT, vertexCode); <para/>
	/// -shaderReflection.AddShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, fragmentCode); <para/>
	/// -... <para/>
	/// Extract information about individual stages: <para/>
	/// -Get(StageName)StageInfo(); <para/>
	/// Information about descriptorSet reflection: <para/>
	/// -GetDescriptorSetReflection(uint32_t set); <para/>
	/// -GetDescriptorSetReflection(uint32_t set).GetDescriptorReflections();
	/// </summary>
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

		// Movable:
		ShaderReflection(ShaderReflection&& other) noexcept;
		ShaderReflection& operator=(ShaderReflection&& other) noexcept;

		// Functionality:
		static std::vector<char> ReadShaderCode(const std::filesystem::path& spvFile);
		void AddShaderStage(uint32_t shaderStage, const std::vector<char>& code);
		void CreateDescriptorSetReflections();

		// Getters:
		const ComputeStageInfo* GetComputeStageInfo() const;
		const VertexStageInfo* GetVertexStageInfo() const;
		const DescriptorSetReflection& GetDescriptorSetReflection(uint32_t set) const;
		const DescriptorReflection* GetDescriptorReflection(uint32_t setIndex, const std::string& name) const;

		// Debugging:
		std::string ToString() const;
		std::string ToStringAll() const;

	private: // Methods:
		const ShaderStageReflection* GetFragmentShaderReflection() const;
		const ShaderStageReflection* GetVertexShaderReflection() const;
		const ShaderStageReflection* GetComputeShaderReflection() const;
		const ShaderStageReflection* GetStageReflection(uint32_t shaderStage) const;
	};
}