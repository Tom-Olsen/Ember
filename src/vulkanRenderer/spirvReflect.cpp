#include "spirvReflect.h"
#include "logger.h"
#include "vulkanUtility.h"



namespace emberEngine
{
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



    // -------------------- UniformBufferMember --------------------
    // Constructor/Destructor:
    UniformBufferMember::UniformBufferMember()
    {
        offset = size = 0;
    }
    UniformBufferMember::~UniformBufferMember()
    {

    }

    // Public methods:
    void UniformBufferMember::AddSubMember(std::string name, UniformBufferMember* pSubMember)
    {
        // Take ownership of UniformBufferMember pointer:
        m_subMembers.emplace(name, pSubMember);
    }
    UniformBufferMember* UniformBufferMember::GetSubMember(const std::string& name) const
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
    // -------------------------------------------------------------



    // -------------------- UniformBufferBlock ---------------------
    // Constructor/Destructor:
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

    // Public methods:
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
    std::string UniformBufferBlock::ToString(int indent) const
    {
        std::string output(indent, ' ');
        output += name + "(binding=" + std::to_string(bindingIndex) + ", size=" + std::to_string(size) + "):\n";
        for (const auto& [name, member] : members)
            output += member->ToString(name, indent + 2);
        return output;
    }
    // -------------------------------------------------------------



    // ------------------ VertexInputDescriptions ------------------
    // Public methods:
    std::string VertexInputDescriptions::ToString() const
    {
        std::string output = "";
        for (int i = 0; i < size; i++)
        {
            output += semantics[i] + ":\n";
            output += "  BindingDescription:   binding=" + std::to_string(bindings[i].binding) + ", stride=" + std::to_string(bindings[i].stride) + ", inputRate=" + vulkanObjToString::VkVertexInputRateToString(bindings[i].inputRate) + "\n";
            output += "  AttributeDescription: binding=" + std::to_string(attributes[i].binding) + ", format=" + vulkanObjToString::VkFormatToString(attributes[i].format) + ", location=" + std::to_string(attributes[i].location) + ", offset=" + std::to_string(attributes[i].offset) + "\n";
        }
        return output;
    }
    // -------------------------------------------------------------
     
     
     
    // ------------------ DescriptorBoundResources -----------------
    std::string DescriptorBoundResources::ToString() const
    {
        std::string output = "descriptorSetBindingNames:(" + std::to_string(bindingCount) + ")\n";

        output += "\ndescriptorSetLayoutBindings:\n";
        for (int i = 0; i < bindingCount; i++)
        {
            output += "  " + descriptorSetBindingNames[i] + ": binding=" + std::to_string(descriptorSetLayoutBindings[i].binding) + ", count=" + std::to_string(descriptorSetLayoutBindings[i].descriptorCount) + ", type=" + vulkanObjToString::VkDescriptorTypeToString(descriptorSetLayoutBindings[i].descriptorType) + ", stage=" + vulkanObjToString::VkShaderStageFlagsToString(descriptorSetLayoutBindings[i].stageFlags);

            auto it = uniformBufferBlockMap.find(descriptorSetBindingNames[i]);
            if (it != uniformBufferBlockMap.end())
            {
                output += "\n" + it->second->ToString(4);
                continue;
            }

            auto itt = sampleViewTypeMap.find(descriptorSetBindingNames[i]);
            if (itt != sampleViewTypeMap.end())
            {
                output += ", viewType=" + vulkanObjToString::VkImageViewTypeToString(itt->second) + "\n";
                continue;
            }

            auto ittt = storageViewTypeMap.find(descriptorSetBindingNames[i]);
            if (ittt != storageViewTypeMap.end())
            {
                output += ", viewType=" + vulkanObjToString::VkImageViewTypeToString(ittt->second) + "\n";
                continue;
            }

            output += "\n"; // extra new line in case of samplers as they do not have their own extra output which already adds the new line.
        }

        return output;
    }
    // -------------------------------------------------------------



    // ----------------------- SpirvReflect ------------------------
    // Constructor/Destructor:
    SpirvReflect::SpirvReflect(const std::vector<char>& code)
    {
        SPVA(spvReflectCreateShaderModule(code.size(), code.data(), &m_module));
    }
    SpirvReflect::~SpirvReflect()
    {
        spvReflectDestroyShaderModule(&m_module);
    }

