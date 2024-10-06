#include "spirvReflect.h"
#include <iostream> // TODO: Remove this
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



// Constructor:
SpirvReflect::SpirvReflect(std::vector<char> code)
{
    SPVA(spvReflectCreateShaderModule(code.size(), code.data(), &module));
}



// Destructor:
SpirvReflect::~SpirvReflect()
{
    spvReflectDestroyShaderModule(&module);
}



// Public methods:
std::vector<SpvReflectDescriptorSet*> SpirvReflect::GetDescriptorSetsReflection() const
{
    uint32_t setCount = 0;
    SPVA(spvReflectEnumerateDescriptorSets(&module, &setCount, nullptr));
    std::vector<SpvReflectDescriptorSet*> sets(setCount);
    SPVA(spvReflectEnumerateDescriptorSets(&module, &setCount, sets.data()));
    //FixFragmentShaderDescriptorSetsReflection(sets);
    return sets;
}
//std::vector<SpvReflectInterfaceVariable*> SpirvReflect::GetInputVariablesReflection() const
//{
//    uint32_t inputCount = 0;
//    SPVA(spvReflectEnumerateInputVariables(&module, &inputCount, NULL));
//    std::vector<SpvReflectInterfaceVariable*> inputs(inputCount);
//    SPVA(spvReflectEnumerateInputVariables(&module, &inputCount, inputs.data()));
//    return inputs;
//}



// Debugging:
void SpirvReflect::PrintDescriptorSetsInfo() const
{
    std::vector<SpvReflectDescriptorSet*> sets = GetDescriptorSetsReflection();
    for (uint32_t i = 0; i < sets.size(); i++)
    {
		SpvReflectDescriptorSet* set = sets[i];
        std::cout << "set:           " << set->set << std::endl;
        std::cout << "binding_count: " << set->binding_count << std::endl;
        for (uint32_t j = 0; j < set->binding_count; j++)
        {
            SpvReflectDescriptorBinding* binding = set->bindings[j];
            std::cout << "\tspirv_id:        " << binding->spirv_id << std::endl;
            std::cout << "\tname:            " << binding->name << std::endl;
            std::cout << "\tbinding:         " << binding->binding << std::endl;
            std::cout << "\tdescriptor_type: " << (int)binding->descriptor_type << " = " << GetSpvReflectDescriptorTypeName(binding->descriptor_type) << std::endl;
            std::cout << "\tdescriptorCount: " << binding->count << std::endl;
            std::cout << "\tinput_attachment_index:" << binding->input_attachment_index << std::endl;
            std::cout << "\tset:" << binding->set << std::endl;
            std::cout << std::endl;
        }
        std::cout << std::endl;
        std::cout << std::endl;
    }
}
//void SpirvReflect::PrintInputVariablesInfo() const
//{
//    std::vector<SpvReflectInterfaceVariable*> inputs = GetInputVariablesReflection();
//    for (uint32_t i = 0; i < inputs.size(); i++)
//    {
//        SpvReflectInterfaceVariable* input = inputs[i];
//        std::cout << "id:                    " << input->spirv_id << std::endl;
//        std::cout << "name:                  " << input->name << std::endl;
//        std::cout << "location:              " << input->location << std::endl;
//        std::cout << "conmponent:            " << input->location << std::endl;
//        std::cout << "storage_class value:   " << (int)input->storage_class << std::endl;
//        std::cout << "storage_class name:    " << GetSpvStorageClassName(input->storage_class) << std::endl;
//        //std::cout << "semantic:              " << input->semantic << std::endl;
//        std::cout << "decoration_flags:      " << input->decoration_flags << std::endl;
//        std::cout << "built_in value:        " << (int)input->built_in << std::endl;
//        std::cout << "built_in name :        " << GetSpvBuiltInName(input->built_in) << std::endl;
//        std::cout << "numeric.sca.width:     " << input->numeric.scalar.width << std::endl;
//        std::cout << "numeric.sca.sign:      " << input->numeric.scalar.signedness << std::endl;
//        std::cout << "numeric.vec.count:     " << input->numeric.vector.component_count << std::endl;
//        std::cout << "numeric.mat.count:     " << input->numeric.matrix.column_count << std::endl;
//        std::cout << "numeric.mat.rows:      " << input->numeric.matrix.row_count << std::endl;
//        std::cout << "numeric.mat.sstride:   " << input->numeric.matrix.stride << std::endl;
//        std::cout << "array.dims_count:      " << input->array.dims_count << std::endl;
//        std::cout << "array.stride:          " << input->array.stride << std::endl;
//        for (uint32_t i = 0; i < input->array.dims_count; i++)
//        {
//            std::cout << "array.dims[" << i << "]: " << input->array.dims[i] << std::endl;
//            std::cout << "array.spec_const_op_id[" << i << "]: " << input->array.spec_constant_op_ids[i] << std::endl;
//        }
//        std::cout << "array.member_count:    " << input->member_count << std::endl;
//        std::cout << "array.word_offset.loc: " << input->word_offset.location << std::endl;
//        std::cout << std::endl;
//    }
//}



