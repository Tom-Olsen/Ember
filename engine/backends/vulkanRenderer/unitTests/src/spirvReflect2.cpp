#include "spirvReflect2.h"
#include "logger.h"
#include <spirv_reflect.h>



// Macros:
#define SPIRV_REFLECT_ASSERT(val) \
if (val != SPV_REFLECT_RESULT_SUCCESS) { \
LOG_CRITICAL("File: {}, Line: {} SPIRV-Reflect error: {}", __FILE__, __LINE__, std::to_string(val)); std::abort(); }
#ifndef SPVA
    #define SPVA(f) SPIRV_REFLECT_ASSERT(f)
#endif



void Print(SpvReflectNumericTraits numeric, int indent = 0)
{
    std::string indentStr = std::string(indent, ' ');
    LOG_TRACE("{}numeric.scaler.width: {}", indentStr, numeric.scalar.width);
    LOG_TRACE("{}numeric.scaler.signedness: {}", indentStr, numeric.scalar.signedness);
    LOG_TRACE("{}numeric.vector.component_count: {}", indentStr, numeric.vector.component_count);
    LOG_TRACE("{}numeric.matrix.column_count: {}", indentStr, numeric.matrix.column_count);
    LOG_TRACE("{}numeric.matrix.row_count: {}", indentStr, numeric.matrix.row_count);
    LOG_TRACE("{}numeric.matrix.stride: {}", indentStr, numeric.matrix.stride);
}
void Print(SpvReflectImageTraits image, int indent = 0)
{
    std::string indentStr = std::string(indent, ' ');
    LOG_TRACE("{}image.dim: {}", indentStr, (int)image.dim);
    LOG_TRACE("{}image.depth: {}", indentStr, image.depth);
    LOG_TRACE("{}image.arrayed: {}", indentStr, image.arrayed);
    LOG_TRACE("{}image.ms: {}", indentStr, image.ms);
    LOG_TRACE("{}image.sampled: {}", indentStr, image.sampled);
    LOG_TRACE("{}image.image_format: {}", indentStr, (int)image.image_format);
}
void Print(SpvReflectArrayTraits array, int indent = 0)
{
    std::string indentStr = std::string(indent, ' ');
    LOG_TRACE("{}array.dims_count: {}", indentStr, array.dims_count);
    LOG_TRACE("{}array.stride: {}", indentStr, array.stride);
    for (int i = 0; i < array.dims_count; i++)
    {
        LOG_TRACE("  {}array.dims_count: {}", indentStr, array.dims[i]);
        LOG_TRACE("  {}array.dims_count: {}", indentStr, array.spec_constant_op_ids[i]);
    }
}

void Print(SpvReflectTypeDescription* type_description, int indent = 0)
{
    std::string indentStr = std::string(indent, ' ');
    LOG_TRACE("{}type_description.id: {}", indentStr, type_description->id);
    LOG_TRACE("{}type_description.op: {}", indentStr, (int)type_description->op);
    LOG_TRACE("{}type_description.type_name: {}", indentStr, type_description->type_name ? type_description->type_name : "");
    LOG_TRACE("{}type_description.struct_member_name: {}", indentStr, type_description->struct_member_name ? type_description->struct_member_name : "");
    LOG_TRACE("{}type_description.storage_class: {}", indentStr, type_description->storage_class);
    LOG_TRACE("{}type_description.type_flags: {}", indentStr, (int)type_description->type_flags);
    LOG_TRACE("{}type_description.decoration_flags: {}", indentStr, (int)type_description->decoration_flags);
    Print(type_description->traits.numeric, indent + 2);
    Print(type_description->traits.image, indent + 2);
    Print(type_description->traits.array, indent + 2);
}

void Print(SpvReflectInterfaceVariable* pVariable, int indent = 0)
{
    std::string indentStr = std::string(indent, ' ');
    LOG_TRACE("{}variable.spirv_id: {}", indentStr, pVariable->spirv_id);
    LOG_TRACE("{}variable.name: {}", indentStr, pVariable->name ? pVariable->name : "");
    LOG_TRACE("{}variable.location: {}", indentStr, pVariable->location);
    LOG_TRACE("{}variable.component: {}", indentStr, pVariable->component);
    LOG_TRACE("{}variable.storage_class: {}", indentStr, (int)pVariable->storage_class);
    LOG_TRACE("{}variable.semantic: {}", indentStr, pVariable->semantic ? pVariable->semantic : "");
    LOG_TRACE("{}variable.decoration_flags: {}", indentStr, (int)pVariable->decoration_flags);
    LOG_TRACE("{}variable.built_in: {}", indentStr, pVariable->built_in);
    Print(pVariable->numeric, indent + 2);
    Print(pVariable->array, indent + 2);
    LOG_TRACE("{}variable.member_count: {}", indentStr, pVariable->member_count);
    for (int i = 0; i < pVariable->member_count; i++)
        Print(&pVariable->members[i], indent + 2);
    LOG_TRACE("{}variable.format: {}", indentStr, (int)pVariable->format);
    if (pVariable->type_description)
        Print(pVariable->type_description, indent + 2);
    LOG_TRACE("{}variable.word_offset.location: {}\n\n\n", indentStr, pVariable->word_offset.location);
}



