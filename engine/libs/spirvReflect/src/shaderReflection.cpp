#include "shaderReflection.h"
#include <assert.h>
#include <sstream>



namespace emberSpirvReflect
{
    // Public methods:
    // Constructor/Destructor:
    ShaderReflection::ShaderReflection()
    {

    }
    ShaderReflection::~ShaderReflection()
    {

    }



    // Functioanlity:
    void ShaderReflection::AddShaderStage(VkShaderStageFlagBits shaderStage, const std::vector<char>& code)
    {
        assert(!GetStageReflection(shaderStage) && "Can't add a ShaderStageReflection twice to ShaderReflection.");
        if (shaderStage == VK_SHADER_STAGE_COMPUTE_BIT)
            assert(m_shaderStageReflections.empty() && "Can't add compute ShaderStageReflection to none empty ShaderReflection.");
        assert(!GetStageReflection(VK_SHADER_STAGE_COMPUTE_BIT) && "Can't add additional ShaderStageReflection to ShaderReflection containing compute.");

        m_shaderStageReflections.emplace_back(shaderStage, code);
    }
    void ShaderReflection::MergeDescriptors()
    {
        m_descriptors.clear();
        for (const ShaderStageReflection& stageReflection : m_shaderStageReflections)
        {
            const std::vector<Descriptor>& stageDescriptors = stageReflection.GetDescriptors();
            for (const Descriptor& descriptor : stageDescriptors)
            {
                bool merged = false;
                for (Descriptor& existingDescriptor : m_descriptors)
                {
                    // Same descriptor used in multiple stages → merge stage flags:
                    if (existingDescriptor.IsEqual(descriptor))
                    {
                        existingDescriptor.shaderStage |= descriptor.shaderStage;
                        merged = true;
                        break;
                    }
                }

                if (!merged)
                    m_descriptors.push_back(descriptor);
            }
        }
    }


    // Getters:
    const ShaderStageReflection* ShaderReflection::GetFragmentShaderReflection() const
    {
        return GetStageReflection(VK_SHADER_STAGE_FRAGMENT_BIT);
    }
    const ShaderStageReflection* ShaderReflection::GetVertexShaderReflection() const
    {
        return GetStageReflection(VK_SHADER_STAGE_VERTEX_BIT);
    }
    const ShaderStageReflection* ShaderReflection::GetComputeShaderReflection() const
    {
        return GetStageReflection(VK_SHADER_STAGE_COMPUTE_BIT);
    }
    const ShaderStageReflection* ShaderReflection::GetStageReflection(VkShaderStageFlagBits shaderStage) const
    {
        for (const auto& stageReflection : m_shaderStageReflections)
            if (stageReflection.GetShaderStage() == shaderStage)
                return &stageReflection;
        return nullptr;
    }



    // Debugging:
    std::string ShaderReflection::ToString() const
    {
        std::ostringstream ss;

        // Vertex inputs:
        if (const ShaderStageReflection* pShaderStageReflection = GetVertexShaderReflection())
        {
            if (const VertexStageInfo* vertexStageInfo = pShaderStageReflection->GetVertexInfo())
            {
                ss << "vertexInput:\n";
                for (int i = 0; i < vertexStageInfo->inputs.size(); i++)
                    ss << "  " << vertexStageInfo->inputs[i].ToString() << "\n";
            }
        }

        // Compute block size:
        if (const ShaderStageReflection* pShaderStageReflection = GetComputeShaderReflection())
        {
            if (const ComputeStageInfo* computeStageInfo = pShaderStageReflection->GetComputeInfo())
                ss << "blockSize: " << computeStageInfo->blockSize.ToString();
        }

        // Descriptor sets:
        if (m_descriptors.size() > 0)
        {
            ss << "descriptors:\n";
            for (int i = 0; i < m_descriptors.size(); i++)
                ss << m_descriptors[i].ToString(2) << "\n";
        }
        return ss.str();
    }
    std::string ShaderReflection::ToStringAll() const
    {
        std::ostringstream ss;
        ss << "All shader stages without merger:\n";
        int indent = 2;
        for (int i = 0; i < m_shaderStageReflections.size(); i++)
            ss << m_shaderStageReflections[i].ToString(indent);
        return ss.str();
    }
}