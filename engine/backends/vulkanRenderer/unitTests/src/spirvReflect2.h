#pragma once
#include <memory>
#include <string>
#include <vector>
#include <vulkan/vulkan.h>



// Forward declerations:
struct SpvReflectShaderModule;



// Helper structs:
struct VertexInputDescriptions
{
	uint32_t size = 0;
	std::vector<std::string> semantics;
	std::vector<VkVertexInputBindingDescription> bindings;
	std::vector<VkVertexInputAttributeDescription> attributes;
	//std::string ToString() const;
};



class SpirvReflect2
{
private: // Members:
	std::unique_ptr<SpvReflectShaderModule> m_pModule;

public: // Methods:
	// Constructor/Destructor:
	SpirvReflect2(const std::vector<char>& code);
	~SpirvReflect2();

	// Non-copyable:
	SpirvReflect2(const SpirvReflect2&) = delete;
	SpirvReflect2& operator=(const SpirvReflect2&) = delete;

	// Non-Movable:
	SpirvReflect2(SpirvReflect2&& other) noexcept = delete;
	SpirvReflect2& operator=(SpirvReflect2&& other) noexcept = delete;

	// Functionality:
	VertexInputDescriptions GetVertexInputDescriptions() const;
};