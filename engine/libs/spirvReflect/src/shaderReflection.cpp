#include "shaderReflection.h"
#include "logger.h"
#include "vulkanDescriptorSetLayoutBindingToString.h"
#include <assert.h>
#include <fstream>
#include <sstream>
#include <vulkan/vulkan.h>



namespace emberSpirvReflect
{
    // Public methods:
    // Constructor/Destructor:
    ShaderReflection::ShaderReflection(size_t descriptorSetCount)
    {
        m_isInitialized = false;
        m_descriptorSetReflections.reserve(descriptorSetCount);
        for (uint32_t i = 0; i < descriptorSetCount; i++)
            m_descriptorSetReflections.emplace_back(i);
    }
    ShaderReflection::~ShaderReflection()
    {

    }

    // Movable:
    ShaderReflection::ShaderReflection(ShaderReflection&& other) noexcept
	    : m_isInitialized(other.m_isInitialized)
		, m_shaderStageReflections(std::move(m_shaderStageReflections))
		, m_descriptorSetReflections(std::move(m_descriptorSetReflections))
    {
        
    }
    ShaderReflection& ShaderReflection::operator=(ShaderReflection&& other) noexcept
    {
		if (this != &other)
		{
		    m_isInitialized = other.m_isInitialized;
		    m_shaderStageReflections = std::move(m_shaderStageReflections);
		    m_descriptorSetReflections = std::move(m_descriptorSetReflections);
		}
		return *this;
    }



    // Functioanlity:
    std::vector<char> ShaderReflection::ReadShaderCode(const std::filesystem::path& spvFile)
    {
        // Open shader file:
        std::ifstream file(spvFile, std::ios::binary);
        if (!file.is_open())
        {
            LOG_CRITICAL("ShaderReflection::ReadShaderCode: Error opening shader file: {}", spvFile.string());
            throw std::runtime_error("ShaderReflection::ReadShaderCode: Failed to open shader file: " + spvFile.string());
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
    void ShaderReflection::AddShaderStage(uint32_t shaderStage, const std::vector<char>& code)
    {
        assert(!m_isInitialized && "ShaderReflection::AddShaderStage: Can't add further ShaderStageReflections to already initialized ShaderReflection.");
        assert(!GetStageReflection(shaderStage) && "ShaderReflection::AddShaderStage: Can't add a ShaderStageReflection twice to ShaderReflection.");
        assert(!GetStageReflection(VK_SHADER_STAGE_COMPUTE_BIT) && "ShaderReflection::AddShaderStage: Can't add ShaderStageReflection to ShaderReflection containing compute.");
        if (shaderStage == VK_SHADER_STAGE_COMPUTE_BIT)
            assert(m_shaderStageReflections.empty() && "ShaderReflection::AddShaderStage: Can't add compute ShaderStageReflection to none empty ShaderReflection.");
        m_shaderStageReflections.emplace_back(shaderStage, code);
    }
    void ShaderReflection::CreateDescriptorSetReflections()
    {
        assert(!m_isInitialized && "Can't ShaderReflection::CreateDescriptorSets() twice.");

        // Fill descriptor set reflections:
        for (const ShaderStageReflection& stageReflection : m_shaderStageReflections)
        {
            const std::vector<DescriptorReflection>& stageDescriptorReflections = stageReflection.GetDescriptorReflections();
            for (const DescriptorReflection& descriptorReflection : stageDescriptorReflections)
            {
                if (descriptorReflection.GetSet() >= m_descriptorSetReflections.size())
                {
                    LOG_CRITICAL("ShaderReflection::CreateDescriptorSetReflections: Descriptor set index out of range: {}, highest allowed descriptorSet: {}", std::to_string(descriptorReflection.GetSet()), m_descriptorSetReflections.size() - 1);
                    assert(false);
                }
                m_descriptorSetReflections[descriptorReflection.GetSet()].AddDescriptorReflection(descriptorReflection);
            }
        }

        m_isInitialized = true;
    }


    // Getters:
    const ComputeStageInfo* ShaderReflection::GetComputeStageInfo() const
    {
        return GetComputeShaderReflection()->GetComputeInfo();
    }
    const VertexStageInfo* ShaderReflection::GetVertexStageInfo() const
    {
        return GetVertexShaderReflection()->GetVertexInfo();
    }
    const DescriptorSetReflection& ShaderReflection::GetDescriptorSetReflection(uint32_t set) const
    {
        return m_descriptorSetReflections[set];
    }
    const DescriptorReflection* ShaderReflection::GetDescriptorReflection(uint32_t setIndex, const std::string& name) const
    {
        if (setIndex >= m_descriptorSetReflections.size())
            return nullptr;

        const DescriptorSetReflection& descriptorSetReflection = m_descriptorSetReflections[setIndex];
        for (const auto& descriptor : descriptorSetReflection.GetDescriptorReflections())
        {
            if (descriptor.GetName() == name)
                return &descriptor;
        }

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
                for (const VertexAttributeInfo& vertexAttribute : vertexStageInfo->vertexAttributes)
                    ss << "  " << vertexAttribute.ToString() << "\n";
            }
        }

        // Compute block size:
        if (const ShaderStageReflection* pShaderStageReflection = GetComputeShaderReflection())
        {
            if (const ComputeStageInfo* computeStageInfo = pShaderStageReflection->GetComputeInfo())
                ss << "blockSize: " << computeStageInfo->blockSize.ToString() << "\n";
        }

        // Descriptor set reflections:
        for (int i = 0; i < m_descriptorSetReflections.size(); i++)
            //if (m_descriptorSetReflections[i].GetSet() == i) // else invalid.
                ss << m_descriptorSetReflections[i].ToString() << "\n";

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



    // Private methods:
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
    const ShaderStageReflection* ShaderReflection::GetStageReflection(uint32_t shaderStage) const
    {
        for (const auto& stageReflection : m_shaderStageReflections)
            if (stageReflection.GetShaderStage() == shaderStage)
                return &stageReflection;
        return nullptr;
    }
}