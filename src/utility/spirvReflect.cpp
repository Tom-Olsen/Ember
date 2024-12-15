#include "spirvReflect.h"
#include "logger.h"



// Macro to check if a SPIRV-Reflect function returns SPV_REFLECT_RESULT_SUCCESS.
#define SPIRV_REFLECT_ASSERT(val) \
    if (val != SPV_REFLECT_RESULT_SUCCESS) { \
        LOG_CRITICAL("File: {}, Line: {} SPIRV-Reflect error: {}", __FILE__, __LINE__, std::to_string(val)); \
        std::abort(); \
    }

// Shorthand for SPIRV_REFLECT_ASSERT which can be disabled:
#ifndef SPVA
#define SPVA(f) SPIRV_REFLECT_ASSERT(f)
#endif



// UniformBufferMember public methods:
void UniformBufferMember::AddSubMember(std::string name, UniformBufferMember* pSubMember)
{
    // Take ownership of UniformBufferMember pointer:
    m_subMembers.emplace(name, pSubMember);
}
UniformBufferMember* UniformBufferMember::GetSubMember(const std::string & name) const
{
    auto it = m_subMembers.find(name);
    if (it != m_subMembers.end())
        return it->second;
    return nullptr;
}
std::string UniformBufferMember::ToString(const std::string& name, int indent) const
{
	std::string output(indent, ' ');
    output += name + ": offset=" + std::to_string(offset) + ", size=" + std::to_string(size) + "\n";
    for (const auto& [subMemberName, pSubMember] : m_subMembers)
		output += pSubMember->ToString(subMemberName, indent + 2);
	return output;
}


// UniformBufferBlock Constructor/Destructor:
UniformBufferBlock::UniformBufferBlock(const std::string& name, uint32_t size, uint32_t setIndex, uint32_t bindingIndex)
{
	this->name = name;
    this->size = size;
	this->setIndex = setIndex;
	this->bindingIndex = bindingIndex;
}
UniformBufferBlock::~UniformBufferBlock()
{
	for (auto& member : members)
		delete member.second;
}



// UniformBufferBlock public methods:
void UniformBufferBlock::AddMember(std::string name, UniformBufferMember* pMember)
{
    // Take ownership of UniformBufferMember pointer:
    members.emplace(name, pMember);

	// Adjust size:
    uint32_t newSize = pMember->offset + pMember->size;
    if (newSize > size)
        size = newSize;
}
UniformBufferMember* UniformBufferBlock::GetMember(const std::string& name) const
{
    auto it = members.find(name);
    if (it != members.end())
        return it->second;
    return nullptr;
}
std::string UniformBufferBlock::ToString() const
{
    std::string output = name + "(binding=" + std::to_string(bindingIndex) + ", size=" + std::to_string(size) + "):\n";
    for (const auto& [name, member] : members)
        output += member->ToString(name, 0);
    return output;
}



// SpirvReflect Constructor/Destructor:
SpirvReflect::SpirvReflect(const std::vector<char>& code)
{
    SPVA(spvReflectCreateShaderModule(code.size(), code.data(), &m_module));
}
SpirvReflect::~SpirvReflect()
{
    spvReflectDestroyShaderModule(&m_module);
}



