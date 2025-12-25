#pragma once
#include "vertexInputDescription.h"
#include "emberMath.h"
#include <memory>
#include <string>
#include <vector>
#include <vulkan/vulkan.h>



// Forward declerations:
struct SpvReflectShaderModule;
struct DescriptorBoundResources;



namespace emberSpirvReflect
{
	// Forward declerations:
	struct DescriptorBoundResources;



	class ShaderReflection
	{
	private: // Members:
		std::unique_ptr<SpvReflectShaderModule> m_pModule;

	public: // Methods:
		// Constructor/Destructor:
		ShaderReflection(const std::vector<char>& code);
		~ShaderReflection();

		// Non-copyable:
		ShaderReflection(const ShaderReflection&) = delete;
		ShaderReflection& operator=(const ShaderReflection&) = delete;

		// Non-Movable:
		ShaderReflection(ShaderReflection&& other) noexcept = delete;
		ShaderReflection& operator=(ShaderReflection&& other) noexcept = delete;

		// Functionality:
		// For compute shaders only:
		Uint3 GetBlockSize() const;
		// Assumes 1 buffer per vertex input, no interleaving:
		std::vector<VertexInputDescription> GetVertexInputDescriptions() const;
		// Instead of returning pointer, this function adds to one, as it is used for multiple pipeline stages, e.g. vertex + fragment.
		void AddDescriptorBoundResources(DescriptorBoundResources* const descriptorBoundResources, VkShaderStageFlagBits shaderStage) const;
	};
}