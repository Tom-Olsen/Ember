#include "vertexInputDescription.h"
#include "vulkanVertexInputBindingDescriptionToString.h"
#include "vulkanVertexInputAttributeDescriptionToString.h"
#include <spirv_reflect.h>
#include <sstream>



namespace emberSpirvReflect
{
	// Public methods:
    // Constructors:
	VertexInputDescription::VertexInputDescription()
        : isValid(false),
        semantic("unknown"),
        binding{ uint32_t(-1), uint32_t(-1), VK_VERTEX_INPUT_RATE_MAX_ENUM },
        attribute{ uint32_t(-1), uint32_t(-1), VK_FORMAT_UNDEFINED, uint32_t(-1) }
	{

	}
	VertexInputDescription::VertexInputDescription(SpvReflectInterfaceVariable* pInput)
        : isValid(false),
        semantic("unknown"),
        binding{ uint32_t(-1), uint32_t(-1), VK_VERTEX_INPUT_RATE_MAX_ENUM},
        attribute{ uint32_t(-1), uint32_t(-1), VK_FORMAT_UNDEFINED, uint32_t(-1) }
	{
        if (pInput->built_in != -1)
            return; // built in system values (e.g. SV_InstanceID) are handled automatically by vulkan.
        if (pInput->array.dims_count > 0)
            return; // array inputs not supported (yet).
        isValid = true;

        // Semantic:
        semantic = (pInput->name) ? std::string(pInput->name) : "";
        size_t pos = semantic.rfind('.');
        if (pos != std::string::npos)
            semantic = semantic.substr(pos + 1);
        else    // shader semantic somehow broken.
            semantic = "unknown";

        // Compute stride:
        uint32_t typeSize = pInput->type_description->traits.numeric.scalar.width / 8;
        uint32_t vectorSize = pInput->type_description->traits.numeric.vector.component_count;
        uint32_t stride = typeSize * vectorSize;

        // Binding description:
        binding.binding = pInput->location;
        binding.stride = stride;
        binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        // Attribute description:
        attribute.binding = pInput->location;
        attribute.location = pInput->location;
        attribute.format = (VkFormat)pInput->format;
        attribute.offset = 0;
	}



    // Debugging:
	std::string VertexInputDescription::ToString() const
	{
		std::ostringstream ss;
		ss << "semantic=" << semantic << ", ";
		ss << "binding={" << emberVulkanUtility::ToString(binding) << "}, ";
		ss << "attribute={" << emberVulkanUtility::ToString(attribute) << "}";
		return ss.str();
	}
}