// Constructor/Destructor:
SpirvReflect2::SpirvReflect2(const std::vector<char>&code)
{
    m_pModule = std::make_unique<SpvReflectShaderModule>();
	SPVA(spvReflectCreateShaderModule(code.size(), code.data(), m_pModule.get()));
}
SpirvReflect2::~SpirvReflect2()
{
    if (m_pModule)
        spvReflectDestroyShaderModule(m_pModule.get());
}



// Public methods:
VertexInputDescriptions SpirvReflect2::GetVertexInputDescriptions() const
{
    // Input variables reflection:
    uint32_t inputCount = 0;
    SPVA(spvReflectEnumerateInputVariables(m_pModule.get(), &inputCount, nullptr));
    std::vector<SpvReflectInterfaceVariable*> inputs(inputCount);
    SPVA(spvReflectEnumerateInputVariables(m_pModule.get(), &inputCount, inputs.data()));

    // Reserve:
    std::vector<std::string> semantics;
    std::vector<VkVertexInputBindingDescription> bindings;
    std::vector<VkVertexInputAttributeDescription> attributes;
    semantics.reserve(inputs.size());
    bindings.reserve(inputs.size());
    attributes.reserve(inputs.size());

    int size = 0;
    for (uint32_t i = 0; i < inputs.size(); i++)
    {
        SpvReflectInterfaceVariable* pInput = inputs[i];
        Print(pInput);
        //if (pInput->built_in != -1) // Built in system values (e.g. SV_InstanceID) are handled automatically by vulkan.
        //    continue;
        //size++;
        //
        //// Compute stride:
        //uint32_t typeSize = pInput->type_description->traits.numeric.scalar.width / 8;
        //uint32_t vectorSize = pInput->type_description->traits.numeric.vector.component_count;
        //uint32_t stride = typeSize * vectorSize;
        //LOG_TRACE("typeSize: {}", typeSize);
        //LOG_TRACE("vectorSize: {}", vectorSize);
        //LOG_TRACE("stride: {}", stride);
        //
        //// Binding description:
        //VkVertexInputBindingDescription bindingDescription = {};
        //bindingDescription.binding = pInput->location;
        //bindingDescription.stride = stride;
        //bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        //bindings.push_back(bindingDescription);
        //LOG_TRACE("binding: {}", pInput->location);
        //LOG_TRACE("inputRate: {}", (int)VK_VERTEX_INPUT_RATE_VERTEX);
        //
        //// Semantic:
        //std::string semantic = (pInput->name) ? std::string(pInput->name) : "";
        //size_t pos = semantic.rfind('.');
        //if (pos != std::string::npos)
        //    semantic = semantic.substr(pos + 1);
        //else    // shader semantic somehow broken.
        //    semantic = "unknown";
        //semantics.push_back(semantic);
        //LOG_TRACE("input name: {}", pInput->name);
        //LOG_TRACE("semantic: {}", semantic);
        //
        //// Attribute description:
        //VkVertexInputAttributeDescription attributeDescription = {};
        //attributeDescription.binding = pInput->location;
        //attributeDescription.location = pInput->location;
        //attributeDescription.format = (VkFormat)pInput->format;
        //attributeDescription.offset = 0;
        //attributes.push_back(attributeDescription);
        //LOG_TRACE("Is array: {}", pInput->array.dims_count > 0);
        //LOG_TRACE("Has members: {}", pInput->member_count);
        //LOG_TRACE("format: {}\n", (int)pInput->format);
    }
    LOG_TRACE("input size: {}", inputs.size());
    LOG_TRACE("actual Input Size: {}", size);

    // Shrink to fit (above for loop has a continue):
    semantics.shrink_to_fit();
    bindings.shrink_to_fit();
    attributes.shrink_to_fit();

    // Package and return:
    VertexInputDescriptions vertexInputDescriptions;
    vertexInputDescriptions.size = size;
    vertexInputDescriptions.semantics = std::move(semantics);
    vertexInputDescriptions.bindings = std::move(bindings);
    vertexInputDescriptions.attributes = std::move(attributes);
    return vertexInputDescriptions;
}