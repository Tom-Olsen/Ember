#pragma once
#include "vulkanFormatToString.h"
#include <spirv_reflect.h>
#include <sstream>
#include <vulkan/vulkan.h>



// ---------------------- Shader stage in/out struct reflection ----------------------
inline std::string ToString(const SpvDim& dim)
{
    switch (dim)
    {
        case SpvDim1D: return "SpvDim1D"; break;
        case SpvDim2D: return "SpvDim2D"; break;
        case SpvDim3D: return "SpvDim3D"; break;
        case SpvDimCube: return "SpvDimCube"; break;
        case SpvDimRect: return "SpvDimRect"; break;
        case SpvDimBuffer: return "SpvDimBuffer"; break;
        case SpvDimSubpassData: return "SpvDimSubpassData"; break;
        case SpvDimTileImageDataEXT: return "SpvDimTileImageDataEXT"; break;
        case SpvDimMax: return "SpvDimMax"; break;
        default: return "unknown";
    }
}

inline std::string ToString(const SpvReflectNumericTraits& numeric, int indent = 0)
{
    std::string indentStr = std::string(indent, ' ');
    std::ostringstream ss;
    ss << indentStr << "numeric.scalar.width: " << numeric.scalar.width << "\n";
    ss << indentStr << "numeric.scalar.signedness: " << numeric.scalar.signedness << "\n";
    ss << indentStr << "numeric.vector.component_count: " << numeric.vector.component_count << "\n";
    ss << indentStr << "numeric.matrix.column_count: " << numeric.matrix.column_count << "\n";
    ss << indentStr << "numeric.matrix.row_count: " << numeric.matrix.row_count << "\n";
    ss << indentStr << "numeric.matrix.stride: " << numeric.matrix.stride << "\n";
    return ss.str();
}
inline std::string ToString(const SpvReflectImageTraits& image, int indent = 0)
{
    std::string indentStr = std::string(indent, ' ');
    std::ostringstream ss;
    ss << indentStr << "image.dim: " << ToString(image.dim) << "\n";
    ss << indentStr << "image.depth: " << image.depth << "\n";
    ss << indentStr << "image.arrayed: " << image.arrayed << "\n";
    ss << indentStr << "image.ms: " << image.ms << "\n";
    ss << indentStr << "image.sampled: " << image.sampled << "\n";
    ss << indentStr << "image.image_format: " << emberVulkanUtility::ToString((VkFormat)image.image_format) << "\n";
    return ss.str();
}
inline std::string ToString(const SpvReflectArrayTraits& array, int indent = 0)
{
    std::string indentStr = std::string(indent, ' ');
    std::ostringstream ss;
    ss << indentStr << "array.dims_count: " << array.dims_count << "\n";
    ss << indentStr << "array.stride: " << array.stride << "\n";
    for (int i = 0; i < array.dims_count; i++)
    {
        ss << "  " << indentStr << "array.dims[" << i << "]: " << array.dims[i] << "\n";
        ss << "  " << indentStr << "array.spec_constant_op_ids[" << i << "]: " << array.spec_constant_op_ids[i] << "\n";
    }
    return ss.str();
}
inline std::string ToString(const SpvReflectBindingArrayTraits& array, int indent = 0)
{
    std::string indentStr = std::string(indent, ' ');
    std::ostringstream ss;
    ss << indentStr << "array.dims_count: " << array.dims_count << "\n";
    for (int i = 0; i < array.dims_count; i++)
        ss << "  " << indentStr << "array.dims[" << i << "]: " << array.dims[i] << "\n";
    return ss.str();
}

