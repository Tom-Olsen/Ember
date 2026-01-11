
// Rewrite binding and attribute description construction in vulkan backend!
VertexInputDescription::VertexInputDescription(SpvReflectInterfaceVariable* pInput)
{
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