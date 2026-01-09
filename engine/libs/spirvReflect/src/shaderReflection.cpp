#include "shaderReflection.h"
#include "logger.h"
#include "vulkanDescriptorSetLayoutBindingToString.h"
#include <assert.h>
#include <fstream>
#include <sstream>



namespace emberSpirvReflect
{
    // Public methods:
    // Constructor/Destructor:
    ShaderReflection::ShaderReflection(size_t descriptorSetCount)
    {
        m_isInitialized = false;
        m_descriptorSets.reserve(descriptorSetCount);
        for (uint32_t i = 0; i < descriptorSetCount; i++)
            m_descriptorSets.emplace_back(i);
    }
    ShaderReflection::~ShaderReflection()
    {

    }



    // Functioanlity:
    std::vector<char> ShaderReflection::ReadShaderCode(const std::filesystem::path& spvFile)
    {
        // Open shader file:
        std::ifstream file(spvFile, std::ios::binary);
        if (!file.is_open())
        {
            LOG_CRITICAL("Error opening shader file: {}", spvFile.string());
            throw std::runtime_error("Failed to open shader file: " + spvFile.string());
        }

        // Get file size:
        file.seekg(0, std::ios::end);
        size_t fileSize = static_cast<size_t>(file.tellg());
        file.seekg(0, std::ios::beg);

        // Copy code:
        std::vector<char> code(fileSize);
        file.read(code.data(), fileSize);
        file.close();

        return code;
    }
    void ShaderReflection::AddShaderStage(VkShaderStageFlagBits shaderStage, const std::vector<char>& code)
    {
        assert(!m_isInitialized && "Can't add further ShaderStageReflections to already initialized ShaderReflection.");
        assert(!GetStageReflection(shaderStage) && "Can't add a ShaderStageReflection twice to ShaderReflection.");
        assert(!GetStageReflection(VK_SHADER_STAGE_COMPUTE_BIT) && "Can't add ShaderStageReflection to ShaderReflection containing compute.");
        if (shaderStage == VK_SHADER_STAGE_COMPUTE_BIT)
            assert(m_shaderStageReflections.empty() && "Can't add compute ShaderStageReflection to none empty ShaderReflection.");
        m_shaderStageReflections.emplace_back(shaderStage, code);
    }
    void ShaderReflection::CreateDescriptorSets()
    {
        assert(!m_isInitialized && "Can't ShaderReflection::CreateDescriptorSets() twice.");

        // Fill descriptor sets:
        for (const ShaderStageReflection& stageReflection : m_shaderStageReflections)
        {
            const std::vector<Descriptor>& stageDescriptors = stageReflection.GetDescriptors();
            for (const Descriptor& descriptor : stageDescriptors)
            {
                if (descriptor.set >= m_descriptorSets.size())
                {
                    LOG_CRITICAL("Descriptor set index out of range: {}", std::to_string(descriptor.set));
                    assert(false);
                }
                m_descriptorSets[descriptor.set].AddDescriptor(descriptor);
            }
        }

        // Create descriptor set layot bindings:
        for (int i = 0; i < m_descriptorSets.size(); i++)
            m_descriptorSets[i].CreateVkDescriptorSetLayoutBindings();

        m_isInitialized = true;
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
    const DescriptorSet& ShaderReflection::GetDescriptorSet(uint32_t set) const
    {
        return m_descriptorSets[set];
    }
    const std::vector<DescriptorSet>& ShaderReflection::GetDescriptorSets() const
    {
        return m_descriptorSets;
    }



    // Debugging:
    std::string ShaderReflection::ToString() const
    {
        std::ostringstream ss;

        // Vertex inputs:
        if (const ShaderStageReflection* pShaderStageReflection = GetVertexShaderReflection())
        {
            if (const std::vector<VertexStageInfo>* vertexStageInfos = pShaderStageReflection->GetVertexInfos())
            {
                ss << "vertexInput:\n";
                for (const VertexStageInfo& vertexStageInfo : *vertexStageInfos)
                    ss << "  " << vertexStageInfo.ToString() << "\n";
            }
        }

        // Compute block size:
        if (const ShaderStageReflection* pShaderStageReflection = GetComputeShaderReflection())
        {
            if (const ComputeStageInfo* computeStageInfo = pShaderStageReflection->GetComputeInfo())
                ss << "blockSize: " << computeStageInfo->blockSize.ToString() << "\n";
        }

        // Descriptor sets:
        for (int i = 0; i < m_descriptorSets.size(); i++)
            if (m_descriptorSets[i].GetSet() == i) // else invalid.
                ss << m_descriptorSets[i].ToString() << "\n";

        return ss.str();
    }
    std::string ShaderReflection::ToStringAll() const
    {
        std::ostringstream ss;
        ss << "All shader stages:\n";
        int indent = 2;
        for (int i = 0; i < m_shaderStageReflections.size(); i++)
            ss << m_shaderStageReflections[i].ToString(indent);
        return ss.str();
    }
    void ShaderReflection::PrintDescriptorSetLayoutBindings()
    {
        for (auto& descriptorSet : m_descriptorSets)
        {
            LOG_INFO("Set{}", descriptorSet.GetSet());
            const std::vector<VkDescriptorSetLayoutBinding>& bindings = descriptorSet.GetVkDescriptorSetLayoutBindings();
            for (const auto& binding : bindings)
                LOG_TRACE(emberVulkanUtility::ToString(binding));
        }
    }
}