inline std::string ToStringA(const SpvReflectDecorationFlags& decoration_flags)
{
    if (decoration_flags == SPV_REFLECT_DECORATION_NONE)
        return "SPV_REFLECT_DECORATION_NONE";

    std::ostringstream ss;
    bool first = true;
    auto append = [&](SpvReflectDecorationFlags bit, const char* name)
    {
        if (decoration_flags & bit)
        {
            if (!first)
                ss << " | ";
            first = false;
            ss << name << "(0x" << std::hex << static_cast<uint32_t>(bit) << std::dec << ")";
        }
    };
    append(SPV_REFLECT_DECORATION_BLOCK, "SPV_REFLECT_DECORATION_BLOCK");
    append(SPV_REFLECT_DECORATION_BUFFER_BLOCK, "SPV_REFLECT_DECORATION_BUFFER_BLOCK");
    append(SPV_REFLECT_DECORATION_ROW_MAJOR, "SPV_REFLECT_DECORATION_ROW_MAJOR");
    append(SPV_REFLECT_DECORATION_COLUMN_MAJOR, "SPV_REFLECT_DECORATION_COLUMN_MAJOR");
    append(SPV_REFLECT_DECORATION_BUILT_IN, "SPV_REFLECT_DECORATION_BUILT_IN");
    append(SPV_REFLECT_DECORATION_NOPERSPECTIVE, "SPV_REFLECT_DECORATION_NOPERSPECTIVE");
    append(SPV_REFLECT_DECORATION_FLAT, "SPV_REFLECT_DECORATION_FLAT");
    append(SPV_REFLECT_DECORATION_NON_WRITABLE, "SPV_REFLECT_DECORATION_NON_WRITABLE");
    append(SPV_REFLECT_DECORATION_RELAXED_PRECISION, "SPV_REFLECT_DECORATION_RELAXED_PRECISION");
    append(SPV_REFLECT_DECORATION_NON_READABLE, "SPV_REFLECT_DECORATION_NON_READABLE");
    append(SPV_REFLECT_DECORATION_PATCH, "SPV_REFLECT_DECORATION_PATCH");
    append(SPV_REFLECT_DECORATION_PER_VERTEX, "SPV_REFLECT_DECORATION_PER_VERTEX");
    append(SPV_REFLECT_DECORATION_PER_TASK, "SPV_REFLECT_DECORATION_PER_TASK");
    append(SPV_REFLECT_DECORATION_WEIGHT_TEXTURE, "SPV_REFLECT_DECORATION_WEIGHT_TEXTURE");
    append(SPV_REFLECT_DECORATION_BLOCK_MATCH_TEXTURE, "SPV_REFLECT_DECORATION_BLOCK_MATCH_TEXTURE");

    if (first)
        ss << "UNKNOWN_REFLECT_DECORATION_FLAG_BITS(0x" << std::hex << static_cast<uint32_t>(decoration_flags) << std::dec << ")";

    return ss.str();
}
inline std::string ToString(SpvReflectTypeDescription* type_description, int indent = 0)
{
    std::string indentStr = std::string(indent, ' ');
    std::ostringstream ss;
    ss << indentStr << "type_description.id: " << type_description->id << "\n";
    ss << indentStr << "type_description.op(ToDo): " << (int)type_description->op << "\n";
    ss << indentStr << "type_description.type_name: " << (type_description->type_name ? type_description->type_name : "") << "\n";
    ss << indentStr << "type_description.struct_member_name: " << (type_description->struct_member_name ? type_description->struct_member_name : "") << "\n";
    ss << indentStr << "type_description.storage_class: " << type_description->storage_class << "\n";
    ss << indentStr << "type_description.type_flags(ToDo): " << (int)type_description->type_flags << "\n";
    ss << indentStr << "type_description.decoration_flags: " << (int)type_description->decoration_flags << "\n";
    ss << ToString(type_description->traits.numeric, indent + 2);
    ss << ToString(type_description->traits.image, indent + 2);
    ss << ToString(type_description->traits.array, indent + 2);
    return ss.str();
}

