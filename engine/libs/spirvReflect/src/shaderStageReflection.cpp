#include "shaderStageReflection.h"
#include "logger.h"
#include "spirvReflectToString.h"
#include "vulkanShaderStageFlagsToString.h"
#include <spirv_reflect.h>
#include <sstream>



// Macros:
#define SPIRV_REFLECT_ASSERT(val) \
if (val != SPV_REFLECT_RESULT_SUCCESS) { \
LOG_CRITICAL("File: {}, Line: {} SPIRV-Reflect error: {}", __FILE__, __LINE__, std::to_string(val)); std::abort(); }
#ifndef SPVA
#define SPVA(f) SPIRV_REFLECT_ASSERT(f)
#endif



namespace emberSpirvReflect
{
    // Public methods:
    // Constructor/Destructor:
    ShaderStageReflection::ShaderStageReflection(VkShaderStageFlagBits shaderStage, const std::vector<char>& code)
    {
        m_shaderStage = shaderStage;
        m_pModule = std::make_unique<SpvReflectShaderModule>();
        SPVA(spvReflectCreateShaderModule(code.size(), code.data(), m_pModule.get()));

        switch (m_shaderStage)
        {
            case VK_SHADER_STAGE_VERTEX_BIT:
                m_stageSpecific = VertexStageInfo{ ExtractVertexInputDescriptions() };
                break;

            case VK_SHADER_STAGE_COMPUTE_BIT:
                m_stageSpecific = ComputeStageInfo{ ExtractBlockSize() };
                break;

            default:
                m_stageSpecific = std::monostate{};
                break;
        }
        m_descriptors = ExtractDescriptors();
    }
    ShaderStageReflection::~ShaderStageReflection()
    {
        if (m_pModule)
            spvReflectDestroyShaderModule(m_pModule.get());
    }



    // Movable:
    ShaderStageReflection::ShaderStageReflection(ShaderStageReflection&& other) noexcept = default;
    ShaderStageReflection& ShaderStageReflection::operator=(ShaderStageReflection&& other) noexcept = default;



    // Getters:
    VkShaderStageFlagBits ShaderStageReflection::GetShaderStage() const
    {
        return m_shaderStage;
    }
    const VertexStageInfo* ShaderStageReflection::GetVertexInfo() const
    {
        return std::get_if<VertexStageInfo>(&m_stageSpecific);
    }
    const ComputeStageInfo* ShaderStageReflection::GetComputeInfo() const
    {
        return std::get_if<ComputeStageInfo>(&m_stageSpecific);
    }
    const std::vector<Descriptor>& ShaderStageReflection::GetDescriptors() const
    {
        return m_descriptors;
    }



    // Debugging:
    std::string ShaderStageReflection::ToString(int indent) const
    {
        std::ostringstream ss;
        std::string indentStr(indent, ' ');
        ss << indentStr << "shaderStage: " << emberVulkanUtility::ToString_VkShaderStageFlags(m_shaderStage) << "\n";
        indent += 2;
        indentStr = std::string(indent, ' ');

        // Vertex inputs:
        if (const VertexStageInfo* vertexStageInfo = GetVertexInfo())
        {
            ss << indentStr << "vertexInput:\n";
            for (int i = 0; i < vertexStageInfo->inputs.size(); i++)
                ss << indentStr << "  " << vertexStageInfo->inputs[i].ToString() << "\n";
        }

        // Compute block size:
        if (const ComputeStageInfo* computeStageInfo = GetComputeInfo())
            ss << indentStr << "blockSize: " << computeStageInfo->blockSize.ToString();

        // Descriptor sets:
        if (m_descriptors.size() > 0)
        {
            ss << indentStr << "descriptors:\n";
            for (int i = 0; i < m_descriptors.size(); i++)
                ss << m_descriptors[i].ToString(indent + 2) << "\n";
        }
        return ss.str();
    }



    // Private methods:
    Uint3 ShaderStageReflection::ExtractBlockSize() const
    {
        const SpvReflectEntryPoint* entryPoint = spvReflectGetEntryPoint(m_pModule.get(), "main");
        if (entryPoint)
            return Uint3(entryPoint->local_size.x, entryPoint->local_size.y, entryPoint->local_size.z);
        else
        {
            LOG_WARN("Given shader does not contain the entry point 'main' or is not a compute shader!");
            return Uint3::zero;
        }
    }
    std::vector<VertexInputDescription> ShaderStageReflection::ExtractVertexInputDescriptions() const
    {
        // Get vertex input reflection:
        uint32_t inputCount = 0;
        SPVA(spvReflectEnumerateInputVariables(m_pModule.get(), &inputCount, nullptr));
        std::vector<SpvReflectInterfaceVariable*> inputs(inputCount);
        SPVA(spvReflectEnumerateInputVariables(m_pModule.get(), &inputCount, inputs.data()));

        // Extract vertex input description information:
        std::vector<VertexInputDescription> vertexInputDescriptions;
        vertexInputDescriptions.reserve(inputs.size());
        for (uint32_t i = 0; i < inputs.size(); i++)
        {
            SpvReflectInterfaceVariable* pInput = inputs[i];
            //LOG_INFO("Vertex Input [{}]:", i);
            //LOG_TRACE(ToString(pInput));
            VertexInputDescription vertexInputDescription = VertexInputDescription(pInput);
            if (vertexInputDescription.isValid) // skips array and build in types.
            {
                //LOG_INFO(vertexInputDescription.ToString());
                vertexInputDescriptions.push_back(vertexInputDescription);
            }
        }

        vertexInputDescriptions.shrink_to_fit();
        return vertexInputDescriptions;
    }
    std::vector<Descriptor> ShaderStageReflection::ExtractDescriptors() const
    {
        // Get descriptor set reflection:
        uint32_t setCount = 0;
        SPVA(spvReflectEnumerateDescriptorSets(m_pModule.get(), &setCount, nullptr));
        std::vector<SpvReflectDescriptorSet*> descriptorSets(setCount);
        SPVA(spvReflectEnumerateDescriptorSets(m_pModule.get(), &setCount, descriptorSets.data()));

        std::vector<Descriptor> descriptors;
        // Loop over sets/spaces:
        for (uint32_t i = 0; i < descriptorSets.size(); i++)
        {
            SpvReflectDescriptorSet* pDescriptorSet = descriptorSets[i];
            //LOG_INFO("Descriptor Set [{}]:", i);
            //LOG_TRACE(emberSpirvReflect::ToString(pDescriptorSet));

            // Loop over all bindings:
            for (uint32_t bindingIndex = 0; bindingIndex < pDescriptorSet->binding_count; bindingIndex++)
            {
                SpvReflectDescriptorBinding* pBinding = pDescriptorSet->bindings[bindingIndex];
                descriptors.emplace_back(pBinding, m_shaderStage);
                //LOG_INFO(descriptors[descriptors.size() - 1].ToString());
            }
        }

        return descriptors;
    }
}