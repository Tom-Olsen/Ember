#include "shaderReflection.h"
#include "descriptorBoundResources.h"
#include "logger.h"
#include "spirvReflectToString.h"
//#include "common/output_stream.h" // ToDo: check the output functons in here.
#include <spirv_reflect.h>



// Macros:
#define SPIRV_REFLECT_ASSERT(val) \
if (val != SPV_REFLECT_RESULT_SUCCESS) { \
LOG_CRITICAL("File: {}, Line: {} SPIRV-Reflect error: {}", __FILE__, __LINE__, std::to_string(val)); std::abort(); }
#ifndef SPVA
    #define SPVA(f) SPIRV_REFLECT_ASSERT(f)
#endif



namespace emberSpirvReflect
{
    // Constructor/Destructor:
    ShaderReflection::ShaderReflection(const std::vector<char>& code)
    {
        m_pModule = std::make_unique<SpvReflectShaderModule>();
        SPVA(spvReflectCreateShaderModule(code.size(), code.data(), m_pModule.get()));
    }
    ShaderReflection::~ShaderReflection()
    {
        if (m_pModule)
            spvReflectDestroyShaderModule(m_pModule.get());
    }



    // Public methods:
    Uint3 ShaderReflection::GetBlockSize() const
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

    std::vector<VertexInputDescription> ShaderReflection::GetVertexInputDescriptions() const
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
            //LOG_TRACE(ToString(pInput));    // debug output.
            VertexInputDescription vertexInputDescription = VertexInputDescription(pInput);
            if (vertexInputDescription.isValid) // skips array and build in types.
            {
                //LOG_INFO(vertexInputDescription.ToString());    // debug output.
                vertexInputDescriptions.push_back(vertexInputDescription);
            }
        }

        vertexInputDescriptions.shrink_to_fit();
        return vertexInputDescriptions;
    }

    void ShaderReflection::AddDescriptorBoundResources(DescriptorBoundResources* const descriptorBoundResources) const
    {
        // Get descriptor set reflection:
        uint32_t setCount = 0;
        SPVA(spvReflectEnumerateDescriptorSets(m_pModule.get(), &setCount, nullptr));
        std::vector<SpvReflectDescriptorSet*> descriptors(setCount);
        SPVA(spvReflectEnumerateDescriptorSets(m_pModule.get(), &setCount, descriptors.data()));
    
        // First print all the informationsimilat to GetVertexInputDescriptions.

        for (uint32_t setIndex = 0; setIndex < descriptors.size(); setIndex++)
        {
            LOG_TRACE(ToString(descriptors[setIndex]));
            //SpvReflectDescriptorSet* pSetReflection = descriptors[setIndex];
            //descriptorBoundResources->bindingCount += pSetReflection->binding_count;
            //for (uint32_t bindingIndex = 0; bindingIndex < pSetReflection->binding_count; bindingIndex++)
            //{
            //    SpvReflectDescriptorBinding* pBindingReflection = pSetReflection->bindings[bindingIndex];
            //    DescriptorSetLayoutBinding layoutBinding = {};
            //    layoutBinding.binding = pBindingReflection->binding;
            //    layoutBinding.descriptorType = DescriptorType((int)pBindingReflection->descriptor_type);
            //    layoutBinding.descriptorCount = pBindingReflection->count;
            //    layoutBinding.shaderStage = ShaderStage((int)m_module.shader_stage);
            //    layoutBinding.pImmutableSamplers = nullptr;
            //
            //    // Add binding and name to lists:
            //    descriptorBoundResources->descriptorSetBindingNames.push_back(pBindingReflection->name);
            //    descriptorBoundResources->descriptorSetLayoutBindings.push_back(layoutBinding);
            //
            //    // In case sample texture retrieve view type:
            //    if (pBindingReflection->descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE)
            //    {
            //        if (pBindingReflection->image.dim == SpvDim1D && !pBindingReflection->image.arrayed)
            //            descriptorBoundResources->sampleViewTypeMap.emplace(pBindingReflection->name, VK_IMAGE_VIEW_TYPE_1D);
            //        else if (pBindingReflection->image.dim == SpvDim1D && pBindingReflection->image.arrayed)
            //            descriptorBoundResources->sampleViewTypeMap.emplace(pBindingReflection->name, VK_IMAGE_VIEW_TYPE_1D_ARRAY);
            //        if (pBindingReflection->image.dim == SpvDim2D && !pBindingReflection->image.arrayed)
            //            descriptorBoundResources->sampleViewTypeMap.emplace(pBindingReflection->name, VK_IMAGE_VIEW_TYPE_2D);
            //        else if (pBindingReflection->image.dim == SpvDim2D && pBindingReflection->image.arrayed)
            //            descriptorBoundResources->sampleViewTypeMap.emplace(pBindingReflection->name, VK_IMAGE_VIEW_TYPE_2D_ARRAY);
            //        if (pBindingReflection->image.dim == SpvDim3D && !pBindingReflection->image.arrayed)
            //            descriptorBoundResources->sampleViewTypeMap.emplace(pBindingReflection->name, VK_IMAGE_VIEW_TYPE_3D);
            //        // VK_IMAGE_VIEW_TYPE_3D_ARRAY does not exist in HLSL.
            //        if (pBindingReflection->image.dim == SpvDimCube && !pBindingReflection->image.arrayed)
            //            descriptorBoundResources->sampleViewTypeMap.emplace(pBindingReflection->name, VK_IMAGE_VIEW_TYPE_CUBE);
            //        if (pBindingReflection->image.dim == SpvDimCube && pBindingReflection->image.arrayed)
            //            descriptorBoundResources->sampleViewTypeMap.emplace(pBindingReflection->name, VK_IMAGE_VIEW_TYPE_CUBE_ARRAY);
            //    }
            //    // In case storage texture retrieve view type:
            //    if (pBindingReflection->descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_IMAGE)
            //    {
            //        if (pBindingReflection->image.dim == SpvDim1D && !pBindingReflection->image.arrayed)
            //            descriptorBoundResources->storageViewTypeMap.emplace(pBindingReflection->name, VK_IMAGE_VIEW_TYPE_1D);
            //        else if (pBindingReflection->image.dim == SpvDim1D && pBindingReflection->image.arrayed)
            //            descriptorBoundResources->storageViewTypeMap.emplace(pBindingReflection->name, VK_IMAGE_VIEW_TYPE_1D_ARRAY);
            //        if (pBindingReflection->image.dim == SpvDim2D && !pBindingReflection->image.arrayed)
            //            descriptorBoundResources->storageViewTypeMap.emplace(pBindingReflection->name, VK_IMAGE_VIEW_TYPE_2D);
            //        else if (pBindingReflection->image.dim == SpvDim2D && pBindingReflection->image.arrayed)
            //            descriptorBoundResources->storageViewTypeMap.emplace(pBindingReflection->name, VK_IMAGE_VIEW_TYPE_2D_ARRAY);
            //        if (pBindingReflection->image.dim == SpvDim3D && !pBindingReflection->image.arrayed)
            //            descriptorBoundResources->storageViewTypeMap.emplace(pBindingReflection->name, VK_IMAGE_VIEW_TYPE_3D);
            //        // VK_IMAGE_VIEW_TYPE_3D_ARRAY does not exist in HLSL.
            //        if (pBindingReflection->image.dim == SpvDimCube && !pBindingReflection->image.arrayed)
            //            descriptorBoundResources->storageViewTypeMap.emplace(pBindingReflection->name, VK_IMAGE_VIEW_TYPE_CUBE);
            //        if (pBindingReflection->image.dim == SpvDimCube && pBindingReflection->image.arrayed)
            //            descriptorBoundResources->storageViewTypeMap.emplace(pBindingReflection->name, VK_IMAGE_VIEW_TYPE_CUBE_ARRAY);
            //    }
            //
            //    // In case of uniform buffer create UniformBufferBlock:
            //    if (pBindingReflection->descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
            //    {
            //        SpvReflectBlockVariable& blockReflection = pBindingReflection->block;
            //        UniformBufferBlock* pUniformBufferBlock = GetUniformBufferBlock(blockReflection, pSetReflection->set, pBindingReflection->binding);
            //        descriptorBoundResources->uniformBufferBlockMap.emplace(pUniformBufferBlock->name, std::unique_ptr<UniformBufferBlock>(pUniformBufferBlock));
            //    }
            //}
        }
    }
}