inline std::string ToString(const SpvReflectFormat& format)
{
    std::ostringstream ss;
    switch (format)
    {
        case SPV_REFLECT_FORMAT_UNDEFINED:              ss << "VK_FORMAT_UNDEFINED(" << static_cast<uint32_t>(VK_FORMAT_UNDEFINED) << ")"; break;
        case SPV_REFLECT_FORMAT_R16_UINT:               ss << "VK_FORMAT_R16_UINT(" << static_cast<uint32_t>(VK_FORMAT_R16_UINT) << ")"; break;
        case SPV_REFLECT_FORMAT_R16_SINT:               ss << "VK_FORMAT_R16_SINT(" << static_cast<uint32_t>(VK_FORMAT_R16_SINT) << ")"; break;
        case SPV_REFLECT_FORMAT_R16_SFLOAT:             ss << "VK_FORMAT_R16_SFLOAT(" << static_cast<uint32_t>(VK_FORMAT_R16_SFLOAT) << ")"; break;
        case SPV_REFLECT_FORMAT_R16G16_UINT:            ss << "VK_FORMAT_R16G16_UINT(" << static_cast<uint32_t>(VK_FORMAT_R16G16_UINT) << ")"; break;
        case SPV_REFLECT_FORMAT_R16G16_SINT:            ss << "VK_FORMAT_R16G16_SINT(" << static_cast<uint32_t>(VK_FORMAT_R16G16_SINT) << ")"; break;
        case SPV_REFLECT_FORMAT_R16G16_SFLOAT:          ss << "VK_FORMAT_R16G16_SFLOAT(" << static_cast<uint32_t>(VK_FORMAT_R16G16_SFLOAT) << ")"; break;
        case SPV_REFLECT_FORMAT_R16G16B16_UINT:         ss << "VK_FORMAT_R16G16B16_UINT(" << static_cast<uint32_t>(VK_FORMAT_R16G16B16_UINT) << ")"; break;
        case SPV_REFLECT_FORMAT_R16G16B16_SINT:         ss << "VK_FORMAT_R16G16B16_SINT(" << static_cast<uint32_t>(VK_FORMAT_R16G16B16_SINT) << ")"; break;
        case SPV_REFLECT_FORMAT_R16G16B16_SFLOAT:       ss << "VK_FORMAT_R16G16B16_SFLOAT(" << static_cast<uint32_t>(VK_FORMAT_R16G16B16_SFLOAT) << ")"; break;
        case SPV_REFLECT_FORMAT_R16G16B16A16_UINT:      ss << "VK_FORMAT_R16G16B16A16_UINT(" << static_cast<uint32_t>(VK_FORMAT_R16G16B16A16_UINT) << ")"; break;
        case SPV_REFLECT_FORMAT_R16G16B16A16_SINT:      ss << "VK_FORMAT_R16G16B16A16_SINT(" << static_cast<uint32_t>(VK_FORMAT_R16G16B16A16_SINT) << ")"; break;
        case SPV_REFLECT_FORMAT_R16G16B16A16_SFLOAT:    ss << "VK_FORMAT_R16G16B16A16_SFLOAT(" << static_cast<uint32_t>(VK_FORMAT_R16G16B16A16_SFLOAT) << ")"; break;
        case SPV_REFLECT_FORMAT_R32_UINT:               ss << "VK_FORMAT_R32_UINT(" << static_cast<uint32_t>(VK_FORMAT_R32_UINT) << ")"; break;
        case SPV_REFLECT_FORMAT_R32_SINT:               ss << "VK_FORMAT_R32_SINT(" << static_cast<uint32_t>(VK_FORMAT_R32_SINT) << ")"; break;
        case SPV_REFLECT_FORMAT_R32_SFLOAT:             ss << "VK_FORMAT_R32_SFLOAT(" << static_cast<uint32_t>(VK_FORMAT_R32_SFLOAT) << ")"; break;
        case SPV_REFLECT_FORMAT_R32G32_UINT:            ss << "VK_FORMAT_R32G32_UINT(" << static_cast<uint32_t>(VK_FORMAT_R32G32_UINT) << ")"; break;
        case SPV_REFLECT_FORMAT_R32G32_SINT:            ss << "VK_FORMAT_R32G32_SINT(" << static_cast<uint32_t>(VK_FORMAT_R32G32_SINT) << ")"; break;
        case SPV_REFLECT_FORMAT_R32G32_SFLOAT:          ss << "VK_FORMAT_R32G32_SFLOAT(" << static_cast<uint32_t>(VK_FORMAT_R32G32_SFLOAT) << ")"; break;
        case SPV_REFLECT_FORMAT_R32G32B32_UINT:         ss << "VK_FORMAT_R32G32B32_UINT(" << static_cast<uint32_t>(VK_FORMAT_R32G32B32_UINT) << ")"; break;
        case SPV_REFLECT_FORMAT_R32G32B32_SINT:         ss << "VK_FORMAT_R32G32B32_SINT(" << static_cast<uint32_t>(VK_FORMAT_R32G32B32_SINT) << ")"; break;
        case SPV_REFLECT_FORMAT_R32G32B32_SFLOAT:       ss << "VK_FORMAT_R32G32B32_SFLOAT(" << static_cast<uint32_t>(VK_FORMAT_R32G32B32_SFLOAT) << ")"; break;
        case SPV_REFLECT_FORMAT_R32G32B32A32_UINT:      ss << "VK_FORMAT_R32G32B32A32_UINT(" << static_cast<uint32_t>(VK_FORMAT_R32G32B32A32_UINT) << ")"; break;
        case SPV_REFLECT_FORMAT_R32G32B32A32_SINT:      ss << "VK_FORMAT_R32G32B32A32_SINT(" << static_cast<uint32_t>(VK_FORMAT_R32G32B32A32_SINT) << ")"; break;
        case SPV_REFLECT_FORMAT_R32G32B32A32_SFLOAT:    ss << "VK_FORMAT_R32G32B32A32_SFLOAT(" << static_cast<uint32_t>(VK_FORMAT_R32G32B32A32_SFLOAT) << ")"; break;
        case SPV_REFLECT_FORMAT_R64_UINT:               ss << "VK_FORMAT_R64_UINT(" << static_cast<uint32_t>(VK_FORMAT_R64_UINT) << ")"; break;
        case SPV_REFLECT_FORMAT_R64_SINT:               ss << "VK_FORMAT_R64_SINT(" << static_cast<uint32_t>(VK_FORMAT_R64_SINT) << ")"; break;
        case SPV_REFLECT_FORMAT_R64_SFLOAT:             ss << "VK_FORMAT_R64_SFLOAT(" << static_cast<uint32_t>(VK_FORMAT_R64_SFLOAT) << ")"; break;
        case SPV_REFLECT_FORMAT_R64G64_UINT:            ss << "VK_FORMAT_R64G64_UINT(" << static_cast<uint32_t>(VK_FORMAT_R64G64_UINT) << ")"; break;
        case SPV_REFLECT_FORMAT_R64G64_SINT:            ss << "VK_FORMAT_R64G64_SINT(" << static_cast<uint32_t>(VK_FORMAT_R64G64_SINT) << ")"; break;
        case SPV_REFLECT_FORMAT_R64G64_SFLOAT:          ss << "VK_FORMAT_R64G64_SFLOAT(" << static_cast<uint32_t>(VK_FORMAT_R64G64_SFLOAT) << ")"; break;
        case SPV_REFLECT_FORMAT_R64G64B64_UINT:         ss << "VK_FORMAT_R64G64B64_UINT(" << static_cast<uint32_t>(VK_FORMAT_R64G64B64_UINT) << ")"; break;
        case SPV_REFLECT_FORMAT_R64G64B64_SINT:         ss << "VK_FORMAT_R64G64B64_SINT(" << static_cast<uint32_t>(VK_FORMAT_R64G64B64_SINT) << ")"; break;
        case SPV_REFLECT_FORMAT_R64G64B64_SFLOAT:       ss << "VK_FORMAT_R64G64B64_SFLOAT(" << static_cast<uint32_t>(VK_FORMAT_R64G64B64_SFLOAT) << ")"; break;
        case SPV_REFLECT_FORMAT_R64G64B64A64_UINT:      ss << "VK_FORMAT_R64G64B64A64_UINT(" << static_cast<uint32_t>(VK_FORMAT_R64G64B64A64_UINT) << ")"; break;
        case SPV_REFLECT_FORMAT_R64G64B64A64_SINT:      ss << "VK_FORMAT_R64G64B64A64_SINT(" << static_cast<uint32_t>(VK_FORMAT_R64G64B64A64_SINT) << ")"; break;
        case SPV_REFLECT_FORMAT_R64G64B64A64_SFLOAT:    ss << "VK_FORMAT_R64G64B64A64_SFLOAT(" << static_cast<uint32_t>(VK_FORMAT_R64G64B64A64_SFLOAT) << ")"; break;
        default: ss << "unknown";
    }
    return ss.str();
}
inline std::string ToString(const SpvStorageClass& storage_class)
{
    std::ostringstream ss;
    switch (storage_class)
    {
        case SpvStorageClassUniformConstant:            ss << "SpvStorageClassUniformConstant("                 << static_cast<uint32_t>(SpvStorageClassUniformConstant)            << ")"; break;
        case SpvStorageClassInput:                      ss << "SpvStorageClassInput("                           << static_cast<uint32_t>(SpvStorageClassInput)                      << ")"; break;
        case SpvStorageClassUniform:                    ss << "SpvStorageClassUniform("                         << static_cast<uint32_t>(SpvStorageClassUniform)                    << ")"; break;
        case SpvStorageClassOutput:                     ss << "SpvStorageClassOutput("                          << static_cast<uint32_t>(SpvStorageClassOutput)                     << ")"; break;
        case SpvStorageClassWorkgroup:                  ss << "SpvStorageClassWorkgroup("                       << static_cast<uint32_t>(SpvStorageClassWorkgroup)                  << ")"; break;
        case SpvStorageClassCrossWorkgroup:             ss << "SpvStorageClassCrossWorkgroup("                  << static_cast<uint32_t>(SpvStorageClassCrossWorkgroup)             << ")"; break;
        case SpvStorageClassPrivate:                    ss << "SpvStorageClassPrivate("                         << static_cast<uint32_t>(SpvStorageClassPrivate)                    << ")"; break;
        case SpvStorageClassFunction:                   ss << "SpvStorageClassFunction("                        << static_cast<uint32_t>(SpvStorageClassFunction)                   << ")"; break;
        case SpvStorageClassGeneric:                    ss << "SpvStorageClassGeneric("                         << static_cast<uint32_t>(SpvStorageClassGeneric)                    << ")"; break;
        case SpvStorageClassPushConstant:               ss << "SpvStorageClassPushConstant("                    << static_cast<uint32_t>(SpvStorageClassPushConstant)               << ")"; break;
        case SpvStorageClassAtomicCounter:              ss << "SpvStorageClassAtomicCounter("                   << static_cast<uint32_t>(SpvStorageClassAtomicCounter)              << ")"; break;
        case SpvStorageClassImage:                      ss << "SpvStorageClassImage("                           << static_cast<uint32_t>(SpvStorageClassImage)                      << ")"; break;
        case SpvStorageClassStorageBuffer:              ss << "SpvStorageClassStorageBuffer("                   << static_cast<uint32_t>(SpvStorageClassStorageBuffer)              << ")"; break;
        case SpvStorageClassTileImageEXT:               ss << "SpvStorageClassTileImageEXT("                    << static_cast<uint32_t>(SpvStorageClassTileImageEXT)               << ")"; break;
        case SpvStorageClassTileAttachmentQCOM:         ss << "SpvStorageClassTileAttachmentQCOM("              << static_cast<uint32_t>(SpvStorageClassTileAttachmentQCOM)         << ")"; break;
        case SpvStorageClassNodePayloadAMDX:            ss << "SpvStorageClassNodePayloadAMDX("                 << static_cast<uint32_t>(SpvStorageClassNodePayloadAMDX)            << ")"; break;
        case SpvStorageClassCallableDataKHR:            ss << "SpvStorageClassCallableData(KHR/NV)("            << static_cast<uint32_t>(SpvStorageClassCallableDataKHR)            << ")"; break;
        case SpvStorageClassIncomingCallableDataKHR:    ss << "SpvStorageClassIncomingCallableData(KHR/NV)("    << static_cast<uint32_t>(SpvStorageClassIncomingCallableDataKHR)    << ")"; break;
        case SpvStorageClassRayPayloadKHR:              ss << "SpvStorageClassRayPayload(KHR/NV)("              << static_cast<uint32_t>(SpvStorageClassRayPayloadKHR)              << ")"; break;
        case SpvStorageClassHitAttributeKHR:            ss << "SpvStorageClassHitAttribute(KHR/NV)("            << static_cast<uint32_t>(SpvStorageClassHitAttributeKHR)            << ")"; break;
        case SpvStorageClassIncomingRayPayloadKHR:      ss << "SpvStorageClassIncomingRayPayload(KHR/NV)("      << static_cast<uint32_t>(SpvStorageClassIncomingRayPayloadKHR)      << ")"; break;
        case SpvStorageClassShaderRecordBufferKHR:      ss << "SpvStorageClassShaderRecordBuffer(KHR/NV)("      << static_cast<uint32_t>(SpvStorageClassShaderRecordBufferKHR)      << ")"; break;
        case SpvStorageClassPhysicalStorageBuffer:      ss << "SpvStorageClassPhysicalStorageBuffer(EXT)("      << static_cast<uint32_t>(SpvStorageClassPhysicalStorageBuffer)      << ")"; break;
        case SpvStorageClassHitObjectAttributeNV:       ss << "SpvStorageClassHitObjectAttributeNV("            << static_cast<uint32_t>(SpvStorageClassHitObjectAttributeNV)       << ")"; break;
        case SpvStorageClassTaskPayloadWorkgroupEXT:    ss << "SpvStorageClassTaskPayloadWorkgroupEXT("         << static_cast<uint32_t>(SpvStorageClassTaskPayloadWorkgroupEXT)    << ")"; break;
        case SpvStorageClassCodeSectionINTEL:           ss << "SpvStorageClassCodeSectionINTEL("                << static_cast<uint32_t>(SpvStorageClassCodeSectionINTEL)           << ")"; break;
        case SpvStorageClassDeviceOnlyINTEL:            ss << "SpvStorageClassDeviceOnlyINTEL("                 << static_cast<uint32_t>(SpvStorageClassDeviceOnlyINTEL)            << ")"; break;
        case SpvStorageClassHostOnlyINTEL:              ss << "SpvStorageClassHostOnlyINTEL("                   << static_cast<uint32_t>(SpvStorageClassHostOnlyINTEL)              << ")"; break;
        case SpvStorageClassMax:                        ss << "SpvStorageClassMax("                             << static_cast<uint32_t>(SpvStorageClassMax)                        << ")"; break;
        default: ss << "unknown";
    }
    return ss.str();
}

