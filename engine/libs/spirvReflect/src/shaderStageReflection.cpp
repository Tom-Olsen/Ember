#include "shaderStageReflection.h"
#include "logger.h"
#include "spirvReflectToString.h"
#include "vulkanFormatToString.h"
#include "vulkanShaderStageFlagsToString.h"
#include "vulkanVertexInputRateToString.h"
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
    // VertexStageInfo struct:
	std::string VertexStageInfo::ToString() const
	{
    	std::ostringstream ss;
        ss << "semantic=" << semantic << ", ";
        ss << "location=" << location << ", ";
        ss << "typeSize=" << typeSize << ", ";
        ss << "vectorSize=" << vectorSize << ", ";
        ss << "format=" << emberVulkanUtility::ToString(format);
        return ss.str();
	}



    // Public methods:
    // Constructor/Destructor:
    ShaderStageReflection::ShaderStageReflection(VkShaderStageFlagBits shaderStage, const std::vector<char>& code)
    {
        m_shaderStage = shaderStage;
        m_pModule = std::make_unique<SpvReflectShaderModule>();
        SPVA(spvReflectCreateShaderModule(code.size(), code.data(), m_pModule.get()));

        switch (m_shaderStage)
        {
            case VK_SHADER_STAGE_COMPUTE_BIT:
                m_shaderStageInfo = ExtractComputeStageInfo();
                break;

            case VK_SHADER_STAGE_VERTEX_BIT:
                m_shaderStageInfo = ExtractVertexStageInfo();
                break;

            default:
                m_shaderStageInfo = std::monostate{};
                break;
        }
        m_descriptorReflections = ExtractDescriptorReflections();
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
    const std::vector<VertexStageInfo>* ShaderStageReflection::GetVertexInfos() const
    {
        return std::get_if<std::vector<VertexStageInfo>>(&m_shaderStageInfo);
    }
    const ComputeStageInfo* ShaderStageReflection::GetComputeInfo() const
    {
        return std::get_if<ComputeStageInfo>(&m_shaderStageInfo);
    }
    const std::vector<DescriptorReflection>& ShaderStageReflection::GetDescriptorReflections() const
    {
        return m_descriptorReflections;
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
        if (const std::vector<VertexStageInfo>* vertexStageInfos = GetVertexInfos())
        {
            ss << indentStr << "vertexInput:\n";
            for (const VertexStageInfo& vertexStageInfo : *vertexStageInfos)
                ss << indentStr << "  " << vertexStageInfo.ToString() << "\n";
        }

        // Compute block size:
        if (const ComputeStageInfo* computeStageInfo = GetComputeInfo())
            ss << indentStr << "blockSize: " << computeStageInfo->blockSize.ToString() << "\n";

        // Descriptor set reflections:
        if (m_descriptorReflections.size() > 0)
        {
            ss << indentStr << "descriptors:\n";
            for (int i = 0; i < m_descriptorReflections.size(); i++)
                ss << m_descriptorReflections[i].ToString(indent + 2) << "\n";
        }
        return ss.str();
    }



    // Private methods:
    ComputeStageInfo ShaderStageReflection::ExtractComputeStageInfo() const
    {
        const SpvReflectEntryPoint* entryPoint = spvReflectGetEntryPoint(m_pModule.get(), "main");
        if (entryPoint)
            return ComputeStageInfo{ Uint3(entryPoint->local_size.x, entryPoint->local_size.y, entryPoint->local_size.z) };
        else
        {
            LOG_WARN("Given shader does not contain the entry point 'main' or is not a compute shader!");
            return ComputeStageInfo{ Uint3::zero };
        }
    }
    std::vector<VertexStageInfo> ShaderStageReflection::ExtractVertexStageInfo() const
    {
        // Get vertex input reflection:
        uint32_t inputCount = 0;
        SPVA(spvReflectEnumerateInputVariables(m_pModule.get(), &inputCount, nullptr));
        std::vector<SpvReflectInterfaceVariable*> inputs(inputCount);
        SPVA(spvReflectEnumerateInputVariables(m_pModule.get(), &inputCount, inputs.data()));

        // Extract vertex input description information:
        std::vector<VertexStageInfo> vertexStageInfos;
        vertexStageInfos.reserve(inputs.size());
        for (uint32_t i = 0; i < inputs.size(); i++)
        {
            SpvReflectInterfaceVariable* pInput = inputs[i];
            //LOG_INFO("Vertex Input [{}]:", i);
            //LOG_TRACE(emberSpirvReflect::ToString(pInput));

            // Built in and array types:
            if (pInput->built_in != -1)
                continue; // built in system values (e.g. SV_InstanceID) are handled automatically by vulkan.
            if (pInput->array.dims_count > 0)
                continue; // array inputs not supported (yet).
            VertexStageInfo vertexStageInfo;

            // Semantic:
            vertexStageInfo.semantic = (pInput->name) ? std::string(pInput->name) : "";
            size_t pos = vertexStageInfo.semantic.rfind('.');
            if (pos != std::string::npos)
                vertexStageInfo.semantic = vertexStageInfo.semantic.substr(pos + 1);
            else    // shader semantic somehow broken.
                vertexStageInfo.semantic = "unknown";

            // Location, typeSize, vectorSize, format:
            vertexStageInfo.location = pInput->location;
            vertexStageInfo.typeSize = pInput->type_description->traits.numeric.scalar.width / 8;
            vertexStageInfo.vectorSize = pInput->type_description->traits.numeric.vector.component_count;
            vertexStageInfo.format = (VkFormat)pInput->format;

            vertexStageInfos.push_back(vertexStageInfo);
        }

        vertexStageInfos.shrink_to_fit();
        return vertexStageInfos;
    }
    std::vector<DescriptorReflection> ShaderStageReflection::ExtractDescriptorReflections() const
    {
        // Get descriptor set reflection:
        uint32_t setCount = 0;
        SPVA(spvReflectEnumerateDescriptorSets(m_pModule.get(), &setCount, nullptr));
        std::vector<SpvReflectDescriptorSet*> spvDescriptorSets(setCount);
        SPVA(spvReflectEnumerateDescriptorSets(m_pModule.get(), &setCount, spvDescriptorSets.data()));

        std::vector<DescriptorReflection> descriptorReflections;
        // Loop over sets/spaces:
        for (uint32_t i = 0; i < spvDescriptorSets.size(); i++)
        {
            SpvReflectDescriptorSet* pSpvDescriptorSet = spvDescriptorSets[i];
            //LOG_INFO("Descriptor Set [{}]:", i);
            //LOG_TRACE(emberSpirvReflect::ToString(pSpvDescriptorSet));

            // Loop over all bindings:
            for (uint32_t bindingIndex = 0; bindingIndex < pSpvDescriptorSet->binding_count; bindingIndex++)
            {
                SpvReflectDescriptorBinding* pBinding = pSpvDescriptorSet->bindings[bindingIndex];
                descriptorReflections.emplace_back(pBinding, m_shaderStage);
                //LOG_INFO(descriptors[descriptors.size() - 1].ToString());
            }
        }

        return descriptorReflections;
    }
}