// SpirvReflect public methods:
void SpirvReflect::GetInputBindingAndAttributeDescriptions(
    std::vector<VkVertexInputBindingDescription>& inputBindingDescriptions,
    std::vector<VkVertexInputAttributeDescription>& inputAttributeDescriptions,
    std::vector<std::string>& inputSemantics) const
{
    std::vector<SpvReflectInterfaceVariable*> inputs = GetInputVariablesReflection();
	for (uint32_t i = 0; i < inputs.size(); i++)
	{
		SpvReflectInterfaceVariable* pInput = inputs[i];
		uint32_t typeSize = pInput->type_description->traits.numeric.scalar.width / 8;
		uint32_t vectorSize = pInput->type_description->traits.numeric.vector.component_count;
        uint32_t size = typeSize * vectorSize;

		VkVertexInputBindingDescription bindingDescription = {};
        bindingDescription.binding = pInput->location;
        bindingDescription.stride = size;
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        inputBindingDescriptions.push_back(bindingDescription);

		std::string semantic = std::string(pInput->name);
        size_t pos = semantic.rfind('.');
		if (pos != std::string::npos)
			semantic = semantic.substr(pos + 1);
        else
			throw std::runtime_error("Semantic not found");
        inputSemantics.push_back(semantic);

        VkFormat format;
        switch (typeSize)
        {
        case 4: // 4 bytes per component (e.g., float, int)
            switch (vectorSize)
            {
            case 1: format = VK_FORMAT_R32_SFLOAT; break;
            case 2: format = VK_FORMAT_R32G32_SFLOAT; break;
            case 3: format = VK_FORMAT_R32G32B32_SFLOAT; break;
            case 4: format = VK_FORMAT_R32G32B32A32_SFLOAT; break;
            default: throw std::runtime_error("Unsupported vector size");
            }
            break;
        case 8: // 8 bytes per component (e.g., double)
            switch (vectorSize)
            {
            case 1: format = VK_FORMAT_R64_SFLOAT; break;
            case 2: format = VK_FORMAT_R64G64_SFLOAT; break;
            case 3: format = VK_FORMAT_R64G64B64_SFLOAT; break;
            case 4: format = VK_FORMAT_R64G64B64A64_SFLOAT; break;
            default: throw std::runtime_error("Unsupported vector size");
            }
            break;
        default:
            throw std::runtime_error("Unsupported scalar width");
        }

        VkVertexInputAttributeDescription attributeDescription = {};
        attributeDescription.binding = pInput->location;
        attributeDescription.location = pInput->location;
        attributeDescription.format = format;
        attributeDescription.offset = 0;
        inputAttributeDescriptions.push_back(attributeDescription);
	}
}
void SpirvReflect::GetDescriptorSetLayoutBindings(
    std::vector<VkDescriptorSetLayoutBinding>& descriptorSetBindings,
    std::vector<std::string>& descriptorSetBindingNames,
    std::unordered_map<std::string, UniformBufferBlock*>& uniformBufferBlockMap)
{
    // Shader descriptor set reflection:
    std::vector<SpvReflectDescriptorSet*> descriptorSetsReflection = GetDescriptorSetsReflection();

    for (uint32_t setIndex = 0; setIndex < descriptorSetsReflection.size(); setIndex++)
    {
        SpvReflectDescriptorSet* pSetReflection = descriptorSetsReflection[setIndex];
        for (uint32_t bindingIndex = 0; bindingIndex < pSetReflection->binding_count; bindingIndex++)
        {
            SpvReflectDescriptorBinding* pBindingReflection = pSetReflection->bindings[bindingIndex];
            VkDescriptorSetLayoutBinding layoutBinding = {};
            layoutBinding.binding = pBindingReflection->binding;
            layoutBinding.descriptorType = VkDescriptorType((int)pBindingReflection->descriptor_type);
            layoutBinding.descriptorCount = pBindingReflection->count;
            layoutBinding.stageFlags = VkShaderStageFlagBits((int)m_module.shader_stage);
            layoutBinding.pImmutableSamplers = nullptr;

            // Add binding and name to lists:
            descriptorSetBindings.push_back(layoutBinding);
            descriptorSetBindingNames.push_back(pBindingReflection->name);

            // In case of uniform buffer create UniformBufferBlock:
            if (pBindingReflection->descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
            {
                SpvReflectBlockVariable& blockReflection = pBindingReflection->block;
                UniformBufferBlock* pUniformBufferBlock = GetUniformBufferBlock(blockReflection, pSetReflection->set, pBindingReflection->binding);
                uniformBufferBlockMap.emplace(pUniformBufferBlock->name, pUniformBufferBlock);
            }
        }
    }
}



// SpirvReflect private static methods:
std::vector<SpvReflectInterfaceVariable*> SpirvReflect::GetInputVariablesReflection() const
{
	uint32_t inputCount = 0;
	SPVA(spvReflectEnumerateInputVariables(&m_module, &inputCount, nullptr));
	std::vector<SpvReflectInterfaceVariable*> inputs(inputCount);
	SPVA(spvReflectEnumerateInputVariables(&m_module, &inputCount, inputs.data()));
	return inputs;
}
std::vector<SpvReflectDescriptorSet*> SpirvReflect::GetDescriptorSetsReflection() const
{
    uint32_t setCount = 0;
    SPVA(spvReflectEnumerateDescriptorSets(&m_module, &setCount, nullptr));
    std::vector<SpvReflectDescriptorSet*> sets(setCount);
    SPVA(spvReflectEnumerateDescriptorSets(&m_module, &setCount, sets.data()));
    return sets;
}
UniformBufferBlock* SpirvReflect::GetUniformBufferBlock(const SpvReflectBlockVariable& blockReflection, uint32_t setIndex, uint32_t bindingIndex) const
{
    UniformBufferBlock* pBlock = new UniformBufferBlock(blockReflection.name, blockReflection.size, setIndex, bindingIndex);

    for (uint32_t memberIndex = 0; memberIndex < blockReflection.member_count; memberIndex++)
    {
        // Create UniformBufferMember:
        SpvReflectBlockVariable& memberReflection = blockReflection.members[memberIndex];
        UniformBufferMember* pMember = new UniformBufferMember();

        // Base member:
        pMember->offset = memberReflection.offset;
        pMember->size = memberReflection.size;

        // Submember struct reflection:
        if (IsStruct(memberReflection) && !IsArray(memberReflection))
			StructReflection(memberReflection, pMember);

        // Submember array reflection:
        if (IsArray(memberReflection))
			ArrayReflection(memberReflection.name, memberReflection, pMember);

        pBlock->AddMember(memberReflection.name, pMember);
    }

    return pBlock;
}
bool SpirvReflect::IsStruct(const SpvReflectBlockVariable& memberReflection) const
{
    return memberReflection.member_count > 0;
}
bool SpirvReflect::IsArray(const SpvReflectBlockVariable& memberReflection) const
{
	return memberReflection.array.dims_count > 0;
}
void SpirvReflect::StructReflection(const SpvReflectBlockVariable& memberReflection, UniformBufferMember* pMember) const
{
    for (uint32_t subMemberIndex = 0; subMemberIndex < memberReflection.member_count; subMemberIndex++)
    {
        SpvReflectBlockVariable& subMemberReflection = memberReflection.members[subMemberIndex];
        UniformBufferMember* pSubMember = new UniformBufferMember();

		// Base struct:
        pSubMember->offset = subMemberReflection.offset + pMember->offset;
        pSubMember->size = subMemberReflection.size;

        // Submember struct reflection:
        if (IsStruct(subMemberReflection) && !IsArray(subMemberReflection))
            StructReflection(subMemberReflection, pSubMember);

        // Submember array or arrayOfStruct reflection:
        if (IsArray(subMemberReflection))
            ArrayReflection(subMemberReflection.name, subMemberReflection, pSubMember);

        pMember->AddSubMember(subMemberReflection.name, pSubMember);
    }
}
void SpirvReflect::ArrayReflection(const std::string& arrayName, const SpvReflectBlockVariable& arrayReflection, UniformBufferMember* pMember) const
{
    for (uint32_t arrayIndex = 0; arrayIndex < arrayReflection.array.dims[0]; arrayIndex++)
    {
        UniformBufferMember* pElement = new UniformBufferMember();
        pElement->offset = pMember->offset + arrayReflection.array.stride * arrayIndex;
        pElement->size = arrayReflection.array.stride;

        // Submember struct reflection:
        if (IsStruct(arrayReflection))
            StructReflection(arrayReflection, pElement);
    
        pMember->AddSubMember(arrayName + "[" + std::to_string(arrayIndex) + "]", pElement);
    }
}