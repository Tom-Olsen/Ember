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



// UniformBufferMember pubic methods:
std::string UniformBufferMember::ToString() const
{
    return "Name: " + name + ", Offset: " + std::to_string(offset) + ", Size: " + std::to_string(size) + ", Type: " + type;
}



// UniformBufferBlock Constructor/Destructor:
UniformBufferBlock::UniformBufferBlock(const std::string& name, uint32_t setIndex, uint32_t bindingIndex)
{
	this->name = name;
	this->setIndex = setIndex;
	this->bindingIndex = bindingIndex;
    this->size = 0;
}
UniformBufferBlock::~UniformBufferBlock()
{
	for (auto& member : members)
		delete member.second;
	LOG_INFO("UniformBufferBlock '{}' destroyed.", name);
}



// UniformBufferBlock public methods:
void UniformBufferBlock::AddMember(UniformBufferMember* member)
{
    // Take ownership of UniformBufferMember pointer:
    members.emplace(member->name, member);

	// Adjust size:
    uint32_t newSize = member->offset + member->size;
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
    std::string output = "Uniform Buffer Block(binding=" + std::to_string(bindingIndex) + "):\n";
    for (const auto& member : members)
        output += "  " + member.second->ToString() + "\n";
    return output;
}



// SpirvReflect Constructor/Destructor:
SpirvReflect::SpirvReflect(std::vector<char> code)
{
    SPVA(spvReflectCreateShaderModule(code.size(), code.data(), &module));
}
SpirvReflect::~SpirvReflect()
{
    spvReflectDestroyShaderModule(&module);
}



// SpirvReflect public methods:
std::vector<SpvReflectDescriptorSet*> SpirvReflect::GetDescriptorSetsReflection() const
{
    uint32_t setCount = 0;
    SPVA(spvReflectEnumerateDescriptorSets(&module, &setCount, nullptr));
    std::vector<SpvReflectDescriptorSet*> sets(setCount);
    SPVA(spvReflectEnumerateDescriptorSets(&module, &setCount, sets.data()));
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
UniformBufferBlock* SpirvReflect::GetUniformBufferBlock(const SpvReflectBlockVariable& blockReflection, uint32_t setIndex, uint32_t bindingIndex) const
{
    UniformBufferBlock* uniformBufferBlock = new UniformBufferBlock(blockReflection.name, setIndex, bindingIndex);

    for (uint32_t memberIndex = 0; memberIndex < blockReflection.member_count; memberIndex++)
    {
		// Create UniformBufferMember:
        SpvReflectBlockVariable& memberReflection = blockReflection.members[memberIndex];
        UniformBufferMember* uniformBufferMember = new UniformBufferMember();
        uniformBufferMember->name = memberReflection.name;
        uniformBufferMember->type = GetMemberType(memberReflection.type_description);
        uniformBufferMember->offset = memberReflection.offset;
        uniformBufferMember->size = memberReflection.size;

        // Array details (not supported yet):
        //if (member.array.dims_count > 0)
        //{
        //    for (uint32_t dimIndex = 0; dimIndex < member.array.dims_count; dimIndex++)
        //        std::cout << "      Dim[" << dimIndex << "]: " << member.array.dims[dimIndex] << std::endl;
        //}

        uniformBufferBlock->AddMember(uniformBufferMember);
    }

    return uniformBufferBlock;
}



// Debugging:
void SpirvReflect::PrintDescriptorSetsInfo() const
{
	std::string output = "Descriptor Sets:\n";
    std::vector<SpvReflectDescriptorSet*> sets = GetDescriptorSetsReflection();
    for (uint32_t setIndex = 0; setIndex < sets.size(); setIndex++)
    {
		SpvReflectDescriptorSet* set = sets[setIndex];
		output += "set:           " + std::to_string(set->set) + "\n";
		output += "binding_count: " + std::to_string(set->binding_count) + "\n";
        for (uint32_t bindingIndex = 0; bindingIndex < set->binding_count; bindingIndex++)
        {
            SpvReflectDescriptorBinding* binding = set->bindings[bindingIndex];
			output += "  spirv_id:        " + std::to_string(binding->spirv_id) + "\n";
			output += "  name:            " + std::string(binding->name) + "\n";
			output += "  binding:         " + std::to_string(binding->binding) + "\n";
			output += "  descriptor_type: " + std::to_string((int)binding->descriptor_type) + " = " + GetSpvReflectDescriptorTypeName(binding->descriptor_type) + "\n";
			output += "  descriptorCount: " + std::to_string(binding->count) + "\n";
			output += "  input_attachment_index:" + std::to_string(binding->input_attachment_index) + "\n";
			output += "  set:" + std::to_string(binding->set) + "\n";

			// If the binding is a uniform buffer, print the layout:
            if (binding->descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
            {
				output += "  Uniform Buffer Layout:\n";
                SpvReflectBlockVariable& block = binding->block;
                for (uint32_t memberIndex = 0; memberIndex < block.member_count; memberIndex++)
                {
                    SpvReflectBlockVariable& member = block.members[memberIndex];
                    std::string type = GetMemberType(member.type_description);
					output += "    Name: " + std::string(member.name) + ", Offset: " + std::to_string(member.offset) + ", Size: " + std::to_string(member.size) + ", Type: " + type + "\n";

                    // If the member is an array, print array details
                    if (member.array.dims_count > 0)
                    {
						output += "    Array Dimensions:\n";
                        for (uint32_t dimIndex = 0; dimIndex < member.array.dims_count; dimIndex++)
							output += "      Dim[" + std::to_string(dimIndex) + "]: " + std::to_string(member.array.dims[dimIndex]) + "\n";
                    }

                    // Nested struct extension not supported yet.
                }
            }
			output += "\n";
        }
		output += "\n\n";
    }
	LOG_TRACE(output);
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



// SpirvReflect private static methods:
std::string SpirvReflect::GetMemberType(SpvReflectTypeDescription* typeReflection) const
{
	std::string type = "unknown";

    if (typeReflection == nullptr)
        return type;

    switch (typeReflection->op)
    {
    case SpvOpTypeInt:
        type = "int";
        break;
    case SpvOpTypeFloat:
        type = "float";
        break;
    case SpvOpTypeVector:
        if (typeReflection->type_flags & SPV_REFLECT_TYPE_FLAG_INT)
            type = "int" + std::to_string(typeReflection->traits.numeric.vector.component_count);
        else if (typeReflection->type_flags & SPV_REFLECT_TYPE_FLAG_FLOAT)
            type = "float" + std::to_string(typeReflection->traits.numeric.vector.component_count);
        break;
    case SpvOpTypeMatrix:
        if (typeReflection->type_flags & SPV_REFLECT_TYPE_FLAG_INT)
            type = "int" + std::to_string(typeReflection->traits.numeric.matrix.row_count) + "x" + std::to_string(typeReflection->traits.numeric.matrix.column_count);
        else if (typeReflection->type_flags & SPV_REFLECT_TYPE_FLAG_FLOAT)
            type = "float" + std::to_string(typeReflection->traits.numeric.matrix.row_count) + "x" + std::to_string(typeReflection->traits.numeric.matrix.column_count);
        break;
    case SpvOpTypeStruct:
        type = "struct";
        break;
    default:
        type = "unknown";
        break;
    }

    return type;
}
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