// Main object for shader in/out reflection:
inline std::string ToString(SpvReflectInterfaceVariable* pVariable, int indent = 0)
{
    std::string indentStr = std::string(indent, ' ');
    std::ostringstream ss;
    ss << indentStr << "variable.spirv_id: " << pVariable->spirv_id << "\n";
    ss << indentStr << "variable.name: " << (pVariable->name ? pVariable->name : "") << "\n";
    ss << indentStr << "variable.location: " << pVariable->location << "\n";
    ss << indentStr << "variable.component: " << pVariable->component << "\n";
    ss << indentStr << "variable.storage_class: " << ToString(pVariable->storage_class) << "\n";
    ss << indentStr << "variable.semantic: " << (pVariable->semantic ? pVariable->semantic : "") << "\n";
    ss << indentStr << "variable.decoration_flags: " << ToStringA(pVariable->decoration_flags) << "\n";
	ss << indentStr << "variable.built_in: " << pVariable->built_in << "\n";
    ss << indentStr << "variable.numeric:\n" << ToString(pVariable->numeric, indent + 2);
    ss << indentStr << "variable.array:\n" << ToString(pVariable->array, indent + 2);
    ss << indentStr << "variable.member_count: " << pVariable->member_count << "\n";
    for (int i = 0; i < pVariable->member_count; i++)
        ss << ToString(&pVariable->members[i], indent + 2);
    ss << indentStr << "variable.format: " << ToString(pVariable->format) << "\n";
    if (pVariable->type_description)
        ss << indentStr << "variable.type_description:\n" << ToString(pVariable->type_description, indent + 2);
    ss << indentStr << "variable.word_offset.location: " << pVariable->word_offset.location << "\n\n\n";
    return ss.str();
}
// -----------------------------------------------------------------------------------