// Private static methods:
std::string SpirvReflect::GetSpvReflectDescriptorTypeName(SpvReflectDescriptorType spvReflectDescriptorType)
{
    switch (spvReflectDescriptorType)
    {
        case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLER: return "VK_DESCRIPTOR_TYPE_SAMPLER";
        case SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER: return "VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER";
        case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE: return "VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE";
        case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_IMAGE: return "VK_DESCRIPTOR_TYPE_STORAGE_IMAGE";
        case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER: return "VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER";
        case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER: return "VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER";
        case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER: return "VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER";
        case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER: return "VK_DESCRIPTOR_TYPE_STORAGE_BUFFER";
        case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC: return "VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC";
        case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC: return "VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC";
        case SPV_REFLECT_DESCRIPTOR_TYPE_INPUT_ATTACHMENT: return "VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT";
        case SPV_REFLECT_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR: return "VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR";
        default: return "Unknown SpvReflectDescriptorType";
    }
}
std::string SpirvReflect::GetSpvStorageClassName(SpvStorageClass spvStorageClass)
{
    switch (spvStorageClass)
    {
        case SpvStorageClassUniformConstant: return "SpvStorageClassUniformConstant";
        case SpvStorageClassInput: return "SpvStorageClassInput";
        case SpvStorageClassUniform: return "SpvStorageClassUniform";
        case SpvStorageClassOutput: return "SpvStorageClassOutput";
        case SpvStorageClassWorkgroup: return "SpvStorageClassWorkgroup";
        case SpvStorageClassCrossWorkgroup: return "SpvStorageClassCrossWorkgroup";
        case SpvStorageClassPrivate: return "SpvStorageClassPrivate";
        case SpvStorageClassFunction: return "SpvStorageClassFunction";
        case SpvStorageClassGeneric: return "SpvStorageClassGeneric";
        case SpvStorageClassPushConstant: return "SpvStorageClassPushConstant";
        case SpvStorageClassAtomicCounter: return "SpvStorageClassAtomicCounter";
        case SpvStorageClassImage: return "SpvStorageClassImage";
        case SpvStorageClassStorageBuffer: return "SpvStorageClassStorageBuffer";
        case SpvStorageClassTileImageEXT: return "SpvStorageClassTileImageEXT";
        case SpvStorageClassCallableDataKHR: return "SpvStorageClassCallableDataKHR |SpvStorageClassCallableDataNV";
        case SpvStorageClassIncomingCallableDataKHR: return "SpvStorageClassIncomingCallableDataKHR | SpvStorageClassIncomingCallableDataNV";
        case SpvStorageClassRayPayloadKHR: return "SpvStorageClassRayPayloadKHR | SpvStorageClassRayPayloadNV";
        case SpvStorageClassHitAttributeKHR: return "SpvStorageClassHitAttributeKHR | SpvStorageClassHitAttributeNV";
        case SpvStorageClassIncomingRayPayloadKHR: return "SpvStorageClassIncomingRayPayloadKHR | SpvStorageClassIncomingRayPayloadNV";
        case SpvStorageClassShaderRecordBufferKHR: return "SpvStorageClassShaderRecordBufferKHR | SpvStorageClassShaderRecordBufferNV";
        case SpvStorageClassPhysicalStorageBuffer: return "SpvStorageClassPhysicalStorageBuffer | SpvStorageClassPhysicalStorageBufferEXT";
        case SpvStorageClassHitObjectAttributeNV: return "SpvStorageClassHitObjectAttributeNV";
        case SpvStorageClassTaskPayloadWorkgroupEXT: return "SpvStorageClassTaskPayloadWorkgroupEXT";
        case SpvStorageClassCodeSectionINTEL: return "SpvStorageClassCodeSectionINTEL";
        case SpvStorageClassDeviceOnlyINTEL: return "SpvStorageClassDeviceOnlyINTEL";
        case SpvStorageClassHostOnlyINTEL: return "SpvStorageClassHostOnlyINTEL";
        case SpvStorageClassMax: return "SpvStorageClassMax";
        default: return "Unknown SpvStorageClass";
    }
}
std::string SpirvReflect::GetSpvBuiltInName(SpvBuiltIn spvBuiltIn)
{
    switch (spvBuiltIn)
    {
        case SpvBuiltInPosition: return "SpvBuiltInPosition";
        case SpvBuiltInPointSize: return "SpvBuiltInPointSize";
        case SpvBuiltInClipDistance: return "SpvBuiltInClipDistance";
        case SpvBuiltInCullDistance: return "SpvBuiltInCullDistance";
        case SpvBuiltInVertexId: return "SpvBuiltInVertexId";
        case SpvBuiltInInstanceId: return "SpvBuiltInInstanceId";
        case SpvBuiltInPrimitiveId: return "SpvBuiltInPrimitiveId";
        case SpvBuiltInInvocationId: return "SpvBuiltInInvocationId";
        case SpvBuiltInLayer: return "SpvBuiltInLayer";
        case SpvBuiltInViewportIndex: return "SpvBuiltInViewportIndex";
        case SpvBuiltInTessLevelOuter: return "SpvBuiltInTessLevelOuter";
        case SpvBuiltInTessLevelInner: return "SpvBuiltInTessLevelInner";
        case SpvBuiltInTessCoord: return "SpvBuiltInTessCoord";
        case SpvBuiltInPatchVertices: return "SpvBuiltInPatchVertices";
        case SpvBuiltInFragCoord: return "SpvBuiltInFragCoord";
        case SpvBuiltInPointCoord: return "SpvBuiltInPointCoord";
        case SpvBuiltInFrontFacing: return "SpvBuiltInFrontFacing";
        case SpvBuiltInSampleId: return "SpvBuiltInSampleId";
        case SpvBuiltInSamplePosition: return "SpvBuiltInSamplePosition";
        case SpvBuiltInSampleMask: return "SpvBuiltInSampleMask";
        case SpvBuiltInFragDepth: return "SpvBuiltInFragDepth";
        case SpvBuiltInHelperInvocation: return "SpvBuiltInHelperInvocation";
        case SpvBuiltInNumWorkgroups: return "SpvBuiltInNumWorkgroups";
        case SpvBuiltInWorkgroupSize: return "SpvBuiltInWorkgroupSize";
        case SpvBuiltInWorkgroupId: return "SpvBuiltInWorkgroupId";
        case SpvBuiltInLocalInvocationId: return "SpvBuiltInLocalInvocationId";
        case SpvBuiltInGlobalInvocationId: return "SpvBuiltInGlobalInvocationId";
        case SpvBuiltInLocalInvocationIndex: return "SpvBuiltInLocalInvocationIndex";
        case SpvBuiltInWorkDim: return "SpvBuiltInWorkDim";
        case SpvBuiltInGlobalSize: return "SpvBuiltInGlobalSize";
        case SpvBuiltInEnqueuedWorkgroupSize: return "SpvBuiltInEnqueuedWorkgroupSize";
        case SpvBuiltInGlobalOffset: return "SpvBuiltInGlobalOffset";
        case SpvBuiltInGlobalLinearId: return "SpvBuiltInGlobalLinearId";
        case SpvBuiltInSubgroupSize: return "SpvBuiltInSubgroupSize";
        case SpvBuiltInSubgroupMaxSize: return "SpvBuiltInSubgroupMaxSize";
        case SpvBuiltInNumSubgroups: return "SpvBuiltInNumSubgroups";
        case SpvBuiltInNumEnqueuedSubgroups: return "SpvBuiltInNumEnqueuedSubgroups";
        case SpvBuiltInSubgroupId: return "SpvBuiltInSubgroupId";
        case SpvBuiltInSubgroupLocalInvocationId: return "SpvBuiltInSubgroupLocalInvocationId";
        case SpvBuiltInVertexIndex: return "SpvBuiltInVertexIndex";
        case SpvBuiltInInstanceIndex: return "SpvBuiltInInstanceIndex";
        case SpvBuiltInCoreIDARM: return "SpvBuiltInCoreIDARM";
        case SpvBuiltInCoreCountARM: return "SpvBuiltInCoreCountARM";
        case SpvBuiltInCoreMaxIDARM: return "SpvBuiltInCoreMaxIDARM";
        case SpvBuiltInWarpIDARM: return "SpvBuiltInWarpIDARM";
        case SpvBuiltInWarpMaxIDARM: return "SpvBuiltInWarpMaxIDARM";
        case SpvBuiltInSubgroupEqMask: return "SpvBuiltInSubgroupEqMask | SpvBuiltInSubgroupEqMaskKHR";
        case SpvBuiltInSubgroupGeMask: return "SpvBuiltInSubgroupGeMask | SpvBuiltInSubgroupGeMaskKHR";
        case SpvBuiltInSubgroupGtMask: return "SpvBuiltInSubgroupGtMask | SpvBuiltInSubgroupGtMaskKHR";
        case SpvBuiltInSubgroupLeMask: return "SpvBuiltInSubgroupLeMask | SpvBuiltInSubgroupLeMaskKHR";
        case SpvBuiltInSubgroupLtMask: return "SpvBuiltInSubgroupLtMask | SpvBuiltInSubgroupLtMaskKHR";
        case SpvBuiltInBaseVertex: return "SpvBuiltInBaseVertex";
        case SpvBuiltInBaseInstance: return "SpvBuiltInBaseInstance";
        case SpvBuiltInDrawIndex: return "SpvBuiltInDrawIndex";
        case SpvBuiltInPrimitiveShadingRateKHR: return "SpvBuiltInPrimitiveShadingRateKHR";
        case SpvBuiltInDeviceIndex: return "SpvBuiltInDeviceIndex";
        case SpvBuiltInViewIndex: return "SpvBuiltInViewIndex";
        case SpvBuiltInShadingRateKHR: return "SpvBuiltInShadingRateKHR";
        case SpvBuiltInBaryCoordNoPerspAMD: return "SpvBuiltInBaryCoordNoPerspAMD";
        case SpvBuiltInBaryCoordNoPerspCentroidAMD: return "SpvBuiltInBaryCoordNoPerspCentroidAMD";
        case SpvBuiltInBaryCoordNoPerspSampleAMD: return "SpvBuiltInBaryCoordNoPerspSampleAMD";
        case SpvBuiltInBaryCoordSmoothAMD: return "SpvBuiltInBaryCoordSmoothAMD";
        case SpvBuiltInBaryCoordSmoothCentroidAMD: return "SpvBuiltInBaryCoordSmoothCentroidAMD";
        case SpvBuiltInBaryCoordSmoothSampleAMD: return "SpvBuiltInBaryCoordSmoothSampleAMD";
        case SpvBuiltInBaryCoordPullModelAMD: return "SpvBuiltInBaryCoordPullModelAMD";
        case SpvBuiltInFragStencilRefEXT: return "SpvBuiltInFragStencilRefEXT";
        case SpvBuiltInViewportMaskNV: return "SpvBuiltInViewportMaskNV";
        case SpvBuiltInSecondaryPositionNV: return "SpvBuiltInSecondaryPositionNV";
        case SpvBuiltInSecondaryViewportMaskNV: return "SpvBuiltInSecondaryViewportMaskNV";
        case SpvBuiltInPositionPerViewNV: return "SpvBuiltInPositionPerViewNV";
        case SpvBuiltInViewportMaskPerViewNV: return "SpvBuiltInViewportMaskPerViewNV";
        case SpvBuiltInFullyCoveredEXT: return "SpvBuiltInFullyCoveredEXT";
        case SpvBuiltInTaskCountNV: return "SpvBuiltInTaskCountNV";
        case SpvBuiltInPrimitiveCountNV: return "SpvBuiltInPrimitiveCountNV";
        case SpvBuiltInPrimitiveIndicesNV: return "SpvBuiltInPrimitiveIndicesNV";
        case SpvBuiltInClipDistancePerViewNV: return "SpvBuiltInClipDistancePerViewNV";
        case SpvBuiltInCullDistancePerViewNV: return "SpvBuiltInCullDistancePerViewNV";
        case SpvBuiltInLayerPerViewNV: return "SpvBuiltInLayerPerViewNV";
        case SpvBuiltInMeshViewCountNV: return "SpvBuiltInMeshViewCountNV";
        case SpvBuiltInMeshViewIndicesNV: return "SpvBuiltInMeshViewIndicesNV";
        case SpvBuiltInBaryCoordKHR: return "SpvBuiltInBaryCoordKHR | SpvBuiltInBaryCoordNV";
        case SpvBuiltInBaryCoordNoPerspKHR: return "SpvBuiltInBaryCoordNoPerspKHR | SpvBuiltInBaryCoordNoPerspNV";
        case SpvBuiltInFragSizeEXT: return "SpvBuiltInFragSizeEXT | SpvBuiltInFragmentSizeNV";
        case SpvBuiltInFragInvocationCountEXT: return "SpvBuiltInFragInvocationCountEXT | SpvBuiltInInvocationsPerPixelNV";
        case SpvBuiltInPrimitivePointIndicesEXT: return "SpvBuiltInPrimitivePointIndicesEXT";
        case SpvBuiltInPrimitiveLineIndicesEXT: return "SpvBuiltInPrimitiveLineIndicesEXT";
        case SpvBuiltInPrimitiveTriangleIndicesEXT: return "SpvBuiltInPrimitiveTriangleIndicesEXT";
        case SpvBuiltInCullPrimitiveEXT: return "SpvBuiltInCullPrimitiveEXT";
        case SpvBuiltInLaunchIdKHR: return "SpvBuiltInLaunchIdKHR | SpvBuiltInLaunchIdNV";
        case SpvBuiltInLaunchSizeKHR: return "SpvBuiltInLaunchSizeKHR | SpvBuiltInLaunchSizeNV";
        case SpvBuiltInWorldRayOriginKHR: return "SpvBuiltInWorldRayOriginKHR | SpvBuiltInWorldRayOriginNV";
        case SpvBuiltInWorldRayDirectionKHR: return "SpvBuiltInWorldRayDirectionKHR | SpvBuiltInWorldRayDirectionNV";
        case SpvBuiltInObjectRayOriginKHR: return "SpvBuiltInObjectRayOriginKHR | SpvBuiltInObjectRayOriginNV";
        case SpvBuiltInObjectRayDirectionKHR: return "SpvBuiltInObjectRayDirectionKHR | SpvBuiltInObjectRayDirectionNV";
        case SpvBuiltInRayTminKHR: return "SpvBuiltInRayTminKHR | SpvBuiltInRayTminNV";
        case SpvBuiltInRayTmaxKHR: return "SpvBuiltInRayTmaxKHR | SpvBuiltInRayTmaxNV";
        case SpvBuiltInInstanceCustomIndexKHR: return "SpvBuiltInInstanceCustomIndexKHR | SpvBuiltInInstanceCustomIndexNV";
        case SpvBuiltInObjectToWorldKHR: return "SpvBuiltInObjectToWorldKHR | SpvBuiltInObjectToWorldNV";
        case SpvBuiltInWorldToObjectKHR: return "SpvBuiltInWorldToObjectKHR | SpvBuiltInWorldToObjectNV";
        case SpvBuiltInHitTNV: return "SpvBuiltInHitTNV";
        case SpvBuiltInHitKindKHR: return "SpvBuiltInHitKindKHR | SpvBuiltInHitKindNV";
        case SpvBuiltInCurrentRayTimeNV: return "SpvBuiltInCurrentRayTimeNV";
        case SpvBuiltInHitTriangleVertexPositionsKHR: return "SpvBuiltInHitTriangleVertexPositionsKHR";
        case SpvBuiltInIncomingRayFlagsKHR: return "SpvBuiltInIncomingRayFlagsKHR | SpvBuiltInIncomingRayFlagsNV";
        case SpvBuiltInRayGeometryIndexKHR: return "SpvBuiltInRayGeometryIndexKHR";
        case SpvBuiltInWarpsPerSMNV: return "SpvBuiltInWarpsPerSMNV";
        case SpvBuiltInSMCountNV: return "SpvBuiltInSMCountNV";
        case SpvBuiltInWarpIDNV: return "SpvBuiltInWarpIDNV";
        case SpvBuiltInSMIDNV: return "SpvBuiltInSMIDNV";
        case SpvBuiltInCullMaskKHR: return "SpvBuiltInCullMaskKHR";
        case SpvBuiltInMax: return "SpvBuiltInMax";
        default: return "Unknown SpvBuildIn";
    }
}