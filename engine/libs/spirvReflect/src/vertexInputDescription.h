#pragma once
#include <string>
#include <vulkan/vulkan.h>



// Forward declerations:
struct SpvReflectInterfaceVariable;



namespace emberSpirvReflect
{
	struct VertexInputDescription
	{
	public: // Members:
		bool isValid;
		std::string semantic;
		VkVertexInputBindingDescription binding;
		VkVertexInputAttributeDescription attribute;

	public: // Methods:
		// Constructors:
		VertexInputDescription();
		VertexInputDescription(SpvReflectInterfaceVariable* pInput);

		// Debugging:
		std::string ToString() const;
	};
}