// ---------------------------- Descriptor set reflection ----------------------------
inline std::string ToString(const SpvReflectVariableFlags& flags)
{
    switch (flags)
    {
        case SPV_REFLECT_VARIABLE_FLAGS_NONE:                   return "SPV_REFLECT_VARIABLE_FLAGS_NONE"; break;
        case SPV_REFLECT_VARIABLE_FLAGS_UNUSED:                 return "SPV_REFLECT_VARIABLE_FLAGS_UNUSED"; break;
        case SPV_REFLECT_VARIABLE_FLAGS_PHYSICAL_POINTER_COPY:  return "SPV_REFLECT_VARIABLE_FLAGS_PHYSICAL_POINTER_COPY"; break;
		default: return "unknown";
    }
}
inline std::string ToString(const SpvReflectBlockVariable& block, int indent = 0)
{
    std::string indentStr = std::string(indent, ' ');
    std::ostringstream ss;
	ss << indentStr << "block.spirv_id: " << block.spirv_id << "\n";
    ss << indentStr << "block.name: " << (block.name ? block.name : "") << "\n";
	ss << indentStr << "block.offset: " << block.offset << "\n";
	ss << indentStr << "block.absolute_offset: " << block.absolute_offset << "\n";
	ss << indentStr << "block.size: " << block.size << "\n";
	ss << indentStr << "block.padded_size: " << block.padded_size << "\n";
    ss << indentStr << "block.decoration_flags: " << ToString(block.decoration_flags) << "\n";
    ss << indentStr << "block.numeric:\n" << ToString(block.numeric, indent + 2);
    ss << indentStr << "block.array:\n" << ToString(block.array, indent + 2);
    ss << indentStr << "block.flags: " << ToString(block.flags) << "\n";
    ss << indentStr << "block.member_count: " << block.member_count << "\n";
    for (int i = 0; i < block.member_count; i++)
        ss << ToString(block.members[i], indent + 2);
    if (block.type_description)
        ss << indentStr << "block.type_description: " << ToString(block.type_description) << "\n";
    ss << indentStr << "block.word_offset.offset: " << block.word_offset.offset << "\n";
    return ss.str();
}
inline std::string ToString(const SpvReflectResourceType& resource_type)
{
    std::ostringstream ss;
    switch (resource_type)
    {
        case SPV_REFLECT_RESOURCE_FLAG_UNDEFINED:   ss << "SPV_REFLECT_RESOURCE_FLAG_UNDEFINED("    << (int)SPV_REFLECT_RESOURCE_FLAG_UNDEFINED <<  ")"; break;
        case SPV_REFLECT_RESOURCE_FLAG_SAMPLER:     ss << "SPV_REFLECT_RESOURCE_FLAG_SAMPLER("      << (int)SPV_REFLECT_RESOURCE_FLAG_SAMPLER <<    ")"; break;
        case SPV_REFLECT_RESOURCE_FLAG_CBV:         ss << "SPV_REFLECT_RESOURCE_FLAG_CBV("          << (int)SPV_REFLECT_RESOURCE_FLAG_CBV <<        ")"; break;
        case SPV_REFLECT_RESOURCE_FLAG_SRV:         ss << "SPV_REFLECT_RESOURCE_FLAG_SRV("          << (int)SPV_REFLECT_RESOURCE_FLAG_SRV <<        ")"; break;
        case SPV_REFLECT_RESOURCE_FLAG_UAV:         ss << "SPV_REFLECT_RESOURCE_FLAG_UAV("          << (int)SPV_REFLECT_RESOURCE_FLAG_UAV <<        ")"; break;
        default: ss << "unknown";
    }
    return ss.str();
}
inline std::string ToString(const SpvReflectDescriptorType& descriptor_type)
{
    std::ostringstream ss;
    switch (descriptor_type)
    {
        case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLER:                       ss << "VK_DESCRIPTOR_TYPE_SAMPLER("                     << static_cast<uint32_t>(VK_DESCRIPTOR_TYPE_SAMPLER)                    << ")"; break;
        case SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:        ss << "VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER("      << static_cast<uint32_t>(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)     << ")"; break;
        case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE:                 ss << "VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE("               << static_cast<uint32_t>(VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE)              << ")"; break;
        case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_IMAGE:                 ss << "VK_DESCRIPTOR_TYPE_STORAGE_IMAGE("               << static_cast<uint32_t>(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE)              << ")"; break;
        case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:          ss << "VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER("        << static_cast<uint32_t>(VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER)       << ")"; break;
        case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:          ss << "VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER("        << static_cast<uint32_t>(VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER)       << ")"; break;
        case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER:                ss << "VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER("              << static_cast<uint32_t>(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)             << ")"; break;
        case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER:                ss << "VK_DESCRIPTOR_TYPE_STORAGE_BUFFER("              << static_cast<uint32_t>(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER)             << ")"; break;
        case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:        ss << "VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC("      << static_cast<uint32_t>(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC)     << ")"; break;
        case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:        ss << "VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC("      << static_cast<uint32_t>(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC)     << ")"; break;
        case SPV_REFLECT_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:              ss << "VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT("            << static_cast<uint32_t>(VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT)           << ")"; break;
        case SPV_REFLECT_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR:    ss << "VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR("  << static_cast<uint32_t>(VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR) << ")"; break;
        default: ss << "unknown";
    }
    return ss.str();
}
inline std::string ToString(SpvReflectDescriptorBinding* pBinding, int index = 0, int indent = 0)
{
    std::string indentStr = std::string(indent, ' ');
    std::ostringstream ss;
    ss << indentStr << "pBinding[" << index << "].spirv_id: " << pBinding->spirv_id << "\n";
    ss << indentStr << "pBinding[" << index << "].name: " << (pBinding->name ? pBinding->name : "") << "\n";
    ss << indentStr << "pBinding[" << index << "].binding: " << pBinding->binding << "\n";
    ss << indentStr << "pBinding[" << index << "].input_attachment_index: " << pBinding->input_attachment_index << "\n";
    ss << indentStr << "pBinding[" << index << "].set: " << pBinding->set << "\n";
    ss << indentStr << "pBinding[" << index << "].descriptor_type: " << ToString(pBinding->descriptor_type) << "\n";
    ss << indentStr << "pBinding[" << index << "].resource_type: " << ToString(pBinding->resource_type) << "\n";
    ss << indentStr << "pBinding[" << index << "].image:\n" << ToString(pBinding->image, indent + 2);
    ss << indentStr << "pBinding[" << index << "].block:\n" << ToString(pBinding->block, indent + 2);
    ss << indentStr << "pBinding[" << index << "].array:\n" << ToString(pBinding->array, indent + 2);
    ss << indentStr << "pBinding[" << index << "].count: " << pBinding->count << "\n";
    ss << indentStr << "pBinding[" << index << "].accessed: " << pBinding->accessed << "\n";
    ss << indentStr << "pBinding[" << index << "].uav_counter_id: " << pBinding->uav_counter_id << "\n";
    if (pBinding->uav_counter_binding)
        ss << indentStr << "pBinding[" << index << "].uav_counter_binding:\n" << ToString(pBinding->uav_counter_binding, indent + 2) << "\n";
    ss << indentStr << "pBinding[" << index << "].byte_address_buffer_offset_count: " << pBinding->byte_address_buffer_offset_count << "\n";
    if (pBinding->byte_address_buffer_offsets)
        ss << indentStr << "pBinding[" << index << "].byte_address_buffer_offsets: " << *pBinding->byte_address_buffer_offsets << "\n";
    if (pBinding->type_description)
        ss << indentStr << "pBinding[" << index << "].type_description: " << ToString(pBinding->type_description) << "\n";
    ss << indentStr << "pBinding[" << index << "].word_offset.binding: " << pBinding->word_offset.binding << "\n";
    ss << indentStr << "pBinding[" << index << "].word_offset.set: " << pBinding->word_offset.set << "\n";
    ss << indentStr << "pBinding[" << index << "].decoration_flags: " << ToString(pBinding->decoration_flags) << "\n";
    ss << indentStr << "pBinding[" << index << "].user_type(ToDo): " << (int)pBinding->user_type << "\n";
    return ss.str();
}

// Main object for descriptor set reflection:
inline std::string ToString(SpvReflectDescriptorSet* pDescriptorSet, int indent = 0)
{
    std::string indentStr = std::string(indent, ' ');
    std::ostringstream ss;
    ss << indentStr << "descriptorSet.set: " << pDescriptorSet->set << "\n";
    ss << indentStr << "descriptorSet.binding_count: " << pDescriptorSet->binding_count << "\n";
    for (int i = 0; i < pDescriptorSet->binding_count; i++)
    {
        SpvReflectDescriptorBinding* pDescriptorBinding = pDescriptorSet->bindings[i];
        ss << ToString(pDescriptorBinding, i, indent + 2);
    }
    return ss.str();
}
// -----------------------------------------------------------------------------------