    // Public methods:
    VertexInputDescriptions* SpirvReflect::GetVertexInputDescriptions() const
    {
        std::vector<SpvReflectInterfaceVariable*> inputs = GetInputVariablesReflection();
        VertexInputDescriptions* vertexInputDescriptions = new VertexInputDescriptions();
        vertexInputDescriptions->semantics.reserve(inputs.size());
        vertexInputDescriptions->bindings.reserve(inputs.size());
        vertexInputDescriptions->attributes.reserve(inputs.size());

        for (uint32_t i = 0; i < inputs.size(); i++)
        {
            SpvReflectInterfaceVariable* pInput = inputs[i];
            if (pInput->built_in != -1) // Built in system values (e.g. SV_InstanceID) are handles automatically by vulkan.
                continue;

            uint32_t typeSize = pInput->type_description->traits.numeric.scalar.width / 8;
            uint32_t vectorSize = pInput->type_description->traits.numeric.vector.component_count;
            uint32_t size = typeSize * vectorSize;

            VkVertexInputBindingDescription bindingDescription = {};
            bindingDescription.binding = pInput->location;
            bindingDescription.stride = size;
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
            vertexInputDescriptions->bindings.push_back(bindingDescription);

            std::string semantic = std::string(pInput->name);
            size_t pos = semantic.rfind('.');
            if (pos != std::string::npos)
                semantic = semantic.substr(pos + 1);
            else    // shader semantic somehow broken.
                vertexInputDescriptions->semantics.push_back("unknown");
            vertexInputDescriptions->semantics.push_back(semantic);

            VkVertexInputAttributeDescription attributeDescription = {};
            attributeDescription.binding = pInput->location;
            attributeDescription.location = pInput->location;
            attributeDescription.format = (VkFormat)pInput->format;
            attributeDescription.offset = 0;
            vertexInputDescriptions->attributes.push_back(attributeDescription);
        }

        // Shrink to fitt, as due to built in system values original reserve might have been to big:
        vertexInputDescriptions->semantics.shrink_to_fit();
        vertexInputDescriptions->bindings.shrink_to_fit();
        vertexInputDescriptions->attributes.shrink_to_fit();
        vertexInputDescriptions->size = vertexInputDescriptions->semantics.size();

        return vertexInputDescriptions;
    }
    void SpirvReflect::AddDescriptorBoundResources(DescriptorBoundResources* const descriptorBoundResources) const
    {
        std::vector<SpvReflectDescriptorSet*> descriptorSetsReflection = GetDescriptorSetsReflection();

        for (uint32_t setIndex = 0; setIndex < descriptorSetsReflection.size(); setIndex++)
        {
            SpvReflectDescriptorSet* pSetReflection = descriptorSetsReflection[setIndex];
            descriptorBoundResources->bindingCount += pSetReflection->binding_count;
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
                descriptorBoundResources->descriptorSetBindingNames.push_back(pBindingReflection->name);
                descriptorBoundResources->descriptorSetLayoutBindings.push_back(layoutBinding);

                // In case sample texture retrieve view type:
                if (pBindingReflection->descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE)
                {
                    if (pBindingReflection->image.dim == SpvDim1D && !pBindingReflection->image.arrayed)
                        descriptorBoundResources->sampleViewTypeMap.emplace(pBindingReflection->name, VK_IMAGE_VIEW_TYPE_1D);
                    else if (pBindingReflection->image.dim == SpvDim1D && pBindingReflection->image.arrayed)
                        descriptorBoundResources->sampleViewTypeMap.emplace(pBindingReflection->name, VK_IMAGE_VIEW_TYPE_1D_ARRAY);
                    if (pBindingReflection->image.dim == SpvDim2D && !pBindingReflection->image.arrayed)
                        descriptorBoundResources->sampleViewTypeMap.emplace(pBindingReflection->name, VK_IMAGE_VIEW_TYPE_2D);
                    else if (pBindingReflection->image.dim == SpvDim2D && pBindingReflection->image.arrayed)
                        descriptorBoundResources->sampleViewTypeMap.emplace(pBindingReflection->name, VK_IMAGE_VIEW_TYPE_2D_ARRAY);
                    if (pBindingReflection->image.dim == SpvDim3D && !pBindingReflection->image.arrayed)
                        descriptorBoundResources->sampleViewTypeMap.emplace(pBindingReflection->name, VK_IMAGE_VIEW_TYPE_3D);
                    // VK_IMAGE_VIEW_TYPE_3D_ARRAY does not exist in HLSL.
                    if (pBindingReflection->image.dim == SpvDimCube && !pBindingReflection->image.arrayed)
                        descriptorBoundResources->sampleViewTypeMap.emplace(pBindingReflection->name, VK_IMAGE_VIEW_TYPE_CUBE);
                    if (pBindingReflection->image.dim == SpvDimCube && pBindingReflection->image.arrayed)
                        descriptorBoundResources->sampleViewTypeMap.emplace(pBindingReflection->name, VK_IMAGE_VIEW_TYPE_CUBE_ARRAY);
                }
                // In case storage texture retrieve view type:
                if (pBindingReflection->descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_IMAGE)
                {
                    if (pBindingReflection->image.dim == SpvDim1D && !pBindingReflection->image.arrayed)
                        descriptorBoundResources->storageViewTypeMap.emplace(pBindingReflection->name, VK_IMAGE_VIEW_TYPE_1D);
                    else if (pBindingReflection->image.dim == SpvDim1D && pBindingReflection->image.arrayed)
                        descriptorBoundResources->storageViewTypeMap.emplace(pBindingReflection->name, VK_IMAGE_VIEW_TYPE_1D_ARRAY);
                    if (pBindingReflection->image.dim == SpvDim2D && !pBindingReflection->image.arrayed)
                        descriptorBoundResources->storageViewTypeMap.emplace(pBindingReflection->name, VK_IMAGE_VIEW_TYPE_2D);
                    else if (pBindingReflection->image.dim == SpvDim2D && pBindingReflection->image.arrayed)
                        descriptorBoundResources->storageViewTypeMap.emplace(pBindingReflection->name, VK_IMAGE_VIEW_TYPE_2D_ARRAY);
                    if (pBindingReflection->image.dim == SpvDim3D && !pBindingReflection->image.arrayed)
                        descriptorBoundResources->storageViewTypeMap.emplace(pBindingReflection->name, VK_IMAGE_VIEW_TYPE_3D);
                    // VK_IMAGE_VIEW_TYPE_3D_ARRAY does not exist in HLSL.
                    if (pBindingReflection->image.dim == SpvDimCube && !pBindingReflection->image.arrayed)
                        descriptorBoundResources->storageViewTypeMap.emplace(pBindingReflection->name, VK_IMAGE_VIEW_TYPE_CUBE);
                    if (pBindingReflection->image.dim == SpvDimCube && pBindingReflection->image.arrayed)
                        descriptorBoundResources->storageViewTypeMap.emplace(pBindingReflection->name, VK_IMAGE_VIEW_TYPE_CUBE_ARRAY);
                }

                // In case of uniform buffer create UniformBufferBlock:
                if (pBindingReflection->descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
                {
                    SpvReflectBlockVariable& blockReflection = pBindingReflection->block;
                    UniformBufferBlock* pUniformBufferBlock = GetUniformBufferBlock(blockReflection, pSetReflection->set, pBindingReflection->binding);
                    descriptorBoundResources->uniformBufferBlockMap.emplace(pUniformBufferBlock->name, pUniformBufferBlock);
                }
            }
        }
    }
    Uint3 SpirvReflect::GetBlockSize() const
    {
        const SpvReflectEntryPoint* entryPoint = spvReflectGetEntryPoint(&m_module, "main");
        if (entryPoint)
            return Uint3(entryPoint->local_size.x, entryPoint->local_size.y, entryPoint->local_size.z);
        else
        {
            LOG_WARN("Given shader does not contain the entry point 'main' or is not a compute shader!");
            return Uint3::zero;
        }
    }

    // Private methods:
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
    // -------------------------------------------------------------
}