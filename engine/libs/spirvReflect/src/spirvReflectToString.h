#pragma once
#include "vulkanFormatToString.h"
#include <spirv_reflect.h>
#include <sstream>
#include <vulkan/vulkan.h>



// Enum to string conversions:
inline std::string ToString(const SpvDim& dim)
{
    switch (dim)
    {
        case SpvDim1D:                  return "SpvDim1D"; break;
        case SpvDim2D:                  return "SpvDim2D"; break;
        case SpvDim3D:                  return "SpvDim3D"; break;
        case SpvDimCube:                return "SpvDimCube"; break;
        case SpvDimRect:                return "SpvDimRect"; break;
        case SpvDimBuffer:              return "SpvDimBuffer"; break;
        case SpvDimSubpassData:         return "SpvDimSubpassData"; break;
        case SpvDimTileImageDataEXT:    return "SpvDimTileImageDataEXT"; break;
        case SpvDimMax:                 return "SpvDimMax"; break;
        default: return "unknown";
    }
}
inline std::string ToString(const SpvReflectFormat& format)
{
    switch (format)
    {
        case SPV_REFLECT_FORMAT_UNDEFINED:              return "VK_FORMAT_UNDEFINED"; break;
        case SPV_REFLECT_FORMAT_R16_UINT:               return "VK_FORMAT_R16_UINT"; break;
        case SPV_REFLECT_FORMAT_R16_SINT:               return "VK_FORMAT_R16_SINT"; break;
        case SPV_REFLECT_FORMAT_R16_SFLOAT:             return "VK_FORMAT_R16_SFLOAT"; break;
        case SPV_REFLECT_FORMAT_R16G16_UINT:            return "VK_FORMAT_R16G16_UINT"; break;
        case SPV_REFLECT_FORMAT_R16G16_SINT:            return "VK_FORMAT_R16G16_SINT"; break;
        case SPV_REFLECT_FORMAT_R16G16_SFLOAT:          return "VK_FORMAT_R16G16_SFLOAT"; break;
        case SPV_REFLECT_FORMAT_R16G16B16_UINT:         return "VK_FORMAT_R16G16B16_UINT"; break;
        case SPV_REFLECT_FORMAT_R16G16B16_SINT:         return "VK_FORMAT_R16G16B16_SINT"; break;
        case SPV_REFLECT_FORMAT_R16G16B16_SFLOAT:       return "VK_FORMAT_R16G16B16_SFLOAT"; break;
        case SPV_REFLECT_FORMAT_R16G16B16A16_UINT:      return "VK_FORMAT_R16G16B16A16_UINT"; break;
        case SPV_REFLECT_FORMAT_R16G16B16A16_SINT:      return "VK_FORMAT_R16G16B16A16_SINT"; break;
        case SPV_REFLECT_FORMAT_R16G16B16A16_SFLOAT:    return "VK_FORMAT_R16G16B16A16_SFLOAT"; break;
        case SPV_REFLECT_FORMAT_R32_UINT:               return "VK_FORMAT_R32_UINT"; break;
        case SPV_REFLECT_FORMAT_R32_SINT:               return "VK_FORMAT_R32_SINT"; break;
        case SPV_REFLECT_FORMAT_R32_SFLOAT:             return "VK_FORMAT_R32_SFLOAT"; break;
        case SPV_REFLECT_FORMAT_R32G32_UINT:            return "VK_FORMAT_R32G32_UINT"; break;
        case SPV_REFLECT_FORMAT_R32G32_SINT:            return "VK_FORMAT_R32G32_SINT"; break;
        case SPV_REFLECT_FORMAT_R32G32_SFLOAT:          return "VK_FORMAT_R32G32_SFLOAT"; break;
        case SPV_REFLECT_FORMAT_R32G32B32_UINT:         return "VK_FORMAT_R32G32B32_UINT"; break;
        case SPV_REFLECT_FORMAT_R32G32B32_SINT:         return "VK_FORMAT_R32G32B32_SINT"; break;
        case SPV_REFLECT_FORMAT_R32G32B32_SFLOAT:       return "VK_FORMAT_R32G32B32_SFLOAT"; break;
        case SPV_REFLECT_FORMAT_R32G32B32A32_UINT:      return "VK_FORMAT_R32G32B32A32_UINT"; break;
        case SPV_REFLECT_FORMAT_R32G32B32A32_SINT:      return "VK_FORMAT_R32G32B32A32_SINT"; break;
        case SPV_REFLECT_FORMAT_R32G32B32A32_SFLOAT:    return "VK_FORMAT_R32G32B32A32_SFLOAT"; break;
        case SPV_REFLECT_FORMAT_R64_UINT:               return "VK_FORMAT_R64_UINT"; break;
        case SPV_REFLECT_FORMAT_R64_SINT:               return "VK_FORMAT_R64_SINT"; break;
        case SPV_REFLECT_FORMAT_R64_SFLOAT:             return "VK_FORMAT_R64_SFLOAT"; break;
        case SPV_REFLECT_FORMAT_R64G64_UINT:            return "VK_FORMAT_R64G64_UINT"; break;
        case SPV_REFLECT_FORMAT_R64G64_SINT:            return "VK_FORMAT_R64G64_SINT"; break;
        case SPV_REFLECT_FORMAT_R64G64_SFLOAT:          return "VK_FORMAT_R64G64_SFLOAT"; break;
        case SPV_REFLECT_FORMAT_R64G64B64_UINT:         return "VK_FORMAT_R64G64B64_UINT"; break;
        case SPV_REFLECT_FORMAT_R64G64B64_SINT:         return "VK_FORMAT_R64G64B64_SINT"; break;
        case SPV_REFLECT_FORMAT_R64G64B64_SFLOAT:       return "VK_FORMAT_R64G64B64_SFLOAT"; break;
        case SPV_REFLECT_FORMAT_R64G64B64A64_UINT:      return "VK_FORMAT_R64G64B64A64_UINT"; break;
        case SPV_REFLECT_FORMAT_R64G64B64A64_SINT:      return "VK_FORMAT_R64G64B64A64_SINT"; break;
        case SPV_REFLECT_FORMAT_R64G64B64A64_SFLOAT:    return "VK_FORMAT_R64G64B64A64_SFLOAT"; break;
        default: return "unknown";
    }
}
inline std::string ToString(const SpvStorageClass& storage_class)
{
    switch (storage_class)
    {
        case SpvStorageClassUniformConstant:            return "SpvStorageClassUniformConstant"; break;
        case SpvStorageClassInput:                      return "SpvStorageClassInput"; break;
        case SpvStorageClassUniform:                    return "SpvStorageClassUniform"; break;
        case SpvStorageClassOutput:                     return "SpvStorageClassOutput"; break;
        case SpvStorageClassWorkgroup:                  return "SpvStorageClassWorkgroup"; break;
        case SpvStorageClassCrossWorkgroup:             return "SpvStorageClassCrossWorkgroup"; break;
        case SpvStorageClassPrivate:                    return "SpvStorageClassPrivate"; break;
        case SpvStorageClassFunction:                   return "SpvStorageClassFunction"; break;
        case SpvStorageClassGeneric:                    return "SpvStorageClassGeneric"; break;
        case SpvStorageClassPushConstant:               return "SpvStorageClassPushConstant"; break;
        case SpvStorageClassAtomicCounter:              return "SpvStorageClassAtomicCounter"; break;
        case SpvStorageClassImage:                      return "SpvStorageClassImage"; break;
        case SpvStorageClassStorageBuffer:              return "SpvStorageClassStorageBuffer"; break;
        case SpvStorageClassTileImageEXT:               return "SpvStorageClassTileImageEXT"; break;
        case SpvStorageClassTileAttachmentQCOM:         return "SpvStorageClassTileAttachmentQCOM"; break;
        case SpvStorageClassNodePayloadAMDX:            return "SpvStorageClassNodePayloadAMDX"; break;
        case SpvStorageClassCallableDataKHR:            return "SpvStorageClassCallableData(KHR/NV"; break;
        case SpvStorageClassIncomingCallableDataKHR:    return "SpvStorageClassIncomingCallableData(KHR/NV"; break;
        case SpvStorageClassRayPayloadKHR:              return "SpvStorageClassRayPayload(KHR/NV"; break;
        case SpvStorageClassHitAttributeKHR:            return "SpvStorageClassHitAttribute(KHR/NV"; break;
        case SpvStorageClassIncomingRayPayloadKHR:      return "SpvStorageClassIncomingRayPayload(KHR/NV"; break;
        case SpvStorageClassShaderRecordBufferKHR:      return "SpvStorageClassShaderRecordBuffer(KHR/NV"; break;
        case SpvStorageClassPhysicalStorageBuffer:      return "SpvStorageClassPhysicalStorageBuffer(EXT"; break;
        case SpvStorageClassHitObjectAttributeNV:       return "SpvStorageClassHitObjectAttributeNV"; break;
        case SpvStorageClassTaskPayloadWorkgroupEXT:    return "SpvStorageClassTaskPayloadWorkgroupEXT"; break;
        case SpvStorageClassCodeSectionINTEL:           return "SpvStorageClassCodeSectionINTEL"; break;
        case SpvStorageClassDeviceOnlyINTEL:            return "SpvStorageClassDeviceOnlyINTEL"; break;
        case SpvStorageClassHostOnlyINTEL:              return "SpvStorageClassHostOnlyINTEL"; break;
        case SpvStorageClassMax:                        return "SpvStorageClassMax"; break;
        default: return "unknown";
    }
}
inline std::string ToString(const SpvReflectResourceType& resource_type)
{
    switch (resource_type)
    {
        case SPV_REFLECT_RESOURCE_FLAG_UNDEFINED:   return "SPV_REFLECT_RESOURCE_FLAG_UNDEFINED"; break;
        case SPV_REFLECT_RESOURCE_FLAG_SAMPLER:     return "SPV_REFLECT_RESOURCE_FLAG_SAMPLER"; break;
        case SPV_REFLECT_RESOURCE_FLAG_CBV:         return "SPV_REFLECT_RESOURCE_FLAG_CBV"; break;
        case SPV_REFLECT_RESOURCE_FLAG_SRV:         return "SPV_REFLECT_RESOURCE_FLAG_SRV"; break;
        case SPV_REFLECT_RESOURCE_FLAG_UAV:         return "SPV_REFLECT_RESOURCE_FLAG_UAV"; break;
        default: return "unknown";
    }
}
inline std::string ToString(const SpvReflectDescriptorType& descriptor_type)
{
    switch (descriptor_type)
    {
        case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLER:                       return "VK_DESCRIPTOR_TYPE_SAMPLER"; break;
        case SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:        return "VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER"; break;
        case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE:                 return "VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE"; break;
        case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_IMAGE:                 return "VK_DESCRIPTOR_TYPE_STORAGE_IMAGE"; break;
        case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:          return "VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER"; break;
        case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:          return "VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER"; break;
        case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER:                return "VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER"; break;
        case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER:                return "VK_DESCRIPTOR_TYPE_STORAGE_BUFFER"; break;
        case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:        return "VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC"; break;
        case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:        return "VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC"; break;
        case SPV_REFLECT_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:              return "VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT"; break;
        case SPV_REFLECT_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR:    return "VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR"; break;
        default: return "unknown";
    }
}
inline std::string ToString(const SpvOp& op)
{
    switch (op)
    {
        case SpvOpNop:                                                                  return "SpvOpNop"; break;
        case SpvOpUndef:                                                                return "SpvOpUndef"; break;
        case SpvOpSourceContinued:                                                      return "SpvOpSourceContinued"; break;
        case SpvOpSource:                                                               return "SpvOpSource"; break;
        case SpvOpSourceExtension:                                                      return "SpvOpSourceExtension"; break;
        case SpvOpName:                                                                 return "SpvOpName"; break;
        case SpvOpMemberName:                                                           return "SpvOpMemberName"; break;
        case SpvOpString:                                                               return "SpvOpString"; break;
        case SpvOpLine:                                                                 return "SpvOpLine"; break;
        case SpvOpExtension:                                                            return "SpvOpExtension"; break;
        case SpvOpExtInstImport:                                                        return "SpvOpExtInstImport"; break;
        case SpvOpExtInst:                                                              return "SpvOpExtInst"; break;
        case SpvOpMemoryModel:                                                          return "SpvOpMemoryModel"; break;
        case SpvOpEntryPoint:                                                           return "SpvOpEntryPoint"; break;
        case SpvOpExecutionMode:                                                        return "SpvOpExecutionMode"; break;
        case SpvOpCapability:                                                           return "SpvOpCapability"; break;
        case SpvOpTypeVoid:                                                             return "SpvOpTypeVoid"; break;
        case SpvOpTypeBool:                                                             return "SpvOpTypeBool"; break;
        case SpvOpTypeInt:                                                              return "SpvOpTypeInt"; break;
        case SpvOpTypeFloat:                                                            return "SpvOpTypeFloat"; break;
        case SpvOpTypeVector:                                                           return "SpvOpTypeVector"; break;
        case SpvOpTypeMatrix:                                                           return "SpvOpTypeMatrix"; break;
        case SpvOpTypeImage:                                                            return "SpvOpTypeImage"; break;
        case SpvOpTypeSampler:                                                          return "SpvOpTypeSampler"; break;
        case SpvOpTypeSampledImage:                                                     return "SpvOpTypeSampledImage"; break;
        case SpvOpTypeArray:                                                            return "SpvOpTypeArray"; break;
        case SpvOpTypeRuntimeArray:                                                     return "SpvOpTypeRuntimeArray"; break;
        case SpvOpTypeStruct:                                                           return "SpvOpTypeStruct"; break;
        case SpvOpTypeOpaque:                                                           return "SpvOpTypeOpaque"; break;
        case SpvOpTypePointer:                                                          return "SpvOpTypePointer"; break;
        case SpvOpTypeFunction:                                                         return "SpvOpTypeFunction"; break;
        case SpvOpTypeEvent:                                                            return "SpvOpTypeEvent"; break;
        case SpvOpTypeDeviceEvent:                                                      return "SpvOpTypeDeviceEvent"; break;
        case SpvOpTypeReserveId:                                                        return "SpvOpTypeReserveId"; break;
        case SpvOpTypeQueue:                                                            return "SpvOpTypeQueue"; break;
        case SpvOpTypePipe:                                                             return "SpvOpTypePipe"; break;
        case SpvOpTypeForwardPointer:                                                   return "SpvOpTypeForwardPointer"; break;
        case SpvOpConstantTrue:                                                         return "SpvOpConstantTrue"; break;
        case SpvOpConstantFalse:                                                        return "SpvOpConstantFalse"; break;
        case SpvOpConstant:                                                             return "SpvOpConstant"; break;
        case SpvOpConstantComposite:                                                    return "SpvOpConstantComposite"; break;
        case SpvOpConstantSampler:                                                      return "SpvOpConstantSampler"; break;
        case SpvOpConstantNull:                                                         return "SpvOpConstantNull"; break;
        case SpvOpSpecConstantTrue:                                                     return "SpvOpSpecConstantTrue"; break;
        case SpvOpSpecConstantFalse:                                                    return "SpvOpSpecConstantFalse"; break;
        case SpvOpSpecConstant:                                                         return "SpvOpSpecConstant"; break;
        case SpvOpSpecConstantComposite:                                                return "SpvOpSpecConstantComposite"; break;
        case SpvOpSpecConstantOp:                                                       return "SpvOpSpecConstantOp"; break;
        case SpvOpFunction:                                                             return "SpvOpFunction"; break;
        case SpvOpFunctionParameter:                                                    return "SpvOpFunctionParameter"; break;
        case SpvOpFunctionEnd:                                                          return "SpvOpFunctionEnd"; break;
        case SpvOpFunctionCall:                                                         return "SpvOpFunctionCall"; break;
        case SpvOpVariable:                                                             return "SpvOpVariable"; break;
        case SpvOpImageTexelPointer:                                                    return "SpvOpImageTexelPointer"; break;
        case SpvOpLoad:                                                                 return "SpvOpLoad"; break;
        case SpvOpStore:                                                                return "SpvOpStore"; break;
        case SpvOpCopyMemory:                                                           return "SpvOpCopyMemory"; break;
        case SpvOpCopyMemorySized:                                                      return "SpvOpCopyMemorySized"; break;
        case SpvOpAccessChain:                                                          return "SpvOpAccessChain"; break;
        case SpvOpInBoundsAccessChain:                                                  return "SpvOpInBoundsAccessChain"; break;
        case SpvOpPtrAccessChain:                                                       return "SpvOpPtrAccessChain"; break;
        case SpvOpArrayLength:                                                          return "SpvOpArrayLength"; break;
        case SpvOpGenericPtrMemSemantics:                                               return "SpvOpGenericPtrMemSemantics"; break;
        case SpvOpInBoundsPtrAccessChain:                                               return "SpvOpInBoundsPtrAccessChain"; break;
        case SpvOpDecorate:                                                             return "SpvOpDecorate"; break;
        case SpvOpMemberDecorate:                                                       return "SpvOpMemberDecorate"; break;
        case SpvOpDecorationGroup:                                                      return "SpvOpDecorationGroup"; break;
        case SpvOpGroupDecorate:                                                        return "SpvOpGroupDecorate"; break;
        case SpvOpGroupMemberDecorate:                                                  return "SpvOpGroupMemberDecorate"; break;
        case SpvOpVectorExtractDynamic:                                                 return "SpvOpVectorExtractDynamic"; break;
        case SpvOpVectorInsertDynamic:                                                  return "SpvOpVectorInsertDynamic"; break;
        case SpvOpVectorShuffle:                                                        return "SpvOpVectorShuffle"; break;
        case SpvOpCompositeConstruct:                                                   return "SpvOpCompositeConstruct"; break;
        case SpvOpCompositeExtract:                                                     return "SpvOpCompositeExtract"; break;
        case SpvOpCompositeInsert:                                                      return "SpvOpCompositeInsert"; break;
        case SpvOpCopyObject:                                                           return "SpvOpCopyObject"; break;
        case SpvOpTranspose:                                                            return "SpvOpTranspose"; break;
        case SpvOpSampledImage:                                                         return "SpvOpSampledImage"; break;
        case SpvOpImageSampleImplicitLod:                                               return "SpvOpImageSampleImplicitLod"; break;
        case SpvOpImageSampleExplicitLod:                                               return "SpvOpImageSampleExplicitLod"; break;
        case SpvOpImageSampleDrefImplicitLod:                                           return "SpvOpImageSampleDrefImplicitLod"; break;
        case SpvOpImageSampleDrefExplicitLod:                                           return "SpvOpImageSampleDrefExplicitLod"; break;
        case SpvOpImageSampleProjImplicitLod:                                           return "SpvOpImageSampleProjImplicitLod"; break;
        case SpvOpImageSampleProjExplicitLod:                                           return "SpvOpImageSampleProjExplicitLod"; break;
        case SpvOpImageSampleProjDrefImplicitLod:                                       return "SpvOpImageSampleProjDrefImplicitLod"; break;
        case SpvOpImageSampleProjDrefExplicitLod:                                       return "SpvOpImageSampleProjDrefExplicitLod"; break;
        case SpvOpImageFetch:                                                           return "SpvOpImageFetch"; break;
        case SpvOpImageGather:                                                          return "SpvOpImageGather"; break;
        case SpvOpImageDrefGather:                                                      return "SpvOpImageDrefGather"; break;
        case SpvOpImageRead:                                                            return "SpvOpImageRead"; break;
        case SpvOpImageWrite:                                                           return "SpvOpImageWrite"; break;
        case SpvOpImage:                                                                return "SpvOpImage"; break;
        case SpvOpImageQueryFormat:                                                     return "SpvOpImageQueryFormat"; break;
        case SpvOpImageQueryOrder:                                                      return "SpvOpImageQueryOrder"; break;
        case SpvOpImageQuerySizeLod:                                                    return "SpvOpImageQuerySizeLod"; break;
        case SpvOpImageQuerySize:                                                       return "SpvOpImageQuerySize"; break;
        case SpvOpImageQueryLod:                                                        return "SpvOpImageQueryLod"; break;
        case SpvOpImageQueryLevels:                                                     return "SpvOpImageQueryLevels"; break;
        case SpvOpImageQuerySamples:                                                    return "SpvOpImageQuerySamples"; break;
        case SpvOpConvertFToU:                                                          return "SpvOpConvertFToU"; break;
        case SpvOpConvertFToS:                                                          return "SpvOpConvertFToS"; break;
        case SpvOpConvertSToF:                                                          return "SpvOpConvertSToF"; break;
        case SpvOpConvertUToF:                                                          return "SpvOpConvertUToF"; break;
        case SpvOpUConvert:                                                             return "SpvOpUConvert"; break;
        case SpvOpSConvert:                                                             return "SpvOpSConvert"; break;
        case SpvOpFConvert:                                                             return "SpvOpFConvert"; break;
        case SpvOpQuantizeToF16:                                                        return "SpvOpQuantizeToF16"; break;
        case SpvOpConvertPtrToU:                                                        return "SpvOpConvertPtrToU"; break;
        case SpvOpSatConvertSToU:                                                       return "SpvOpSatConvertSToU"; break;
        case SpvOpSatConvertUToS:                                                       return "SpvOpSatConvertUToS"; break;
        case SpvOpConvertUToPtr:                                                        return "SpvOpConvertUToPtr"; break;
        case SpvOpPtrCastToGeneric:                                                     return "SpvOpPtrCastToGeneric"; break;
        case SpvOpGenericCastToPtr:                                                     return "SpvOpGenericCastToPtr"; break;
        case SpvOpGenericCastToPtrExplicit:                                             return "SpvOpGenericCastToPtrExplicit"; break;
        case SpvOpBitcast:                                                              return "SpvOpBitcast"; break;
        case SpvOpSNegate:                                                              return "SpvOpSNegate"; break;
        case SpvOpFNegate:                                                              return "SpvOpFNegate"; break;
        case SpvOpIAdd:                                                                 return "SpvOpIAdd"; break;
        case SpvOpFAdd:                                                                 return "SpvOpFAdd"; break;
        case SpvOpISub:                                                                 return "SpvOpISub"; break;
        case SpvOpFSub:                                                                 return "SpvOpFSub"; break;
        case SpvOpIMul:                                                                 return "SpvOpIMul"; break;
        case SpvOpFMul:                                                                 return "SpvOpFMul"; break;
        case SpvOpUDiv:                                                                 return "SpvOpUDiv"; break;
        case SpvOpSDiv:                                                                 return "SpvOpSDiv"; break;
        case SpvOpFDiv:                                                                 return "SpvOpFDiv"; break;
        case SpvOpUMod:                                                                 return "SpvOpUMod"; break;
        case SpvOpSRem:                                                                 return "SpvOpSRem"; break;
        case SpvOpSMod:                                                                 return "SpvOpSMod"; break;
        case SpvOpFRem:                                                                 return "SpvOpFRem"; break;
        case SpvOpFMod:                                                                 return "SpvOpFMod"; break;
        case SpvOpVectorTimesScalar:                                                    return "SpvOpVectorTimesScalar"; break;
        case SpvOpMatrixTimesScalar:                                                    return "SpvOpMatrixTimesScalar"; break;
        case SpvOpVectorTimesMatrix:                                                    return "SpvOpVectorTimesMatrix"; break;
        case SpvOpMatrixTimesVector:                                                    return "SpvOpMatrixTimesVector"; break;
        case SpvOpMatrixTimesMatrix:                                                    return "SpvOpMatrixTimesMatrix"; break;
        case SpvOpOuterProduct:                                                         return "SpvOpOuterProduct"; break;
        case SpvOpDot:                                                                  return "SpvOpDot"; break;
        case SpvOpIAddCarry:                                                            return "SpvOpIAddCarry"; break;
        case SpvOpISubBorrow:                                                           return "SpvOpISubBorrow"; break;
        case SpvOpUMulExtended:                                                         return "SpvOpUMulExtended"; break;
        case SpvOpSMulExtended:                                                         return "SpvOpSMulExtended"; break;
        case SpvOpAny:                                                                  return "SpvOpAny"; break;
        case SpvOpAll:                                                                  return "SpvOpAll"; break;
        case SpvOpIsNan:                                                                return "SpvOpIsNan"; break;
        case SpvOpIsInf:                                                                return "SpvOpIsInf"; break;
        case SpvOpIsFinite:                                                             return "SpvOpIsFinite"; break;
        case SpvOpIsNormal:                                                             return "SpvOpIsNormal"; break;
        case SpvOpSignBitSet:                                                           return "SpvOpSignBitSet"; break;
        case SpvOpLessOrGreater:                                                        return "SpvOpLessOrGreater"; break;
        case SpvOpOrdered:                                                              return "SpvOpOrdered"; break;
        case SpvOpUnordered:                                                            return "SpvOpUnordered"; break;
        case SpvOpLogicalEqual:                                                         return "SpvOpLogicalEqual"; break;
        case SpvOpLogicalNotEqual:                                                      return "SpvOpLogicalNotEqual"; break;
        case SpvOpLogicalOr:                                                            return "SpvOpLogicalOr"; break;
        case SpvOpLogicalAnd:                                                           return "SpvOpLogicalAnd"; break;
        case SpvOpLogicalNot:                                                           return "SpvOpLogicalNot"; break;
        case SpvOpSelect:                                                               return "SpvOpSelect"; break;
        case SpvOpIEqual:                                                               return "SpvOpIEqual"; break;
        case SpvOpINotEqual:                                                            return "SpvOpINotEqual"; break;
        case SpvOpUGreaterThan:                                                         return "SpvOpUGreaterThan"; break;
        case SpvOpSGreaterThan:                                                         return "SpvOpSGreaterThan"; break;
        case SpvOpUGreaterThanEqual:                                                    return "SpvOpUGreaterThanEqual"; break;
        case SpvOpSGreaterThanEqual:                                                    return "SpvOpSGreaterThanEqual"; break;
        case SpvOpULessThan:                                                            return "SpvOpULessThan"; break;
        case SpvOpSLessThan:                                                            return "SpvOpSLessThan"; break;
        case SpvOpULessThanEqual:                                                       return "SpvOpULessThanEqual"; break;
        case SpvOpSLessThanEqual:                                                       return "SpvOpSLessThanEqual"; break;
        case SpvOpFOrdEqual:                                                            return "SpvOpFOrdEqual"; break;
        case SpvOpFUnordEqual:                                                          return "SpvOpFUnordEqual"; break;
        case SpvOpFOrdNotEqual:                                                         return "SpvOpFOrdNotEqual"; break;
        case SpvOpFUnordNotEqual:                                                       return "SpvOpFUnordNotEqual"; break;
        case SpvOpFOrdLessThan:                                                         return "SpvOpFOrdLessThan"; break;
        case SpvOpFUnordLessThan:                                                       return "SpvOpFUnordLessThan"; break;
        case SpvOpFOrdGreaterThan:                                                      return "SpvOpFOrdGreaterThan"; break;
        case SpvOpFUnordGreaterThan:                                                    return "SpvOpFUnordGreaterThan"; break;
        case SpvOpFOrdLessThanEqual:                                                    return "SpvOpFOrdLessThanEqual"; break;
        case SpvOpFUnordLessThanEqual:                                                  return "SpvOpFUnordLessThanEqual"; break;
        case SpvOpFOrdGreaterThanEqual:                                                 return "SpvOpFOrdGreaterThanEqual"; break;
        case SpvOpFUnordGreaterThanEqual:                                               return "SpvOpFUnordGreaterThanEqual"; break;
        case SpvOpShiftRightLogical:                                                    return "SpvOpShiftRightLogical"; break;
        case SpvOpShiftRightArithmetic:                                                 return "SpvOpShiftRightArithmetic"; break;
        case SpvOpShiftLeftLogical:                                                     return "SpvOpShiftLeftLogical"; break;
        case SpvOpBitwiseOr:                                                            return "SpvOpBitwiseOr"; break;
        case SpvOpBitwiseXor:                                                           return "SpvOpBitwiseXor"; break;
        case SpvOpBitwiseAnd:                                                           return "SpvOpBitwiseAnd"; break;
        case SpvOpNot:                                                                  return "SpvOpNot"; break;
        case SpvOpBitFieldInsert:                                                       return "SpvOpBitFieldInsert"; break;
        case SpvOpBitFieldSExtract:                                                     return "SpvOpBitFieldSExtract"; break;
        case SpvOpBitFieldUExtract:                                                     return "SpvOpBitFieldUExtract"; break;
        case SpvOpBitReverse:                                                           return "SpvOpBitReverse"; break;
        case SpvOpBitCount:                                                             return "SpvOpBitCount"; break;
        case SpvOpDPdx:                                                                 return "SpvOpDPdx"; break;
        case SpvOpDPdy:                                                                 return "SpvOpDPdy"; break;
        case SpvOpFwidth:                                                               return "SpvOpFwidth"; break;
        case SpvOpDPdxFine:                                                             return "SpvOpDPdxFine"; break;
        case SpvOpDPdyFine:                                                             return "SpvOpDPdyFine"; break;
        case SpvOpFwidthFine:                                                           return "SpvOpFwidthFine"; break;
        case SpvOpDPdxCoarse:                                                           return "SpvOpDPdxCoarse"; break;
        case SpvOpDPdyCoarse:                                                           return "SpvOpDPdyCoarse"; break;
        case SpvOpFwidthCoarse:                                                         return "SpvOpFwidthCoarse"; break;
        case SpvOpEmitVertex:                                                           return "SpvOpEmitVertex"; break;
        case SpvOpEndPrimitive:                                                         return "SpvOpEndPrimitive"; break;
        case SpvOpEmitStreamVertex:                                                     return "SpvOpEmitStreamVertex"; break;
        case SpvOpEndStreamPrimitive:                                                   return "SpvOpEndStreamPrimitive"; break;
        case SpvOpControlBarrier:                                                       return "SpvOpControlBarrier"; break;
        case SpvOpMemoryBarrier:                                                        return "SpvOpMemoryBarrier"; break;
        case SpvOpAtomicLoad:                                                           return "SpvOpAtomicLoad"; break;
        case SpvOpAtomicStore:                                                          return "SpvOpAtomicStore"; break;
        case SpvOpAtomicExchange:                                                       return "SpvOpAtomicExchange"; break;
        case SpvOpAtomicCompareExchange:                                                return "SpvOpAtomicCompareExchange"; break;
        case SpvOpAtomicCompareExchangeWeak:                                            return "SpvOpAtomicCompareExchangeWeak"; break;
        case SpvOpAtomicIIncrement:                                                     return "SpvOpAtomicIIncrement"; break;
        case SpvOpAtomicIDecrement:                                                     return "SpvOpAtomicIDecrement"; break;
        case SpvOpAtomicIAdd:                                                           return "SpvOpAtomicIAdd"; break;
        case SpvOpAtomicISub:                                                           return "SpvOpAtomicISub"; break;
        case SpvOpAtomicSMin:                                                           return "SpvOpAtomicSMin"; break;
        case SpvOpAtomicUMin:                                                           return "SpvOpAtomicUMin"; break;
        case SpvOpAtomicSMax:                                                           return "SpvOpAtomicSMax"; break;
        case SpvOpAtomicUMax:                                                           return "SpvOpAtomicUMax"; break;
        case SpvOpAtomicAnd:                                                            return "SpvOpAtomicAnd"; break;
        case SpvOpAtomicOr:                                                             return "SpvOpAtomicOr"; break;
        case SpvOpAtomicXor:                                                            return "SpvOpAtomicXor"; break;
        case SpvOpPhi:                                                                  return "SpvOpPhi"; break;
        case SpvOpLoopMerge:                                                            return "SpvOpLoopMerge"; break;
        case SpvOpSelectionMerge:                                                       return "SpvOpSelectionMerge"; break;
        case SpvOpLabel:                                                                return "SpvOpLabel"; break;
        case SpvOpBranch:                                                               return "SpvOpBranch"; break;
        case SpvOpBranchConditional:                                                    return "SpvOpBranchConditional"; break;
        case SpvOpSwitch:                                                               return "SpvOpSwitch"; break;
        case SpvOpKill:                                                                 return "SpvOpKill"; break;
        case SpvOpReturn:                                                               return "SpvOpReturn"; break;
        case SpvOpReturnValue:                                                          return "SpvOpReturnValue"; break;
        case SpvOpUnreachable:                                                          return "SpvOpUnreachable"; break;
        case SpvOpLifetimeStart:                                                        return "SpvOpLifetimeStart"; break;
        case SpvOpLifetimeStop:                                                         return "SpvOpLifetimeStop"; break;
        case SpvOpGroupAsyncCopy:                                                       return "SpvOpGroupAsyncCopy"; break;
        case SpvOpGroupWaitEvents:                                                      return "SpvOpGroupWaitEvents"; break;
        case SpvOpGroupAll:                                                             return "SpvOpGroupAll"; break;
        case SpvOpGroupAny:                                                             return "SpvOpGroupAny"; break;
        case SpvOpGroupBroadcast:                                                       return "SpvOpGroupBroadcast"; break;
        case SpvOpGroupIAdd:                                                            return "SpvOpGroupIAdd"; break;
        case SpvOpGroupFAdd:                                                            return "SpvOpGroupFAdd"; break;
        case SpvOpGroupFMin:                                                            return "SpvOpGroupFMin"; break;
        case SpvOpGroupUMin:                                                            return "SpvOpGroupUMin"; break;
        case SpvOpGroupSMin:                                                            return "SpvOpGroupSMin"; break;
        case SpvOpGroupFMax:                                                            return "SpvOpGroupFMax"; break;
        case SpvOpGroupUMax:                                                            return "SpvOpGroupUMax"; break;
        case SpvOpGroupSMax:                                                            return "SpvOpGroupSMax"; break;
        case SpvOpReadPipe:                                                             return "SpvOpReadPipe"; break;
        case SpvOpWritePipe:                                                            return "SpvOpWritePipe"; break;
        case SpvOpReservedReadPipe:                                                     return "SpvOpReservedReadPipe"; break;
        case SpvOpReservedWritePipe:                                                    return "SpvOpReservedWritePipe"; break;
        case SpvOpReserveReadPipePackets:                                               return "SpvOpReserveReadPipePackets"; break;
        case SpvOpReserveWritePipePackets:                                              return "SpvOpReserveWritePipePackets"; break;
        case SpvOpCommitReadPipe:                                                       return "SpvOpCommitReadPipe"; break;
        case SpvOpCommitWritePipe:                                                      return "SpvOpCommitWritePipe"; break;
        case SpvOpIsValidReserveId:                                                     return "SpvOpIsValidReserveId"; break;
        case SpvOpGetNumPipePackets:                                                    return "SpvOpGetNumPipePackets"; break;
        case SpvOpGetMaxPipePackets:                                                    return "SpvOpGetMaxPipePackets"; break;
        case SpvOpGroupReserveReadPipePackets:                                          return "SpvOpGroupReserveReadPipePackets"; break;
        case SpvOpGroupReserveWritePipePackets:                                         return "SpvOpGroupReserveWritePipePackets"; break;
        case SpvOpGroupCommitReadPipe:                                                  return "SpvOpGroupCommitReadPipe"; break;
        case SpvOpGroupCommitWritePipe:                                                 return "SpvOpGroupCommitWritePipe"; break;
        case SpvOpEnqueueMarker:                                                        return "SpvOpEnqueueMarker"; break;
        case SpvOpEnqueueKernel:                                                        return "SpvOpEnqueueKernel"; break;
        case SpvOpGetKernelNDrangeSubGroupCount:                                        return "SpvOpGetKernelNDrangeSubGroupCount"; break;
        case SpvOpGetKernelNDrangeMaxSubGroupSize:                                      return "SpvOpGetKernelNDrangeMaxSubGroupSize"; break;
        case SpvOpGetKernelWorkGroupSize:                                               return "SpvOpGetKernelWorkGroupSize"; break;
        case SpvOpGetKernelPreferredWorkGroupSizeMultiple:                              return "SpvOpGetKernelPreferredWorkGroupSizeMultiple"; break;
        case SpvOpRetainEvent:                                                          return "SpvOpRetainEvent"; break;
        case SpvOpReleaseEvent:                                                         return "SpvOpReleaseEvent"; break;
        case SpvOpCreateUserEvent:                                                      return "SpvOpCreateUserEvent"; break;
        case SpvOpIsValidEvent:                                                         return "SpvOpIsValidEvent"; break;
        case SpvOpSetUserEventStatus:                                                   return "SpvOpSetUserEventStatus"; break;
        case SpvOpCaptureEventProfilingInfo:                                            return "SpvOpCaptureEventProfilingInfo"; break;
        case SpvOpGetDefaultQueue:                                                      return "SpvOpGetDefaultQueue"; break;
        case SpvOpBuildNDRange:                                                         return "SpvOpBuildNDRange"; break;
        case SpvOpImageSparseSampleImplicitLod:                                         return "SpvOpImageSparseSampleImplicitLod"; break;
        case SpvOpImageSparseSampleExplicitLod:                                         return "SpvOpImageSparseSampleExplicitLod"; break;
        case SpvOpImageSparseSampleDrefImplicitLod:                                     return "SpvOpImageSparseSampleDrefImplicitLod"; break;
        case SpvOpImageSparseSampleDrefExplicitLod:                                     return "SpvOpImageSparseSampleDrefExplicitLod"; break;
        case SpvOpImageSparseSampleProjImplicitLod:                                     return "SpvOpImageSparseSampleProjImplicitLod"; break;
        case SpvOpImageSparseSampleProjExplicitLod:                                     return "SpvOpImageSparseSampleProjExplicitLod"; break;
        case SpvOpImageSparseSampleProjDrefImplicitLod:                                 return "SpvOpImageSparseSampleProjDrefImplicitLod"; break;
        case SpvOpImageSparseSampleProjDrefExplicitLod:                                 return "SpvOpImageSparseSampleProjDrefExplicitLod"; break;
        case SpvOpImageSparseFetch:                                                     return "SpvOpImageSparseFetch"; break;
        case SpvOpImageSparseGather:                                                    return "SpvOpImageSparseGather"; break;
        case SpvOpImageSparseDrefGather:                                                return "SpvOpImageSparseDrefGather"; break;
        case SpvOpImageSparseTexelsResident:                                            return "SpvOpImageSparseTexelsResident"; break;
        case SpvOpNoLine:                                                               return "SpvOpNoLine"; break;
        case SpvOpAtomicFlagTestAndSet:                                                 return "SpvOpAtomicFlagTestAndSet"; break;
        case SpvOpAtomicFlagClear:                                                      return "SpvOpAtomicFlagClear"; break;
        case SpvOpImageSparseRead:                                                      return "SpvOpImageSparseRead"; break;
        case SpvOpSizeOf:                                                               return "SpvOpSizeOf"; break;
        case SpvOpTypePipeStorage:                                                      return "SpvOpTypePipeStorage"; break;
        case SpvOpConstantPipeStorage:                                                  return "SpvOpConstantPipeStorage"; break;
        case SpvOpCreatePipeFromPipeStorage:                                            return "SpvOpCreatePipeFromPipeStorage"; break;
        case SpvOpGetKernelLocalSizeForSubgroupCount:                                   return "SpvOpGetKernelLocalSizeForSubgroupCount"; break;
        case SpvOpGetKernelMaxNumSubgroups:                                             return "SpvOpGetKernelMaxNumSubgroups"; break;
        case SpvOpTypeNamedBarrier:                                                     return "SpvOpTypeNamedBarrier"; break;
        case SpvOpNamedBarrierInitialize:                                               return "SpvOpNamedBarrierInitialize"; break;
        case SpvOpMemoryNamedBarrier:                                                   return "SpvOpMemoryNamedBarrier"; break;
        case SpvOpModuleProcessed:                                                      return "SpvOpModuleProcessed"; break;
        case SpvOpExecutionModeId:                                                      return "SpvOpExecutionModeId"; break;
        case SpvOpDecorateId:                                                           return "SpvOpDecorateId"; break;
        case SpvOpGroupNonUniformElect:                                                 return "SpvOpGroupNonUniformElect"; break;
        case SpvOpGroupNonUniformAll:                                                   return "SpvOpGroupNonUniformAll"; break;
        case SpvOpGroupNonUniformAny:                                                   return "SpvOpGroupNonUniformAny"; break;
        case SpvOpGroupNonUniformAllEqual:                                              return "SpvOpGroupNonUniformAllEqual"; break;
        case SpvOpGroupNonUniformBroadcast:                                             return "SpvOpGroupNonUniformBroadcast"; break;
        case SpvOpGroupNonUniformBroadcastFirst:                                        return "SpvOpGroupNonUniformBroadcastFirst"; break;
        case SpvOpGroupNonUniformBallot:                                                return "SpvOpGroupNonUniformBallot"; break;
        case SpvOpGroupNonUniformInverseBallot:                                         return "SpvOpGroupNonUniformInverseBallot"; break;
        case SpvOpGroupNonUniformBallotBitExtract:                                      return "SpvOpGroupNonUniformBallotBitExtract"; break;
        case SpvOpGroupNonUniformBallotBitCount:                                        return "SpvOpGroupNonUniformBallotBitCount"; break;
        case SpvOpGroupNonUniformBallotFindLSB:                                         return "SpvOpGroupNonUniformBallotFindLSB"; break;
        case SpvOpGroupNonUniformBallotFindMSB:                                         return "SpvOpGroupNonUniformBallotFindMSB"; break;
        case SpvOpGroupNonUniformShuffle:                                               return "SpvOpGroupNonUniformShuffle"; break;
        case SpvOpGroupNonUniformShuffleXor:                                            return "SpvOpGroupNonUniformShuffleXor"; break;
        case SpvOpGroupNonUniformShuffleUp:                                             return "SpvOpGroupNonUniformShuffleUp"; break;
        case SpvOpGroupNonUniformShuffleDown:                                           return "SpvOpGroupNonUniformShuffleDown"; break;
        case SpvOpGroupNonUniformIAdd:                                                  return "SpvOpGroupNonUniformIAdd"; break;
        case SpvOpGroupNonUniformFAdd:                                                  return "SpvOpGroupNonUniformFAdd"; break;
        case SpvOpGroupNonUniformIMul:                                                  return "SpvOpGroupNonUniformIMul"; break;
        case SpvOpGroupNonUniformFMul:                                                  return "SpvOpGroupNonUniformFMul"; break;
        case SpvOpGroupNonUniformSMin:                                                  return "SpvOpGroupNonUniformSMin"; break;
        case SpvOpGroupNonUniformUMin:                                                  return "SpvOpGroupNonUniformUMin"; break;
        case SpvOpGroupNonUniformFMin:                                                  return "SpvOpGroupNonUniformFMin"; break;
        case SpvOpGroupNonUniformSMax:                                                  return "SpvOpGroupNonUniformSMax"; break;
        case SpvOpGroupNonUniformUMax:                                                  return "SpvOpGroupNonUniformUMax"; break;
        case SpvOpGroupNonUniformFMax:                                                  return "SpvOpGroupNonUniformFMax"; break;
        case SpvOpGroupNonUniformBitwiseAnd:                                            return "SpvOpGroupNonUniformBitwiseAnd"; break;
        case SpvOpGroupNonUniformBitwiseOr:                                             return "SpvOpGroupNonUniformBitwiseOr"; break;
        case SpvOpGroupNonUniformBitwiseXor:                                            return "SpvOpGroupNonUniformBitwiseXor"; break;
        case SpvOpGroupNonUniformLogicalAnd:                                            return "SpvOpGroupNonUniformLogicalAnd"; break;
        case SpvOpGroupNonUniformLogicalOr:                                             return "SpvOpGroupNonUniformLogicalOr"; break;
        case SpvOpGroupNonUniformLogicalXor:                                            return "SpvOpGroupNonUniformLogicalXor"; break;
        case SpvOpGroupNonUniformQuadBroadcast:                                         return "SpvOpGroupNonUniformQuadBroadcast"; break;
        case SpvOpGroupNonUniformQuadSwap:                                              return "SpvOpGroupNonUniformQuadSwap"; break;
        case SpvOpCopyLogical:                                                          return "SpvOpCopyLogical"; break;
        case SpvOpPtrEqual:                                                             return "SpvOpPtrEqual"; break;
        case SpvOpPtrNotEqual:                                                          return "SpvOpPtrNotEqual"; break;
        case SpvOpPtrDiff:                                                              return "SpvOpPtrDiff"; break;
        case SpvOpColorAttachmentReadEXT:                                               return "SpvOpColorAttachmentReadEXT"; break;
        case SpvOpDepthAttachmentReadEXT:                                               return "SpvOpDepthAttachmentReadEXT"; break;
        case SpvOpStencilAttachmentReadEXT:                                             return "SpvOpStencilAttachmentReadEXT"; break;
        case SpvOpTypeTensorARM:                                                        return "SpvOpTypeTensorARM"; break;
        case SpvOpTensorReadARM:                                                        return "SpvOpTensorReadARM"; break;
        case SpvOpTensorWriteARM:                                                       return "SpvOpTensorWriteARM"; break;
        case SpvOpTensorQuerySizeARM:                                                   return "SpvOpTensorQuerySizeARM"; break;
        case SpvOpGraphConstantARM:                                                     return "SpvOpGraphConstantARM"; break;
        case SpvOpGraphEntryPointARM:                                                   return "SpvOpGraphEntryPointARM"; break;
        case SpvOpGraphARM:                                                             return "SpvOpGraphARM"; break;
        case SpvOpGraphInputARM:                                                        return "SpvOpGraphInputARM"; break;
        case SpvOpGraphSetOutputARM:                                                    return "SpvOpGraphSetOutputARM"; break;
        case SpvOpGraphEndARM:                                                          return "SpvOpGraphEndARM"; break;
        case SpvOpTypeGraphARM:                                                         return "SpvOpTypeGraphARM"; break;
        case SpvOpTerminateInvocation:                                                  return "SpvOpTerminateInvocation"; break;
        case SpvOpTypeUntypedPointerKHR:                                                return "SpvOpTypeUntypedPointerKHR"; break;
        case SpvOpUntypedVariableKHR:                                                   return "SpvOpUntypedVariableKHR"; break;
        case SpvOpUntypedAccessChainKHR:                                                return "SpvOpUntypedAccessChainKHR"; break;
        case SpvOpUntypedInBoundsAccessChainKHR:                                        return "SpvOpUntypedInBoundsAccessChainKHR"; break;
        case SpvOpSubgroupBallotKHR:                                                    return "SpvOpSubgroupBallotKHR"; break;
        case SpvOpSubgroupFirstInvocationKHR:                                           return "SpvOpSubgroupFirstInvocationKHR"; break;
        case SpvOpUntypedPtrAccessChainKHR:                                             return "SpvOpUntypedPtrAccessChainKHR"; break;
        case SpvOpUntypedInBoundsPtrAccessChainKHR:                                     return "SpvOpUntypedInBoundsPtrAccessChainKHR"; break;
        case SpvOpUntypedArrayLengthKHR:                                                return "SpvOpUntypedArrayLengthKHR"; break;
        case SpvOpUntypedPrefetchKHR:                                                   return "SpvOpUntypedPrefetchKHR"; break;
        case SpvOpSubgroupAllKHR:                                                       return "SpvOpSubgroupAllKHR"; break;
        case SpvOpSubgroupAnyKHR:                                                       return "SpvOpSubgroupAnyKHR"; break;
        case SpvOpSubgroupAllEqualKHR:                                                  return "SpvOpSubgroupAllEqualKHR"; break;
        case SpvOpGroupNonUniformRotateKHR:                                             return "SpvOpGroupNonUniformRotateKHR"; break;
        case SpvOpSubgroupReadInvocationKHR:                                            return "SpvOpSubgroupReadInvocationKHR"; break;
        case SpvOpExtInstWithForwardRefsKHR:                                            return "SpvOpExtInstWithForwardRefsKHR"; break;
        case SpvOpUntypedGroupAsyncCopyKHR:                                             return "SpvOpUntypedGroupAsyncCopyKHR"; break;
        case SpvOpTraceRayKHR:                                                          return "SpvOpTraceRayKHR"; break;
        case SpvOpExecuteCallableKHR:                                                   return "SpvOpExecuteCallableKHR"; break;
        case SpvOpConvertUToAccelerationStructureKHR:                                   return "SpvOpConvertUToAccelerationStructureKHR"; break;
        case SpvOpIgnoreIntersectionKHR:                                                return "SpvOpIgnoreIntersectionKHR"; break;
        case SpvOpTerminateRayKHR:                                                      return "SpvOpTerminateRayKHR"; break;
        case SpvOpSDot:                                                                 return "SpvOpSDot(KHR)"; break;
        case SpvOpUDot:                                                                 return "SpvOpUDot(KHR)"; break;
        case SpvOpSUDot:                                                                return "SpvOpSUDot(KHR)"; break;
        case SpvOpSDotAccSat:                                                           return "SpvOpSDotAccSat(KHR)"; break;
        case SpvOpUDotAccSat:                                                           return "SpvOpUDotAccSat(KHR)"; break;
        case SpvOpSUDotAccSat:                                                          return "SpvOpSUDotAccSat(KHR)"; break;
        case SpvOpTypeCooperativeMatrixKHR:                                             return "SpvOpTypeCooperativeMatrixKHR"; break;
        case SpvOpCooperativeMatrixLoadKHR:                                             return "SpvOpCooperativeMatrixLoadKHR"; break;
        case SpvOpCooperativeMatrixStoreKHR:                                            return "SpvOpCooperativeMatrixStoreKHR"; break;
        case SpvOpCooperativeMatrixMulAddKHR:                                           return "SpvOpCooperativeMatrixMulAddKHR"; break;
        case SpvOpCooperativeMatrixLengthKHR:                                           return "SpvOpCooperativeMatrixLengthKHR"; break;
        case SpvOpConstantCompositeReplicateEXT:                                        return "SpvOpConstantCompositeReplicateEXT"; break;
        case SpvOpSpecConstantCompositeReplicateEXT:                                    return "SpvOpSpecConstantCompositeReplicateEXT"; break;
        case SpvOpCompositeConstructReplicateEXT:                                       return "SpvOpCompositeConstructReplicateEXT"; break;
        case SpvOpTypeRayQueryKHR:                                                      return "SpvOpTypeRayQueryKHR"; break;
        case SpvOpRayQueryInitializeKHR:                                                return "SpvOpRayQueryInitializeKHR"; break;
        case SpvOpRayQueryTerminateKHR:                                                 return "SpvOpRayQueryTerminateKHR"; break;
        case SpvOpRayQueryGenerateIntersectionKHR:                                      return "SpvOpRayQueryGenerateIntersectionKHR"; break;
        case SpvOpRayQueryConfirmIntersectionKHR:                                       return "SpvOpRayQueryConfirmIntersectionKHR"; break;
        case SpvOpRayQueryProceedKHR:                                                   return "SpvOpRayQueryProceedKHR"; break;
        case SpvOpRayQueryGetIntersectionTypeKHR:                                       return "SpvOpRayQueryGetIntersectionTypeKHR"; break;
        case SpvOpImageSampleWeightedQCOM:                                              return "SpvOpImageSampleWeightedQCOM"; break;
        case SpvOpImageBoxFilterQCOM:                                                   return "SpvOpImageBoxFilterQCOM"; break;
        case SpvOpImageBlockMatchSSDQCOM:                                               return "SpvOpImageBlockMatchSSDQCOM"; break;
        case SpvOpImageBlockMatchSADQCOM:                                               return "SpvOpImageBlockMatchSADQCOM"; break;
        case SpvOpBitCastArrayQCOM:                                                     return "SpvOpBitCastArrayQCOM"; break;
        case SpvOpImageBlockMatchWindowSSDQCOM:                                         return "SpvOpImageBlockMatchWindowSSDQCOM"; break;
        case SpvOpImageBlockMatchWindowSADQCOM:                                         return "SpvOpImageBlockMatchWindowSADQCOM"; break;
        case SpvOpImageBlockMatchGatherSSDQCOM:                                         return "SpvOpImageBlockMatchGatherSSDQCOM"; break;
        case SpvOpImageBlockMatchGatherSADQCOM:                                         return "SpvOpImageBlockMatchGatherSADQCOM"; break;
        case SpvOpCompositeConstructCoopMatQCOM:                                        return "SpvOpCompositeConstructCoopMatQCOM"; break;
        case SpvOpCompositeExtractCoopMatQCOM:                                          return "SpvOpCompositeExtractCoopMatQCOM"; break;
        case SpvOpExtractSubArrayQCOM:                                                  return "SpvOpExtractSubArrayQCOM"; break;
        case SpvOpGroupIAddNonUniformAMD:                                               return "SpvOpGroupIAddNonUniformAMD"; break;
        case SpvOpGroupFAddNonUniformAMD:                                               return "SpvOpGroupFAddNonUniformAMD"; break;
        case SpvOpGroupFMinNonUniformAMD:                                               return "SpvOpGroupFMinNonUniformAMD"; break;
        case SpvOpGroupUMinNonUniformAMD:                                               return "SpvOpGroupUMinNonUniformAMD"; break;
        case SpvOpGroupSMinNonUniformAMD:                                               return "SpvOpGroupSMinNonUniformAMD"; break;
        case SpvOpGroupFMaxNonUniformAMD:                                               return "SpvOpGroupFMaxNonUniformAMD"; break;
        case SpvOpGroupUMaxNonUniformAMD:                                               return "SpvOpGroupUMaxNonUniformAMD"; break;
        case SpvOpGroupSMaxNonUniformAMD:                                               return "SpvOpGroupSMaxNonUniformAMD"; break;
        case SpvOpFragmentMaskFetchAMD:                                                 return "SpvOpFragmentMaskFetchAMD"; break;
        case SpvOpFragmentFetchAMD:                                                     return "SpvOpFragmentFetchAMD"; break;
        case SpvOpReadClockKHR:                                                         return "SpvOpReadClockKHR"; break;
        case SpvOpAllocateNodePayloadsAMDX:                                             return "SpvOpAllocateNodePayloadsAMDX"; break;
        case SpvOpEnqueueNodePayloadsAMDX:                                              return "SpvOpEnqueueNodePayloadsAMDX"; break;
        case SpvOpTypeNodePayloadArrayAMDX:                                             return "SpvOpTypeNodePayloadArrayAMDX"; break;
        case SpvOpFinishWritingNodePayloadAMDX:                                         return "SpvOpFinishWritingNodePayloadAMDX"; break;
        case SpvOpNodePayloadArrayLengthAMDX:                                           return "SpvOpNodePayloadArrayLengthAMDX"; break;
        case SpvOpIsNodePayloadValidAMDX:                                               return "SpvOpIsNodePayloadValidAMDX"; break;
        case SpvOpConstantStringAMDX:                                                   return "SpvOpConstantStringAMDX"; break;
        case SpvOpSpecConstantStringAMDX:                                               return "SpvOpSpecConstantStringAMDX"; break;
        case SpvOpGroupNonUniformQuadAllKHR:                                            return "SpvOpGroupNonUniformQuadAllKHR"; break;
        case SpvOpGroupNonUniformQuadAnyKHR:                                            return "SpvOpGroupNonUniformQuadAnyKHR"; break;
        case SpvOpHitObjectRecordHitMotionNV:                                           return "SpvOpHitObjectRecordHitMotionNV"; break;
        case SpvOpHitObjectRecordHitWithIndexMotionNV:                                  return "SpvOpHitObjectRecordHitWithIndexMotionNV"; break;
        case SpvOpHitObjectRecordMissMotionNV:                                          return "SpvOpHitObjectRecordMissMotionNV"; break;
        case SpvOpHitObjectGetWorldToObjectNV:                                          return "SpvOpHitObjectGetWorldToObjectNV"; break;
        case SpvOpHitObjectGetObjectToWorldNV:                                          return "SpvOpHitObjectGetObjectToWorldNV"; break;
        case SpvOpHitObjectGetObjectRayDirectionNV:                                     return "SpvOpHitObjectGetObjectRayDirectionNV"; break;
        case SpvOpHitObjectGetObjectRayOriginNV:                                        return "SpvOpHitObjectGetObjectRayOriginNV"; break;
        case SpvOpHitObjectTraceRayMotionNV:                                            return "SpvOpHitObjectTraceRayMotionNV"; break;
        case SpvOpHitObjectGetShaderRecordBufferHandleNV:                               return "SpvOpHitObjectGetShaderRecordBufferHandleNV"; break;
        case SpvOpHitObjectGetShaderBindingTableRecordIndexNV:                          return "SpvOpHitObjectGetShaderBindingTableRecordIndexNV"; break;
        case SpvOpHitObjectRecordEmptyNV:                                               return "SpvOpHitObjectRecordEmptyNV"; break;
        case SpvOpHitObjectTraceRayNV:                                                  return "SpvOpHitObjectTraceRayNV"; break;
        case SpvOpHitObjectRecordHitNV:                                                 return "SpvOpHitObjectRecordHitNV"; break;
        case SpvOpHitObjectRecordHitWithIndexNV:                                        return "SpvOpHitObjectRecordHitWithIndexNV"; break;
        case SpvOpHitObjectRecordMissNV:                                                return "SpvOpHitObjectRecordMissNV"; break;
        case SpvOpHitObjectExecuteShaderNV:                                             return "SpvOpHitObjectExecuteShaderNV"; break;
        case SpvOpHitObjectGetCurrentTimeNV:                                            return "SpvOpHitObjectGetCurrentTimeNV"; break;
        case SpvOpHitObjectGetAttributesNV:                                             return "SpvOpHitObjectGetAttributesNV"; break;
        case SpvOpHitObjectGetHitKindNV:                                                return "SpvOpHitObjectGetHitKindNV"; break;
        case SpvOpHitObjectGetPrimitiveIndexNV:                                         return "SpvOpHitObjectGetPrimitiveIndexNV"; break;
        case SpvOpHitObjectGetGeometryIndexNV:                                          return "SpvOpHitObjectGetGeometryIndexNV"; break;
        case SpvOpHitObjectGetInstanceIdNV:                                             return "SpvOpHitObjectGetInstanceIdNV"; break;
        case SpvOpHitObjectGetInstanceCustomIndexNV:                                    return "SpvOpHitObjectGetInstanceCustomIndexNV"; break;
        case SpvOpHitObjectGetWorldRayDirectionNV:                                      return "SpvOpHitObjectGetWorldRayDirectionNV"; break;
        case SpvOpHitObjectGetWorldRayOriginNV:                                         return "SpvOpHitObjectGetWorldRayOriginNV"; break;
        case SpvOpHitObjectGetRayTMaxNV:                                                return "SpvOpHitObjectGetRayTMaxNV"; break;
        case SpvOpHitObjectGetRayTMinNV:                                                return "SpvOpHitObjectGetRayTMinNV"; break;
        case SpvOpHitObjectIsEmptyNV:                                                   return "SpvOpHitObjectIsEmptyNV"; break;
        case SpvOpHitObjectIsHitNV:                                                     return "SpvOpHitObjectIsHitNV"; break;
        case SpvOpHitObjectIsMissNV:                                                    return "SpvOpHitObjectIsMissNV"; break;
        case SpvOpReorderThreadWithHitObjectNV:                                         return "SpvOpReorderThreadWithHitObjectNV"; break;
        case SpvOpReorderThreadWithHintNV:                                              return "SpvOpReorderThreadWithHintNV"; break;
        case SpvOpTypeHitObjectNV:                                                      return "SpvOpTypeHitObjectNV"; break;
        case SpvOpImageSampleFootprintNV:                                               return "SpvOpImageSampleFootprintNV"; break;
        case SpvOpTypeCooperativeVectorNV:                                              return "SpvOpTypeCooperativeVectorNV"; break;
        case SpvOpCooperativeVectorMatrixMulNV:                                         return "SpvOpCooperativeVectorMatrixMulNV"; break;
        case SpvOpCooperativeVectorOuterProductAccumulateNV:                            return "SpvOpCooperativeVectorOuterProductAccumulateNV"; break;
        case SpvOpCooperativeVectorReduceSumAccumulateNV:                               return "SpvOpCooperativeVectorReduceSumAccumulateNV"; break;
        case SpvOpCooperativeVectorMatrixMulAddNV:                                      return "SpvOpCooperativeVectorMatrixMulAddNV"; break;
        case SpvOpCooperativeMatrixConvertNV:                                           return "SpvOpCooperativeMatrixConvertNV"; break;
        case SpvOpEmitMeshTasksEXT:                                                     return "SpvOpEmitMeshTasksEXT"; break;
        case SpvOpSetMeshOutputsEXT:                                                    return "SpvOpSetMeshOutputsEXT"; break;
        case SpvOpGroupNonUniformPartitionNV:                                           return "SpvOpGroupNonUniformPartitionNV"; break;
        case SpvOpWritePackedPrimitiveIndices4x8NV:                                     return "SpvOpWritePackedPrimitiveIndices4x8NV"; break;
        case SpvOpFetchMicroTriangleVertexPositionNV:                                   return "SpvOpFetchMicroTriangleVertexPositionNV"; break;
        case SpvOpFetchMicroTriangleVertexBarycentricNV:                                return "SpvOpFetchMicroTriangleVertexBarycentricNV"; break;
        case SpvOpCooperativeVectorLoadNV:                                              return "SpvOpCooperativeVectorLoadNV"; break;
        case SpvOpCooperativeVectorStoreNV:                                             return "SpvOpCooperativeVectorStoreNV"; break;
        case SpvOpReportIntersectionKHR:                                                return "SpvOpReportIntersection(KHR/NV)"; break;
        case SpvOpIgnoreIntersectionNV:                                                 return "SpvOpIgnoreIntersectionNV"; break;
        case SpvOpTerminateRayNV:                                                       return "SpvOpTerminateRayNV"; break;
        case SpvOpTraceNV:                                                              return "SpvOpTraceNV"; break;
        case SpvOpTraceMotionNV:                                                        return "SpvOpTraceMotionNV"; break;
        case SpvOpTraceRayMotionNV:                                                     return "SpvOpTraceRayMotionNV"; break;
        case SpvOpRayQueryGetIntersectionTriangleVertexPositionsKHR:                    return "SpvOpRayQueryGetIntersectionTriangleVertexPositionsKHR"; break;
        case SpvOpTypeAccelerationStructureKHR:                                         return "SpvOpTypeAccelerationStructure(KHR/NV)"; break;
        case SpvOpExecuteCallableNV:                                                    return "SpvOpExecuteCallableNV"; break;
        case SpvOpRayQueryGetIntersectionClusterIdNV:                                   return "SpvOpRayQueryGet(Intersection)ClusterIdNV"; break;
        case SpvOpHitObjectGetClusterIdNV:                                              return "SpvOpHitObjectGetClusterIdNV"; break;
        case SpvOpTypeCooperativeMatrixNV:                                              return "SpvOpTypeCooperativeMatrixNV"; break;
        case SpvOpCooperativeMatrixLoadNV:                                              return "SpvOpCooperativeMatrixLoadNV"; break;
        case SpvOpCooperativeMatrixStoreNV:                                             return "SpvOpCooperativeMatrixStoreNV"; break;
        case SpvOpCooperativeMatrixMulAddNV:                                            return "SpvOpCooperativeMatrixMulAddNV"; break;
        case SpvOpCooperativeMatrixLengthNV:                                            return "SpvOpCooperativeMatrixLengthNV"; break;
        case SpvOpBeginInvocationInterlockEXT:                                          return "SpvOpBeginInvocationInterlockEXT"; break;
        case SpvOpEndInvocationInterlockEXT:                                            return "SpvOpEndInvocationInterlockEXT"; break;
        case SpvOpCooperativeMatrixReduceNV:                                            return "SpvOpCooperativeMatrixReduceNV"; break;
        case SpvOpCooperativeMatrixLoadTensorNV:                                        return "SpvOpCooperativeMatrixLoadTensorNV"; break;
        case SpvOpCooperativeMatrixStoreTensorNV:                                       return "SpvOpCooperativeMatrixStoreTensorNV"; break;
        case SpvOpCooperativeMatrixPerElementOpNV:                                      return "SpvOpCooperativeMatrixPerElementOpNV"; break;
        case SpvOpTypeTensorLayoutNV:                                                   return "SpvOpTypeTensorLayoutNV"; break;
        case SpvOpTypeTensorViewNV:                                                     return "SpvOpTypeTensorViewNV"; break;
        case SpvOpCreateTensorLayoutNV:                                                 return "SpvOpCreateTensorLayoutNV"; break;
        case SpvOpTensorLayoutSetDimensionNV:                                           return "SpvOpTensorLayoutSetDimensionNV"; break;
        case SpvOpTensorLayoutSetStrideNV:                                              return "SpvOpTensorLayoutSetStrideNV"; break;
        case SpvOpTensorLayoutSliceNV:                                                  return "SpvOpTensorLayoutSliceNV"; break;
        case SpvOpTensorLayoutSetClampValueNV:                                          return "SpvOpTensorLayoutSetClampValueNV"; break;
        case SpvOpCreateTensorViewNV:                                                   return "SpvOpCreateTensorViewNV"; break;
        case SpvOpTensorViewSetDimensionNV:                                             return "SpvOpTensorViewSetDimensionNV"; break;
        case SpvOpTensorViewSetStrideNV:                                                return "SpvOpTensorViewSetStrideNV"; break;
        case SpvOpDemoteToHelperInvocation:                                             return "SpvOpDemoteToHelperInvocation(EXT)"; break;
        case SpvOpIsHelperInvocationEXT:                                                return "SpvOpIsHelperInvocationEXT"; break;
        case SpvOpTensorViewSetClipNV:                                                  return "SpvOpTensorViewSetClipNV"; break;
        case SpvOpTensorLayoutSetBlockSizeNV:                                           return "SpvOpTensorLayoutSetBlockSizeNV"; break;
        case SpvOpCooperativeMatrixTransposeNV:                                         return "SpvOpCooperativeMatrixTransposeNV"; break;
        case SpvOpConvertUToImageNV:                                                    return "SpvOpConvertUToImageNV"; break;
        case SpvOpConvertUToSamplerNV:                                                  return "SpvOpConvertUToSamplerNV"; break;
        case SpvOpConvertImageToUNV:                                                    return "SpvOpConvertImageToUNV"; break;
        case SpvOpConvertSamplerToUNV:                                                  return "SpvOpConvertSamplerToUNV"; break;
        case SpvOpConvertUToSampledImageNV:                                             return "SpvOpConvertUToSampledImageNV"; break;
        case SpvOpConvertSampledImageToUNV:                                             return "SpvOpConvertSampledImageToUNV"; break;
        case SpvOpSamplerImageAddressingModeNV:                                         return "SpvOpSamplerImageAddressingModeNV"; break;
        case SpvOpRawAccessChainNV:                                                     return "SpvOpRawAccessChainNV"; break;
        case SpvOpRayQueryGetIntersectionSpherePositionNV:                              return "SpvOpRayQueryGetIntersectionSpherePositionNV"; break;
        case SpvOpRayQueryGetIntersectionSphereRadiusNV:                                return "SpvOpRayQueryGetIntersectionSphereRadiusNV"; break;
        case SpvOpRayQueryGetIntersectionLSSPositionsNV:                                return "SpvOpRayQueryGetIntersectionLSSPositionsNV"; break;
        case SpvOpRayQueryGetIntersectionLSSRadiiNV:                                    return "SpvOpRayQueryGetIntersectionLSSRadiiNV"; break;
        case SpvOpRayQueryGetIntersectionLSSHitValueNV:                                 return "SpvOpRayQueryGetIntersectionLSSHitValueNV"; break;
        case SpvOpHitObjectGetSpherePositionNV:                                         return "SpvOpHitObjectGetSpherePositionNV"; break;
        case SpvOpHitObjectGetSphereRadiusNV:                                           return "SpvOpHitObjectGetSphereRadiusNV"; break;
        case SpvOpHitObjectGetLSSPositionsNV:                                           return "SpvOpHitObjectGetLSSPositionsNV"; break;
        case SpvOpHitObjectGetLSSRadiiNV:                                               return "SpvOpHitObjectGetLSSRadiiNV"; break;
        case SpvOpHitObjectIsSphereHitNV:                                               return "SpvOpHitObjectIsSphereHitNV"; break;
        case SpvOpHitObjectIsLSSHitNV:                                                  return "SpvOpHitObjectIsLSSHitNV"; break;
        case SpvOpRayQueryIsSphereHitNV:                                                return "SpvOpRayQueryIsSphereHitNV"; break;
        case SpvOpRayQueryIsLSSHitNV:                                                   return "SpvOpRayQueryIsLSSHitNV"; break;
        case SpvOpSubgroupShuffleINTEL:                                                 return "SpvOpSubgroupShuffleINTEL"; break;
        case SpvOpSubgroupShuffleDownINTEL:                                             return "SpvOpSubgroupShuffleDownINTEL"; break;
        case SpvOpSubgroupShuffleUpINTEL:                                               return "SpvOpSubgroupShuffleUpINTEL"; break;
        case SpvOpSubgroupShuffleXorINTEL:                                              return "SpvOpSubgroupShuffleXorINTEL"; break;
        case SpvOpSubgroupBlockReadINTEL:                                               return "SpvOpSubgroupBlockReadINTEL"; break;
        case SpvOpSubgroupBlockWriteINTEL:                                              return "SpvOpSubgroupBlockWriteINTEL"; break;
        case SpvOpSubgroupImageBlockReadINTEL:                                          return "SpvOpSubgroupImageBlockReadINTEL"; break;
        case SpvOpSubgroupImageBlockWriteINTEL:                                         return "SpvOpSubgroupImageBlockWriteINTEL"; break;
        case SpvOpSubgroupImageMediaBlockReadINTEL:                                     return "SpvOpSubgroupImageMediaBlockReadINTEL"; break;
        case SpvOpSubgroupImageMediaBlockWriteINTEL:                                    return "SpvOpSubgroupImageMediaBlockWriteINTEL"; break;
        case SpvOpUCountLeadingZerosINTEL:                                              return "SpvOpUCountLeadingZerosINTEL"; break;
        case SpvOpUCountTrailingZerosINTEL:                                             return "SpvOpUCountTrailingZerosINTEL"; break;
        case SpvOpAbsISubINTEL:                                                         return "SpvOpAbsISubINTEL"; break;
        case SpvOpAbsUSubINTEL:                                                         return "SpvOpAbsUSubINTEL"; break;
        case SpvOpIAddSatINTEL:                                                         return "SpvOpIAddSatINTEL"; break;
        case SpvOpUAddSatINTEL:                                                         return "SpvOpUAddSatINTEL"; break;
        case SpvOpIAverageINTEL:                                                        return "SpvOpIAverageINTEL"; break;
        case SpvOpUAverageINTEL:                                                        return "SpvOpUAverageINTEL"; break;
        case SpvOpIAverageRoundedINTEL:                                                 return "SpvOpIAverageRoundedINTEL"; break;
        case SpvOpUAverageRoundedINTEL:                                                 return "SpvOpUAverageRoundedINTEL"; break;
        case SpvOpISubSatINTEL:                                                         return "SpvOpISubSatINTEL"; break;
        case SpvOpUSubSatINTEL:                                                         return "SpvOpUSubSatINTEL"; break;
        case SpvOpIMul32x16INTEL:                                                       return "SpvOpIMul32x16INTEL"; break;
        case SpvOpUMul32x16INTEL:                                                       return "SpvOpUMul32x16INTEL"; break;
        case SpvOpConstantFunctionPointerINTEL:                                         return "SpvOpConstantFunctionPointerINTEL"; break;
        case SpvOpFunctionPointerCallINTEL:                                             return "SpvOpFunctionPointerCallINTEL"; break;
        case SpvOpAsmTargetINTEL:                                                       return "SpvOpAsmTargetINTEL"; break;
        case SpvOpAsmINTEL:                                                             return "SpvOpAsmINTEL"; break;
        case SpvOpAsmCallINTEL:                                                         return "SpvOpAsmCallINTEL"; break;
        case SpvOpAtomicFMinEXT:                                                        return "SpvOpAtomicFMinEXT"; break;
        case SpvOpAtomicFMaxEXT:                                                        return "SpvOpAtomicFMaxEXT"; break;
        case SpvOpAssumeTrueKHR:                                                        return "SpvOpAssumeTrueKHR"; break;
        case SpvOpExpectKHR:                                                            return "SpvOpExpectKHR"; break;
        case SpvOpDecorateStringGOOGLE:                                                 return "SpvOpDecorateString(GOOGLE)"; break;
        case SpvOpMemberDecorateStringGOOGLE:                                           return "SpvOpMemberDecorateString(GOOGLE)"; break;
        case SpvOpVmeImageINTEL:                                                        return "SpvOpVmeImageINTEL"; break;
        case SpvOpTypeVmeImageINTEL:                                                    return "SpvOpTypeVmeImageINTEL"; break;
        case SpvOpTypeAvcImePayloadINTEL:                                               return "SpvOpTypeAvcImePayloadINTEL"; break;
        case SpvOpTypeAvcRefPayloadINTEL:                                               return "SpvOpTypeAvcRefPayloadINTEL"; break;
        case SpvOpTypeAvcSicPayloadINTEL:                                               return "SpvOpTypeAvcSicPayloadINTEL"; break;
        case SpvOpTypeAvcMcePayloadINTEL:                                               return "SpvOpTypeAvcMcePayloadINTEL"; break;
        case SpvOpTypeAvcMceResultINTEL:                                                return "SpvOpTypeAvcMceResultINTEL"; break;
        case SpvOpTypeAvcImeResultINTEL:                                                return "SpvOpTypeAvcImeResultINTEL"; break;
        case SpvOpTypeAvcImeResultSingleReferenceStreamoutINTEL:                        return "SpvOpTypeAvcImeResultSingleReferenceStreamoutINTEL"; break;
        case SpvOpTypeAvcImeResultDualReferenceStreamoutINTEL:                          return "SpvOpTypeAvcImeResultDualReferenceStreamoutINTEL"; break;
        case SpvOpTypeAvcImeSingleReferenceStreaminINTEL:                               return "SpvOpTypeAvcImeSingleReferenceStreaminINTEL"; break;
        case SpvOpTypeAvcImeDualReferenceStreaminINTEL:                                 return "SpvOpTypeAvcImeDualReferenceStreaminINTEL"; break;
        case SpvOpTypeAvcRefResultINTEL:                                                return "SpvOpTypeAvcRefResultINTEL"; break;
        case SpvOpTypeAvcSicResultINTEL:                                                return "SpvOpTypeAvcSicResultINTEL"; break;
        case SpvOpSubgroupAvcMceGetDefaultInterBaseMultiReferencePenaltyINTEL:          return "SpvOpSubgroupAvcMceGetDefaultInterBaseMultiReferencePenaltyINTEL"; break;
        case SpvOpSubgroupAvcMceSetInterBaseMultiReferencePenaltyINTEL:                 return "SpvOpSubgroupAvcMceSetInterBaseMultiReferencePenaltyINTEL"; break;
        case SpvOpSubgroupAvcMceGetDefaultInterShapePenaltyINTEL:                       return "SpvOpSubgroupAvcMceGetDefaultInterShapePenaltyINTEL"; break;
        case SpvOpSubgroupAvcMceSetInterShapePenaltyINTEL:                              return "SpvOpSubgroupAvcMceSetInterShapePenaltyINTEL"; break;
        case SpvOpSubgroupAvcMceGetDefaultInterDirectionPenaltyINTEL:                   return "SpvOpSubgroupAvcMceGetDefaultInterDirectionPenaltyINTEL"; break;
        case SpvOpSubgroupAvcMceSetInterDirectionPenaltyINTEL:                          return "SpvOpSubgroupAvcMceSetInterDirectionPenaltyINTEL"; break;
        case SpvOpSubgroupAvcMceGetDefaultIntraLumaShapePenaltyINTEL:                   return "SpvOpSubgroupAvcMceGetDefaultIntraLumaShapePenaltyINTEL"; break;
        case SpvOpSubgroupAvcMceGetDefaultInterMotionVectorCostTableINTEL:              return "SpvOpSubgroupAvcMceGetDefaultInterMotionVectorCostTableINTEL"; break;
        case SpvOpSubgroupAvcMceGetDefaultHighPenaltyCostTableINTEL:                    return "SpvOpSubgroupAvcMceGetDefaultHighPenaltyCostTableINTEL"; break;
        case SpvOpSubgroupAvcMceGetDefaultMediumPenaltyCostTableINTEL:                  return "SpvOpSubgroupAvcMceGetDefaultMediumPenaltyCostTableINTEL"; break;
        case SpvOpSubgroupAvcMceGetDefaultLowPenaltyCostTableINTEL:                     return "SpvOpSubgroupAvcMceGetDefaultLowPenaltyCostTableINTEL"; break;
        case SpvOpSubgroupAvcMceSetMotionVectorCostFunctionINTEL:                       return "SpvOpSubgroupAvcMceSetMotionVectorCostFunctionINTEL"; break;
        case SpvOpSubgroupAvcMceGetDefaultIntraLumaModePenaltyINTEL:                    return "SpvOpSubgroupAvcMceGetDefaultIntraLumaModePenaltyINTEL"; break;
        case SpvOpSubgroupAvcMceGetDefaultNonDcLumaIntraPenaltyINTEL:                   return "SpvOpSubgroupAvcMceGetDefaultNonDcLumaIntraPenaltyINTEL"; break;
        case SpvOpSubgroupAvcMceGetDefaultIntraChromaModeBasePenaltyINTEL:              return "SpvOpSubgroupAvcMceGetDefaultIntraChromaModeBasePenaltyINTEL"; break;
        case SpvOpSubgroupAvcMceSetAcOnlyHaarINTEL:                                     return "SpvOpSubgroupAvcMceSetAcOnlyHaarINTEL"; break;
        case SpvOpSubgroupAvcMceSetSourceInterlacedFieldPolarityINTEL:                  return "SpvOpSubgroupAvcMceSetSourceInterlacedFieldPolarityINTEL"; break;
        case SpvOpSubgroupAvcMceSetSingleReferenceInterlacedFieldPolarityINTEL:         return "SpvOpSubgroupAvcMceSetSingleReferenceInterlacedFieldPolarityINTEL"; break;
        case SpvOpSubgroupAvcMceSetDualReferenceInterlacedFieldPolaritiesINTEL:         return "SpvOpSubgroupAvcMceSetDualReferenceInterlacedFieldPolaritiesINTEL"; break;
        case SpvOpSubgroupAvcMceConvertToImePayloadINTEL:                               return "SpvOpSubgroupAvcMceConvertToImePayloadINTEL"; break;
        case SpvOpSubgroupAvcMceConvertToImeResultINTEL:                                return "SpvOpSubgroupAvcMceConvertToImeResultINTEL"; break;
        case SpvOpSubgroupAvcMceConvertToRefPayloadINTEL:                               return "SpvOpSubgroupAvcMceConvertToRefPayloadINTEL"; break;
        case SpvOpSubgroupAvcMceConvertToRefResultINTEL:                                return "SpvOpSubgroupAvcMceConvertToRefResultINTEL"; break;
        case SpvOpSubgroupAvcMceConvertToSicPayloadINTEL:                               return "SpvOpSubgroupAvcMceConvertToSicPayloadINTEL"; break;
        case SpvOpSubgroupAvcMceConvertToSicResultINTEL:                                return "SpvOpSubgroupAvcMceConvertToSicResultINTEL"; break;
        case SpvOpSubgroupAvcMceGetMotionVectorsINTEL:                                  return "SpvOpSubgroupAvcMceGetMotionVectorsINTEL"; break;
        case SpvOpSubgroupAvcMceGetInterDistortionsINTEL:                               return "SpvOpSubgroupAvcMceGetInterDistortionsINTEL"; break;
        case SpvOpSubgroupAvcMceGetBestInterDistortionsINTEL:                           return "SpvOpSubgroupAvcMceGetBestInterDistortionsINTEL"; break;
        case SpvOpSubgroupAvcMceGetInterMajorShapeINTEL:                                return "SpvOpSubgroupAvcMceGetInterMajorShapeINTEL"; break;
        case SpvOpSubgroupAvcMceGetInterMinorShapeINTEL:                                return "SpvOpSubgroupAvcMceGetInterMinorShapeINTEL"; break;
        case SpvOpSubgroupAvcMceGetInterDirectionsINTEL:                                return "SpvOpSubgroupAvcMceGetInterDirectionsINTEL"; break;
        case SpvOpSubgroupAvcMceGetInterMotionVectorCountINTEL:                         return "SpvOpSubgroupAvcMceGetInterMotionVectorCountINTEL"; break;
        case SpvOpSubgroupAvcMceGetInterReferenceIdsINTEL:                              return "SpvOpSubgroupAvcMceGetInterReferenceIdsINTEL"; break;
        case SpvOpSubgroupAvcMceGetInterReferenceInterlacedFieldPolaritiesINTEL:        return "SpvOpSubgroupAvcMceGetInterReferenceInterlacedFieldPolaritiesINTEL"; break;
        case SpvOpSubgroupAvcImeInitializeINTEL:                                        return "SpvOpSubgroupAvcImeInitializeINTEL"; break;
        case SpvOpSubgroupAvcImeSetSingleReferenceINTEL:                                return "SpvOpSubgroupAvcImeSetSingleReferenceINTEL"; break;
        case SpvOpSubgroupAvcImeSetDualReferenceINTEL:                                  return "SpvOpSubgroupAvcImeSetDualReferenceINTEL"; break;
        case SpvOpSubgroupAvcImeRefWindowSizeINTEL:                                     return "SpvOpSubgroupAvcImeRefWindowSizeINTEL"; break;
        case SpvOpSubgroupAvcImeAdjustRefOffsetINTEL:                                   return "SpvOpSubgroupAvcImeAdjustRefOffsetINTEL"; break;
        case SpvOpSubgroupAvcImeConvertToMcePayloadINTEL:                               return "SpvOpSubgroupAvcImeConvertToMcePayloadINTEL"; break;
        case SpvOpSubgroupAvcImeSetMaxMotionVectorCountINTEL:                           return "SpvOpSubgroupAvcImeSetMaxMotionVectorCountINTEL"; break;
        case SpvOpSubgroupAvcImeSetUnidirectionalMixDisableINTEL:                       return "SpvOpSubgroupAvcImeSetUnidirectionalMixDisableINTEL"; break;
        case SpvOpSubgroupAvcImeSetEarlySearchTerminationThresholdINTEL:                return "SpvOpSubgroupAvcImeSetEarlySearchTerminationThresholdINTEL"; break;
        case SpvOpSubgroupAvcImeSetWeightedSadINTEL:                                    return "SpvOpSubgroupAvcImeSetWeightedSadINTEL"; break;
        case SpvOpSubgroupAvcImeEvaluateWithSingleReferenceINTEL:                       return "SpvOpSubgroupAvcImeEvaluateWithSingleReferenceINTEL"; break;
        case SpvOpSubgroupAvcImeEvaluateWithDualReferenceINTEL:                         return "SpvOpSubgroupAvcImeEvaluateWithDualReferenceINTEL"; break;
        case SpvOpSubgroupAvcImeEvaluateWithSingleReferenceStreaminINTEL:               return "SpvOpSubgroupAvcImeEvaluateWithSingleReferenceStreaminINTEL"; break;
        case SpvOpSubgroupAvcImeEvaluateWithDualReferenceStreaminINTEL:                 return "SpvOpSubgroupAvcImeEvaluateWithDualReferenceStreaminINTEL"; break;
        case SpvOpSubgroupAvcImeEvaluateWithSingleReferenceStreamoutINTEL:              return "SpvOpSubgroupAvcImeEvaluateWithSingleReferenceStreamoutINTEL"; break;
        case SpvOpSubgroupAvcImeEvaluateWithDualReferenceStreamoutINTEL:                return "SpvOpSubgroupAvcImeEvaluateWithDualReferenceStreamoutINTEL"; break;
        case SpvOpSubgroupAvcImeEvaluateWithSingleReferenceStreaminoutINTEL:            return "SpvOpSubgroupAvcImeEvaluateWithSingleReferenceStreaminoutINTEL"; break;
        case SpvOpSubgroupAvcImeEvaluateWithDualReferenceStreaminoutINTEL:              return "SpvOpSubgroupAvcImeEvaluateWithDualReferenceStreaminoutINTEL"; break;
        case SpvOpSubgroupAvcImeConvertToMceResultINTEL:                                return "SpvOpSubgroupAvcImeConvertToMceResultINTEL"; break;
        case SpvOpSubgroupAvcImeGetSingleReferenceStreaminINTEL:                        return "SpvOpSubgroupAvcImeGetSingleReferenceStreaminINTEL"; break;
        case SpvOpSubgroupAvcImeGetDualReferenceStreaminINTEL:                          return "SpvOpSubgroupAvcImeGetDualReferenceStreaminINTEL"; break;
        case SpvOpSubgroupAvcImeStripSingleReferenceStreamoutINTEL:                     return "SpvOpSubgroupAvcImeStripSingleReferenceStreamoutINTEL"; break;
        case SpvOpSubgroupAvcImeStripDualReferenceStreamoutINTEL:                       return "SpvOpSubgroupAvcImeStripDualReferenceStreamoutINTEL"; break;
        case SpvOpSubgroupAvcImeGetStreamoutSingleReferenceMajorShapeMotionVectorsINTEL:return "SpvOpSubgroupAvcImeGetStreamoutSingleReferenceMajorShapeMotionVectorsINTEL"; break;
        case SpvOpSubgroupAvcImeGetStreamoutSingleReferenceMajorShapeDistortionsINTEL:  return "SpvOpSubgroupAvcImeGetStreamoutSingleReferenceMajorShapeDistortionsINTEL"; break;
        case SpvOpSubgroupAvcImeGetStreamoutSingleReferenceMajorShapeReferenceIdsINTEL: return "SpvOpSubgroupAvcImeGetStreamoutSingleReferenceMajorShapeReferenceIdsINTEL"; break;
        case SpvOpSubgroupAvcImeGetStreamoutDualReferenceMajorShapeMotionVectorsINTEL:  return "SpvOpSubgroupAvcImeGetStreamoutDualReferenceMajorShapeMotionVectorsINTEL"; break;
        case SpvOpSubgroupAvcImeGetStreamoutDualReferenceMajorShapeDistortionsINTEL:    return "SpvOpSubgroupAvcImeGetStreamoutDualReferenceMajorShapeDistortionsINTEL"; break;
        case SpvOpSubgroupAvcImeGetStreamoutDualReferenceMajorShapeReferenceIdsINTEL:   return "SpvOpSubgroupAvcImeGetStreamoutDualReferenceMajorShapeReferenceIdsINTEL"; break;
        case SpvOpSubgroupAvcImeGetBorderReachedINTEL:                                  return "SpvOpSubgroupAvcImeGetBorderReachedINTEL"; break;
        case SpvOpSubgroupAvcImeGetTruncatedSearchIndicationINTEL:                      return "SpvOpSubgroupAvcImeGetTruncatedSearchIndicationINTEL"; break;
        case SpvOpSubgroupAvcImeGetUnidirectionalEarlySearchTerminationINTEL:           return "SpvOpSubgroupAvcImeGetUnidirectionalEarlySearchTerminationINTEL"; break;
        case SpvOpSubgroupAvcImeGetWeightingPatternMinimumMotionVectorINTEL:            return "SpvOpSubgroupAvcImeGetWeightingPatternMinimumMotionVectorINTEL"; break;
        case SpvOpSubgroupAvcImeGetWeightingPatternMinimumDistortionINTEL:              return "SpvOpSubgroupAvcImeGetWeightingPatternMinimumDistortionINTEL"; break;
        case SpvOpSubgroupAvcFmeInitializeINTEL:                                        return "SpvOpSubgroupAvcFmeInitializeINTEL"; break;
        case SpvOpSubgroupAvcBmeInitializeINTEL:                                        return "SpvOpSubgroupAvcBmeInitializeINTEL"; break;
        case SpvOpSubgroupAvcRefConvertToMcePayloadINTEL:                               return "SpvOpSubgroupAvcRefConvertToMcePayloadINTEL"; break;
        case SpvOpSubgroupAvcRefSetBidirectionalMixDisableINTEL:                        return "SpvOpSubgroupAvcRefSetBidirectionalMixDisableINTEL"; break;
        case SpvOpSubgroupAvcRefSetBilinearFilterEnableINTEL:                           return "SpvOpSubgroupAvcRefSetBilinearFilterEnableINTEL"; break;
        case SpvOpSubgroupAvcRefEvaluateWithSingleReferenceINTEL:                       return "SpvOpSubgroupAvcRefEvaluateWithSingleReferenceINTEL"; break;
        case SpvOpSubgroupAvcRefEvaluateWithDualReferenceINTEL:                         return "SpvOpSubgroupAvcRefEvaluateWithDualReferenceINTEL"; break;
        case SpvOpSubgroupAvcRefEvaluateWithMultiReferenceINTEL:                        return "SpvOpSubgroupAvcRefEvaluateWithMultiReferenceINTEL"; break;
        case SpvOpSubgroupAvcRefEvaluateWithMultiReferenceInterlacedINTEL:              return "SpvOpSubgroupAvcRefEvaluateWithMultiReferenceInterlacedINTEL"; break;
        case SpvOpSubgroupAvcRefConvertToMceResultINTEL:                                return "SpvOpSubgroupAvcRefConvertToMceResultINTEL"; break;
        case SpvOpSubgroupAvcSicInitializeINTEL:                                        return "SpvOpSubgroupAvcSicInitializeINTEL"; break;
        case SpvOpSubgroupAvcSicConfigureSkcINTEL:                                      return "SpvOpSubgroupAvcSicConfigureSkcINTEL"; break;
        case SpvOpSubgroupAvcSicConfigureIpeLumaINTEL:                                  return "SpvOpSubgroupAvcSicConfigureIpeLumaINTEL"; break;
        case SpvOpSubgroupAvcSicConfigureIpeLumaChromaINTEL:                            return "SpvOpSubgroupAvcSicConfigureIpeLumaChromaINTEL"; break;
        case SpvOpSubgroupAvcSicGetMotionVectorMaskINTEL:                               return "SpvOpSubgroupAvcSicGetMotionVectorMaskINTEL"; break;
        case SpvOpSubgroupAvcSicConvertToMcePayloadINTEL:                               return "SpvOpSubgroupAvcSicConvertToMcePayloadINTEL"; break;
        case SpvOpSubgroupAvcSicSetIntraLumaShapePenaltyINTEL:                          return "SpvOpSubgroupAvcSicSetIntraLumaShapePenaltyINTEL"; break;
        case SpvOpSubgroupAvcSicSetIntraLumaModeCostFunctionINTEL:                      return "SpvOpSubgroupAvcSicSetIntraLumaModeCostFunctionINTEL"; break;
        case SpvOpSubgroupAvcSicSetIntraChromaModeCostFunctionINTEL:                    return "SpvOpSubgroupAvcSicSetIntraChromaModeCostFunctionINTEL"; break;
        case SpvOpSubgroupAvcSicSetBilinearFilterEnableINTEL:                           return "SpvOpSubgroupAvcSicSetBilinearFilterEnableINTEL"; break;
        case SpvOpSubgroupAvcSicSetSkcForwardTransformEnableINTEL:                      return "SpvOpSubgroupAvcSicSetSkcForwardTransformEnableINTEL"; break;
        case SpvOpSubgroupAvcSicSetBlockBasedRawSkipSadINTEL:                           return "SpvOpSubgroupAvcSicSetBlockBasedRawSkipSadINTEL"; break;
        case SpvOpSubgroupAvcSicEvaluateIpeINTEL:                                       return "SpvOpSubgroupAvcSicEvaluateIpeINTEL"; break;
        case SpvOpSubgroupAvcSicEvaluateWithSingleReferenceINTEL:                       return "SpvOpSubgroupAvcSicEvaluateWithSingleReferenceINTEL"; break;
        case SpvOpSubgroupAvcSicEvaluateWithDualReferenceINTEL:                         return "SpvOpSubgroupAvcSicEvaluateWithDualReferenceINTEL"; break;
        case SpvOpSubgroupAvcSicEvaluateWithMultiReferenceINTEL:                        return "SpvOpSubgroupAvcSicEvaluateWithMultiReferenceINTEL"; break;
        case SpvOpSubgroupAvcSicEvaluateWithMultiReferenceInterlacedINTEL:              return "SpvOpSubgroupAvcSicEvaluateWithMultiReferenceInterlacedINTEL"; break;
        case SpvOpSubgroupAvcSicConvertToMceResultINTEL:                                return "SpvOpSubgroupAvcSicConvertToMceResultINTEL"; break;
        case SpvOpSubgroupAvcSicGetIpeLumaShapeINTEL:                                   return "SpvOpSubgroupAvcSicGetIpeLumaShapeINTEL"; break;
        case SpvOpSubgroupAvcSicGetBestIpeLumaDistortionINTEL:                          return "SpvOpSubgroupAvcSicGetBestIpeLumaDistortionINTEL"; break;
        case SpvOpSubgroupAvcSicGetBestIpeChromaDistortionINTEL:                        return "SpvOpSubgroupAvcSicGetBestIpeChromaDistortionINTEL"; break;
        case SpvOpSubgroupAvcSicGetPackedIpeLumaModesINTEL:                             return "SpvOpSubgroupAvcSicGetPackedIpeLumaModesINTEL"; break;
        case SpvOpSubgroupAvcSicGetIpeChromaModeINTEL:                                  return "SpvOpSubgroupAvcSicGetIpeChromaModeINTEL"; break;
        case SpvOpSubgroupAvcSicGetPackedSkcLumaCountThresholdINTEL:                    return "SpvOpSubgroupAvcSicGetPackedSkcLumaCountThresholdINTEL"; break;
        case SpvOpSubgroupAvcSicGetPackedSkcLumaSumThresholdINTEL:                      return "SpvOpSubgroupAvcSicGetPackedSkcLumaSumThresholdINTEL"; break;
        case SpvOpSubgroupAvcSicGetInterRawSadsINTEL:                                   return "SpvOpSubgroupAvcSicGetInterRawSadsINTEL"; break;
        case SpvOpVariableLengthArrayINTEL:                                             return "SpvOpVariableLengthArrayINTEL"; break;
        case SpvOpSaveMemoryINTEL:                                                      return "SpvOpSaveMemoryINTEL"; break;
        case SpvOpRestoreMemoryINTEL:                                                   return "SpvOpRestoreMemoryINTEL"; break;
        case SpvOpArbitraryFloatSinCosPiINTEL:                                          return "SpvOpArbitraryFloatSinCosPiINTEL"; break;
        case SpvOpArbitraryFloatCastINTEL:                                              return "SpvOpArbitraryFloatCastINTEL"; break;
        case SpvOpArbitraryFloatCastFromIntINTEL:                                       return "SpvOpArbitraryFloatCastFromIntINTEL"; break;
        case SpvOpArbitraryFloatCastToIntINTEL:                                         return "SpvOpArbitraryFloatCastToIntINTEL"; break;
        case SpvOpArbitraryFloatAddINTEL:                                               return "SpvOpArbitraryFloatAddINTEL"; break;
        case SpvOpArbitraryFloatSubINTEL:                                               return "SpvOpArbitraryFloatSubINTEL"; break;
        case SpvOpArbitraryFloatMulINTEL:                                               return "SpvOpArbitraryFloatMulINTEL"; break;
        case SpvOpArbitraryFloatDivINTEL:                                               return "SpvOpArbitraryFloatDivINTEL"; break;
        case SpvOpArbitraryFloatGTINTEL:                                                return "SpvOpArbitraryFloatGTINTEL"; break;
        case SpvOpArbitraryFloatGEINTEL:                                                return "SpvOpArbitraryFloatGEINTEL"; break;
        case SpvOpArbitraryFloatLTINTEL:                                                return "SpvOpArbitraryFloatLTINTEL"; break;
        case SpvOpArbitraryFloatLEINTEL:                                                return "SpvOpArbitraryFloatLEINTEL"; break;
        case SpvOpArbitraryFloatEQINTEL:                                                return "SpvOpArbitraryFloatEQINTEL"; break;
        case SpvOpArbitraryFloatRecipINTEL:                                             return "SpvOpArbitraryFloatRecipINTEL"; break;
        case SpvOpArbitraryFloatRSqrtINTEL:                                             return "SpvOpArbitraryFloatRSqrtINTEL"; break;
        case SpvOpArbitraryFloatCbrtINTEL:                                              return "SpvOpArbitraryFloatCbrtINTEL"; break;
        case SpvOpArbitraryFloatHypotINTEL:                                             return "SpvOpArbitraryFloatHypotINTEL"; break;
        case SpvOpArbitraryFloatSqrtINTEL:                                              return "SpvOpArbitraryFloatSqrtINTEL"; break;
        case SpvOpArbitraryFloatLogINTEL:                                               return "SpvOpArbitraryFloatLogINTEL"; break;
        case SpvOpArbitraryFloatLog2INTEL:                                              return "SpvOpArbitraryFloatLog2INTEL"; break;
        case SpvOpArbitraryFloatLog10INTEL:                                             return "SpvOpArbitraryFloatLog10INTEL"; break;
        case SpvOpArbitraryFloatLog1pINTEL:                                             return "SpvOpArbitraryFloatLog1pINTEL"; break;
        case SpvOpArbitraryFloatExpINTEL:                                               return "SpvOpArbitraryFloatExpINTEL"; break;
        case SpvOpArbitraryFloatExp2INTEL:                                              return "SpvOpArbitraryFloatExp2INTEL"; break;
        case SpvOpArbitraryFloatExp10INTEL:                                             return "SpvOpArbitraryFloatExp10INTEL"; break;
        case SpvOpArbitraryFloatExpm1INTEL:                                             return "SpvOpArbitraryFloatExpm1INTEL"; break;
        case SpvOpArbitraryFloatSinINTEL:                                               return "SpvOpArbitraryFloatSinINTEL"; break;
        case SpvOpArbitraryFloatCosINTEL:                                               return "SpvOpArbitraryFloatCosINTEL"; break;
        case SpvOpArbitraryFloatSinCosINTEL:                                            return "SpvOpArbitraryFloatSinCosINTEL"; break;
        case SpvOpArbitraryFloatSinPiINTEL:                                             return "SpvOpArbitraryFloatSinPiINTEL"; break;
        case SpvOpArbitraryFloatCosPiINTEL:                                             return "SpvOpArbitraryFloatCosPiINTEL"; break;
        case SpvOpArbitraryFloatASinINTEL:                                              return "SpvOpArbitraryFloatASinINTEL"; break;
        case SpvOpArbitraryFloatASinPiINTEL:                                            return "SpvOpArbitraryFloatASinPiINTEL"; break;
        case SpvOpArbitraryFloatACosINTEL:                                              return "SpvOpArbitraryFloatACosINTEL"; break;
        case SpvOpArbitraryFloatACosPiINTEL:                                            return "SpvOpArbitraryFloatACosPiINTEL"; break;
        case SpvOpArbitraryFloatATanINTEL:                                              return "SpvOpArbitraryFloatATanINTEL"; break;
        case SpvOpArbitraryFloatATanPiINTEL:                                            return "SpvOpArbitraryFloatATanPiINTEL"; break;
        case SpvOpArbitraryFloatATan2INTEL:                                             return "SpvOpArbitraryFloatATan2INTEL"; break;
        case SpvOpArbitraryFloatPowINTEL:                                               return "SpvOpArbitraryFloatPowINTEL"; break;
        case SpvOpArbitraryFloatPowRINTEL:                                              return "SpvOpArbitraryFloatPowRINTEL"; break;
        case SpvOpArbitraryFloatPowNINTEL:                                              return "SpvOpArbitraryFloatPowNINTEL"; break;
        case SpvOpLoopControlINTEL:                                                     return "SpvOpLoopControlINTEL"; break;
        case SpvOpAliasDomainDeclINTEL:                                                 return "SpvOpAliasDomainDeclINTEL"; break;
        case SpvOpAliasScopeDeclINTEL:                                                  return "SpvOpAliasScopeDeclINTEL"; break;
        case SpvOpAliasScopeListDeclINTEL:                                              return "SpvOpAliasScopeListDeclINTEL"; break;
        case SpvOpFixedSqrtINTEL:                                                       return "SpvOpFixedSqrtINTEL"; break;
        case SpvOpFixedRecipINTEL:                                                      return "SpvOpFixedRecipINTEL"; break;
        case SpvOpFixedRsqrtINTEL:                                                      return "SpvOpFixedRsqrtINTEL"; break;
        case SpvOpFixedSinINTEL:                                                        return "SpvOpFixedSinINTEL"; break;
        case SpvOpFixedCosINTEL:                                                        return "SpvOpFixedCosINTEL"; break;
        case SpvOpFixedSinCosINTEL:                                                     return "SpvOpFixedSinCosINTEL"; break;
        case SpvOpFixedSinPiINTEL:                                                      return "SpvOpFixedSinPiINTEL"; break;
        case SpvOpFixedCosPiINTEL:                                                      return "SpvOpFixedCosPiINTEL"; break;
        case SpvOpFixedSinCosPiINTEL:                                                   return "SpvOpFixedSinCosPiINTEL"; break;
        case SpvOpFixedLogINTEL:                                                        return "SpvOpFixedLogINTEL"; break;
        case SpvOpFixedExpINTEL:                                                        return "SpvOpFixedExpINTEL"; break;
        case SpvOpPtrCastToCrossWorkgroupINTEL:                                         return "SpvOpPtrCastToCrossWorkgroupINTEL"; break;
        case SpvOpCrossWorkgroupCastToPtrINTEL:                                         return "SpvOpCrossWorkgroupCastToPtrINTEL"; break;
        case SpvOpReadPipeBlockingINTEL:                                                return "SpvOpReadPipeBlockingINTEL"; break;
        case SpvOpWritePipeBlockingINTEL:                                               return "SpvOpWritePipeBlockingINTEL"; break;
        case SpvOpFPGARegINTEL:                                                         return "SpvOpFPGARegINTEL"; break;
        case SpvOpRayQueryGetRayTMinKHR:                                                return "SpvOpRayQueryGetRayTMinKHR"; break;
        case SpvOpRayQueryGetRayFlagsKHR:                                               return "SpvOpRayQueryGetRayFlagsKHR"; break;
        case SpvOpRayQueryGetIntersectionTKHR:                                          return "SpvOpRayQueryGetIntersectionTKHR"; break;
        case SpvOpRayQueryGetIntersectionInstanceCustomIndexKHR:                        return "SpvOpRayQueryGetIntersectionInstanceCustomIndexKHR"; break;
        case SpvOpRayQueryGetIntersectionInstanceIdKHR:                                 return "SpvOpRayQueryGetIntersectionInstanceIdKHR"; break;
        case SpvOpRayQueryGetIntersectionInstanceShaderBindingTableRecordOffsetKHR:     return "SpvOpRayQueryGetIntersectionInstanceShaderBindingTableRecordOffsetKHR"; break;
        case SpvOpRayQueryGetIntersectionGeometryIndexKHR:                              return "SpvOpRayQueryGetIntersectionGeometryIndexKHR"; break;
        case SpvOpRayQueryGetIntersectionPrimitiveIndexKHR:                             return "SpvOpRayQueryGetIntersectionPrimitiveIndexKHR"; break;
        case SpvOpRayQueryGetIntersectionBarycentricsKHR:                               return "SpvOpRayQueryGetIntersectionBarycentricsKHR"; break;
        case SpvOpRayQueryGetIntersectionFrontFaceKHR:                                  return "SpvOpRayQueryGetIntersectionFrontFaceKHR"; break;
        case SpvOpRayQueryGetIntersectionCandidateAABBOpaqueKHR:                        return "SpvOpRayQueryGetIntersectionCandidateAABBOpaqueKHR"; break;
        case SpvOpRayQueryGetIntersectionObjectRayDirectionKHR:                         return "SpvOpRayQueryGetIntersectionObjectRayDirectionKHR"; break;
        case SpvOpRayQueryGetIntersectionObjectRayOriginKHR:                            return "SpvOpRayQueryGetIntersectionObjectRayOriginKHR"; break;
        case SpvOpRayQueryGetWorldRayDirectionKHR:                                      return "SpvOpRayQueryGetWorldRayDirectionKHR"; break;
        case SpvOpRayQueryGetWorldRayOriginKHR:                                         return "SpvOpRayQueryGetWorldRayOriginKHR"; break;
        case SpvOpRayQueryGetIntersectionObjectToWorldKHR:                              return "SpvOpRayQueryGetIntersectionObjectToWorldKHR"; break;
        case SpvOpRayQueryGetIntersectionWorldToObjectKHR:                              return "SpvOpRayQueryGetIntersectionWorldToObjectKHR"; break;
        case SpvOpAtomicFAddEXT:                                                        return "SpvOpAtomicFAddEXT"; break;
        case SpvOpTypeBufferSurfaceINTEL:                                               return "SpvOpTypeBufferSurfaceINTEL"; break;
        case SpvOpTypeStructContinuedINTEL:                                             return "SpvOpTypeStructContinuedINTEL"; break;
        case SpvOpConstantCompositeContinuedINTEL:                                      return "SpvOpConstantCompositeContinuedINTEL"; break;
        case SpvOpSpecConstantCompositeContinuedINTEL:                                  return "SpvOpSpecConstantCompositeContinuedINTEL"; break;
        case SpvOpCompositeConstructContinuedINTEL:                                     return "SpvOpCompositeConstructContinuedINTEL"; break;
        case SpvOpConvertFToBF16INTEL:                                                  return "SpvOpConvertFToBF16INTEL"; break;
        case SpvOpConvertBF16ToFINTEL:                                                  return "SpvOpConvertBF16ToFINTEL"; break;
        case SpvOpControlBarrierArriveINTEL:                                            return "SpvOpControlBarrierArriveINTEL"; break;
        case SpvOpControlBarrierWaitINTEL:                                              return "SpvOpControlBarrierWaitINTEL"; break;
        case SpvOpArithmeticFenceEXT:                                                   return "SpvOpArithmeticFenceEXT"; break;
        case SpvOpTaskSequenceCreateINTEL:                                              return "SpvOpTaskSequenceCreateINTEL"; break;
        case SpvOpTaskSequenceAsyncINTEL:                                               return "SpvOpTaskSequenceAsyncINTEL"; break;
        case SpvOpTaskSequenceGetINTEL:                                                 return "SpvOpTaskSequenceGetINTEL"; break;
        case SpvOpTaskSequenceReleaseINTEL:                                             return "SpvOpTaskSequenceReleaseINTEL"; break;
        case SpvOpTypeTaskSequenceINTEL:                                                return "SpvOpTypeTaskSequenceINTEL"; break;
        case SpvOpSubgroupBlockPrefetchINTEL:                                           return "SpvOpSubgroupBlockPrefetchINTEL"; break;
        case SpvOpSubgroup2DBlockLoadINTEL:                                             return "SpvOpSubgroup2DBlockLoadINTEL"; break;
        case SpvOpSubgroup2DBlockLoadTransformINTEL:                                    return "SpvOpSubgroup2DBlockLoadTransformINTEL"; break;
        case SpvOpSubgroup2DBlockLoadTransposeINTEL:                                    return "SpvOpSubgroup2DBlockLoadTransposeINTEL"; break;
        case SpvOpSubgroup2DBlockPrefetchINTEL:                                         return "SpvOpSubgroup2DBlockPrefetchINTEL"; break;
        case SpvOpSubgroup2DBlockStoreINTEL:                                            return "SpvOpSubgroup2DBlockStoreINTEL"; break;
        case SpvOpSubgroupMatrixMultiplyAccumulateINTEL:                                return "SpvOpSubgroupMatrixMultiplyAccumulateINTEL"; break;
        case SpvOpBitwiseFunctionINTEL:                                                 return "SpvOpBitwiseFunctionINTEL"; break;
        case SpvOpUntypedVariableLengthArrayINTEL:                                      return "SpvOpUntypedVariableLengthArrayINTEL"; break;
        case SpvOpConditionalExtensionINTEL:                                            return "SpvOpConditionalExtensionINTEL"; break;
        case SpvOpConditionalEntryPointINTEL:                                           return "SpvOpConditionalEntryPointINTEL"; break;
        case SpvOpConditionalCapabilityINTEL:                                           return "SpvOpConditionalCapabilityINTEL"; break;
        case SpvOpSpecConstantTargetINTEL:                                              return "SpvOpSpecConstantTargetINTEL"; break;
        case SpvOpSpecConstantArchitectureINTEL:                                        return "SpvOpSpecConstantArchitectureINTEL"; break;
        case SpvOpSpecConstantCapabilitiesINTEL:                                        return "SpvOpSpecConstantCapabilitiesINTEL"; break;
        case SpvOpConditionalCopyObjectINTEL:                                           return "SpvOpConditionalCopyObjectINTEL"; break;
        case SpvOpGroupIMulKHR:                                                         return "SpvOpGroupIMulKHR"; break;
        case SpvOpGroupFMulKHR:                                                         return "SpvOpGroupFMulKHR"; break;
        case SpvOpGroupBitwiseAndKHR:                                                   return "SpvOpGroupBitwiseAndKHR"; break;
        case SpvOpGroupBitwiseOrKHR:                                                    return "SpvOpGroupBitwiseOrKHR"; break;
        case SpvOpGroupBitwiseXorKHR:                                                   return "SpvOpGroupBitwiseXorKHR"; break;
        case SpvOpGroupLogicalAndKHR:                                                   return "SpvOpGroupLogicalAndKHR"; break;
        case SpvOpGroupLogicalOrKHR:                                                    return "SpvOpGroupLogicalOrKHR"; break;
        case SpvOpGroupLogicalXorKHR:                                                   return "SpvOpGroupLogicalXorKHR"; break;
        case SpvOpRoundFToTF32INTEL:                                                    return "SpvOpRoundFToTF32INTEL"; break;
        case SpvOpMaskedGatherINTEL:                                                    return "SpvOpMaskedGatherINTEL"; break;
        case SpvOpMaskedScatterINTEL:                                                   return "SpvOpMaskedScatterINTEL"; break;
        case SpvOpConvertHandleToImageINTEL:                                            return "SpvOpConvertHandleToImageINTEL"; break;
        case SpvOpConvertHandleToSamplerINTEL:                                          return "SpvOpConvertHandleToSamplerINTEL"; break;
        case SpvOpConvertHandleToSampledImageINTEL:                                     return "SpvOpConvertHandleToSampledImageINTEL"; break;
        case SpvOpMax:                                                                  return "SpvOpMax"; break;     
        default: return "unknown";
    }
}
inline std::string ToString(const SpvReflectUserType& user_type)
{
    switch (user_type)
    {
        case SPV_REFLECT_USER_TYPE_INVALID:                                 return "SPV_REFLECT_USER_TYPE_INVALID"; break;
        case SPV_REFLECT_USER_TYPE_CBUFFER:                                 return "SPV_REFLECT_USER_TYPE_CBUFFER"; break;
        case SPV_REFLECT_USER_TYPE_TBUFFER:                                 return "SPV_REFLECT_USER_TYPE_TBUFFER"; break;
        case SPV_REFLECT_USER_TYPE_APPEND_STRUCTURED_BUFFER:                return "SPV_REFLECT_USER_TYPE_APPEND_STRUCTURED_BUFFER"; break;
        case SPV_REFLECT_USER_TYPE_BUFFER:                                  return "SPV_REFLECT_USER_TYPE_BUFFER"; break;
        case SPV_REFLECT_USER_TYPE_BYTE_ADDRESS_BUFFER:                     return "SPV_REFLECT_USER_TYPE_BYTE_ADDRESS_BUFFER"; break;
        case SPV_REFLECT_USER_TYPE_CONSTANT_BUFFER:                         return "SPV_REFLECT_USER_TYPE_CONSTANT_BUFFER"; break;
        case SPV_REFLECT_USER_TYPE_CONSUME_STRUCTURED_BUFFER:               return "SPV_REFLECT_USER_TYPE_CONSUME_STRUCTURED_BUFFER"; break;
        case SPV_REFLECT_USER_TYPE_INPUT_PATCH:                             return "SPV_REFLECT_USER_TYPE_INPUT_PATCH"; break;
        case SPV_REFLECT_USER_TYPE_OUTPUT_PATCH:                            return "SPV_REFLECT_USER_TYPE_OUTPUT_PATCH"; break;
        case SPV_REFLECT_USER_TYPE_RASTERIZER_ORDERED_BUFFER:               return "SPV_REFLECT_USER_TYPE_RASTERIZER_ORDERED_BUFFER"; break;
        case SPV_REFLECT_USER_TYPE_RASTERIZER_ORDERED_BYTE_ADDRESS_BUFFER:  return "SPV_REFLECT_USER_TYPE_RASTERIZER_ORDERED_BYTE_ADDRESS_BUFFER"; break;
        case SPV_REFLECT_USER_TYPE_RASTERIZER_ORDERED_STRUCTURED_BUFFER:    return "SPV_REFLECT_USER_TYPE_RASTERIZER_ORDERED_STRUCTURED_BUFFER"; break;
        case SPV_REFLECT_USER_TYPE_RASTERIZER_ORDERED_TEXTURE_1D:           return "SPV_REFLECT_USER_TYPE_RASTERIZER_ORDERED_TEXTURE_1D"; break;
        case SPV_REFLECT_USER_TYPE_RASTERIZER_ORDERED_TEXTURE_1D_ARRAY:     return "SPV_REFLECT_USER_TYPE_RASTERIZER_ORDERED_TEXTURE_1D_ARRAY"; break;
        case SPV_REFLECT_USER_TYPE_RASTERIZER_ORDERED_TEXTURE_2D:           return "SPV_REFLECT_USER_TYPE_RASTERIZER_ORDERED_TEXTURE_2D"; break;
        case SPV_REFLECT_USER_TYPE_RASTERIZER_ORDERED_TEXTURE_2D_ARRAY:     return "SPV_REFLECT_USER_TYPE_RASTERIZER_ORDERED_TEXTURE_2D_ARRAY"; break;
        case SPV_REFLECT_USER_TYPE_RASTERIZER_ORDERED_TEXTURE_3D:           return "SPV_REFLECT_USER_TYPE_RASTERIZER_ORDERED_TEXTURE_3D"; break;
        case SPV_REFLECT_USER_TYPE_RAYTRACING_ACCELERATION_STRUCTURE:       return "SPV_REFLECT_USER_TYPE_RAYTRACING_ACCELERATION_STRUCTURE"; break;
        case SPV_REFLECT_USER_TYPE_RW_BUFFER:                               return "SPV_REFLECT_USER_TYPE_RW_BUFFER"; break;
        case SPV_REFLECT_USER_TYPE_RW_BYTE_ADDRESS_BUFFER:                  return "SPV_REFLECT_USER_TYPE_RW_BYTE_ADDRESS_BUFFER"; break;
        case SPV_REFLECT_USER_TYPE_RW_STRUCTURED_BUFFER:                    return "SPV_REFLECT_USER_TYPE_RW_STRUCTURED_BUFFER"; break;
        case SPV_REFLECT_USER_TYPE_RW_TEXTURE_1D:                           return "SPV_REFLECT_USER_TYPE_RW_TEXTURE_1D"; break;
        case SPV_REFLECT_USER_TYPE_RW_TEXTURE_1D_ARRAY:                     return "SPV_REFLECT_USER_TYPE_RW_TEXTURE_1D_ARRAY"; break;
        case SPV_REFLECT_USER_TYPE_RW_TEXTURE_2D:                           return "SPV_REFLECT_USER_TYPE_RW_TEXTURE_2D"; break;
        case SPV_REFLECT_USER_TYPE_RW_TEXTURE_2D_ARRAY:                     return "SPV_REFLECT_USER_TYPE_RW_TEXTURE_2D_ARRAY"; break;
        case SPV_REFLECT_USER_TYPE_RW_TEXTURE_3D:                           return "SPV_REFLECT_USER_TYPE_RW_TEXTURE_3D"; break;
        case SPV_REFLECT_USER_TYPE_STRUCTURED_BUFFER:                       return "SPV_REFLECT_USER_TYPE_STRUCTURED_BUFFER"; break;
        case SPV_REFLECT_USER_TYPE_SUBPASS_INPUT:                           return "SPV_REFLECT_USER_TYPE_SUBPASS_INPUT"; break;
        case SPV_REFLECT_USER_TYPE_SUBPASS_INPUT_MS:                        return "SPV_REFLECT_USER_TYPE_SUBPASS_INPUT_MS"; break;
        case SPV_REFLECT_USER_TYPE_TEXTURE_1D:                              return "SPV_REFLECT_USER_TYPE_TEXTURE_1D"; break;
        case SPV_REFLECT_USER_TYPE_TEXTURE_1D_ARRAY:                        return "SPV_REFLECT_USER_TYPE_TEXTURE_1D_ARRAY"; break;
        case SPV_REFLECT_USER_TYPE_TEXTURE_2D:                              return "SPV_REFLECT_USER_TYPE_TEXTURE_2D"; break;
        case SPV_REFLECT_USER_TYPE_TEXTURE_2D_ARRAY:                        return "SPV_REFLECT_USER_TYPE_TEXTURE_2D_ARRAY"; break;
        case SPV_REFLECT_USER_TYPE_TEXTURE_2DMS:                            return "SPV_REFLECT_USER_TYPE_TEXTURE_2DMS"; break;
        case SPV_REFLECT_USER_TYPE_TEXTURE_2DMS_ARRAY:                      return "SPV_REFLECT_USER_TYPE_TEXTURE_2DMS_ARRAY"; break;
        case SPV_REFLECT_USER_TYPE_TEXTURE_3D:                              return "SPV_REFLECT_USER_TYPE_TEXTURE_3D"; break;
        case SPV_REFLECT_USER_TYPE_TEXTURE_BUFFER:                          return "SPV_REFLECT_USER_TYPE_TEXTURE_BUFFER"; break;
        case SPV_REFLECT_USER_TYPE_TEXTURE_CUBE:                            return "SPV_REFLECT_USER_TYPE_TEXTURE_CUBE"; break;
        case SPV_REFLECT_USER_TYPE_TEXTURE_CUBE_ARRAY:                      return "SPV_REFLECT_USER_TYPE_TEXTURE_CUBE_ARRAY"; break;
        default: return "unknown";
    }
}

// Flag bits mask to string conversions: (typedefed as uint32_t, so same function name not possible)
inline std::string ToStringDecorationFlags(const SpvReflectDecorationFlags& decoration_flags)
{
    if (decoration_flags == SPV_REFLECT_DECORATION_NONE)
        return "SPV_REFLECT_DECORATION_NONE";
    std::ostringstream ss;
    bool first = true;
    auto append = [&](SpvReflectDecorationFlags bit, const char* name)
    {
        if (decoration_flags & bit)
        {
            if (!first)
                ss << " | ";
            first = false;
            ss << name << "(0x" << std::hex << static_cast<uint32_t>(bit) << std::dec << ")";
        }
    };
    append(SPV_REFLECT_DECORATION_BLOCK, "SPV_REFLECT_DECORATION_BLOCK");
    append(SPV_REFLECT_DECORATION_BUFFER_BLOCK, "SPV_REFLECT_DECORATION_BUFFER_BLOCK");
    append(SPV_REFLECT_DECORATION_ROW_MAJOR, "SPV_REFLECT_DECORATION_ROW_MAJOR");
    append(SPV_REFLECT_DECORATION_COLUMN_MAJOR, "SPV_REFLECT_DECORATION_COLUMN_MAJOR");
    append(SPV_REFLECT_DECORATION_BUILT_IN, "SPV_REFLECT_DECORATION_BUILT_IN");
    append(SPV_REFLECT_DECORATION_NOPERSPECTIVE, "SPV_REFLECT_DECORATION_NOPERSPECTIVE");
    append(SPV_REFLECT_DECORATION_FLAT, "SPV_REFLECT_DECORATION_FLAT");
    append(SPV_REFLECT_DECORATION_NON_WRITABLE, "SPV_REFLECT_DECORATION_NON_WRITABLE");
    append(SPV_REFLECT_DECORATION_RELAXED_PRECISION, "SPV_REFLECT_DECORATION_RELAXED_PRECISION");
    append(SPV_REFLECT_DECORATION_NON_READABLE, "SPV_REFLECT_DECORATION_NON_READABLE");
    append(SPV_REFLECT_DECORATION_PATCH, "SPV_REFLECT_DECORATION_PATCH");
    append(SPV_REFLECT_DECORATION_PER_VERTEX, "SPV_REFLECT_DECORATION_PER_VERTEX");
    append(SPV_REFLECT_DECORATION_PER_TASK, "SPV_REFLECT_DECORATION_PER_TASK");
    append(SPV_REFLECT_DECORATION_WEIGHT_TEXTURE, "SPV_REFLECT_DECORATION_WEIGHT_TEXTURE");
    append(SPV_REFLECT_DECORATION_BLOCK_MATCH_TEXTURE, "SPV_REFLECT_DECORATION_BLOCK_MATCH_TEXTURE");
    if (first)
        ss << "UNKNOWN_REFLECT_DECORATION_FLAG_BITS(0x" << std::hex << static_cast<uint32_t>(decoration_flags) << std::dec << ")";
    return ss.str();
}
inline std::string ToStringTypeFlags(const SpvReflectTypeFlags& type_flags)
{
    if (type_flags == SPV_REFLECT_TYPE_FLAG_UNDEFINED)
        return "SPV_REFLECT_TYPE_FLAG_UNDEFINED";
    std::ostringstream ss;
    bool first = true;
    auto append = [&](SpvReflectTypeFlags bit, const char* name)
    {
        if (type_flags & bit)
        {
            if (!first)
                ss << " | ";
            first = false;
            ss << name << "(0x" << std::hex << static_cast<uint32_t>(bit) << std::dec << ")";
        }
    };
    append(SPV_REFLECT_TYPE_FLAG_VOID, "SPV_REFLECT_TYPE_FLAG_VOID");
    append(SPV_REFLECT_TYPE_FLAG_BOOL, "SPV_REFLECT_TYPE_FLAG_BOOL");
    append(SPV_REFLECT_TYPE_FLAG_INT, "SPV_REFLECT_TYPE_FLAG_INT");
    append(SPV_REFLECT_TYPE_FLAG_FLOAT, "SPV_REFLECT_TYPE_FLAG_FLOAT");
    append(SPV_REFLECT_TYPE_FLAG_VECTOR, "SPV_REFLECT_TYPE_FLAG_VECTOR");
    append(SPV_REFLECT_TYPE_FLAG_MATRIX, "SPV_REFLECT_TYPE_FLAG_MATRIX");
    append(SPV_REFLECT_TYPE_FLAG_EXTERNAL_IMAGE, "SPV_REFLECT_TYPE_FLAG_EXTERNAL_IMAGE");
    append(SPV_REFLECT_TYPE_FLAG_EXTERNAL_SAMPLER, "SPV_REFLECT_TYPE_FLAG_EXTERNAL_SAMPLER");
    append(SPV_REFLECT_TYPE_FLAG_EXTERNAL_SAMPLED_IMAGE, "SPV_REFLECT_TYPE_FLAG_EXTERNAL_SAMPLED_IMAGE");
    append(SPV_REFLECT_TYPE_FLAG_EXTERNAL_BLOCK, "SPV_REFLECT_TYPE_FLAG_EXTERNAL_BLOCK");
    append(SPV_REFLECT_TYPE_FLAG_EXTERNAL_ACCELERATION_STRUCTURE, "SPV_REFLECT_TYPE_FLAG_EXTERNAL_ACCELERATION_STRUCTURE");
    append(SPV_REFLECT_TYPE_FLAG_EXTERNAL_MASK, "SPV_REFLECT_TYPE_FLAG_EXTERNAL_MASK");
    append(SPV_REFLECT_TYPE_FLAG_STRUCT, "SPV_REFLECT_TYPE_FLAG_STRUCT");
    append(SPV_REFLECT_TYPE_FLAG_ARRAY, "SPV_REFLECT_TYPE_FLAG_ARRAY");
    append(SPV_REFLECT_TYPE_FLAG_REF, "SPV_REFLECT_TYPE_FLAG_REF");
    if (first)
        ss << "UNKNOWN_REFLECT_TYPE_FLAG_BITS(0x" << std::hex << static_cast<uint32_t>(type_flags) << std::dec << ")";
    return ss.str();
}
inline std::string ToStringVariableFlags(const SpvReflectVariableFlags& flags)
{
    if (flags == SPV_REFLECT_VARIABLE_FLAGS_NONE)
        return "SPV_REFLECT_VARIABLE_FLAGS_NONE";
    std::ostringstream ss;
    bool first = true;
    auto append = [&](SpvReflectVariableFlags bit, const char* name)
    {
        if (flags & bit)
        {
            if (!first)
                ss << " | ";
            first = false;
            ss << name << "(0x" << std::hex << static_cast<uint32_t>(bit) << std::dec << ")";
        }
    };
    append(SPV_REFLECT_VARIABLE_FLAGS_UNUSED, "SPV_REFLECT_VARIABLE_FLAGS_UNUSED");
    append(SPV_REFLECT_VARIABLE_FLAGS_PHYSICAL_POINTER_COPY, "SPV_REFLECT_VARIABLE_FLAGS_PHYSICAL_POINTER_COPY");
    if (first)
        ss << "UNKNOWN_REFLECT_VARIABLE_FLAG_BITS(0x" << std::hex << static_cast<uint32_t>(flags) << std::dec << ")";
    return ss.str();
}



// Struct to string conversions:
inline std::string ToString(const SpvReflectNumericTraits& numeric, int indent = 0)
{
    std::string indentStr = std::string(indent, ' ');
    std::ostringstream ss;
    ss << indentStr << "numeric.scalar.width: " << numeric.scalar.width << "\n";
    ss << indentStr << "numeric.scalar.signedness: " << numeric.scalar.signedness << "\n";
    ss << indentStr << "numeric.vector.component_count: " << numeric.vector.component_count << "\n";
    ss << indentStr << "numeric.matrix.column_count: " << numeric.matrix.column_count << "\n";
    ss << indentStr << "numeric.matrix.row_count: " << numeric.matrix.row_count << "\n";
    ss << indentStr << "numeric.matrix.stride: " << numeric.matrix.stride << "\n";
    return ss.str();
}
inline std::string ToString(const SpvReflectImageTraits& image, int indent = 0)
{
    std::string indentStr = std::string(indent, ' ');
    std::ostringstream ss;
    ss << indentStr << "image.dim: " << ToString(image.dim) << "\n";
    ss << indentStr << "image.depth: " << image.depth << "\n";
    ss << indentStr << "image.arrayed: " << image.arrayed << "\n";
    ss << indentStr << "image.ms: " << image.ms << "\n";
    ss << indentStr << "image.sampled: " << image.sampled << "\n";
    ss << indentStr << "image.image_format: " << emberVulkanUtility::ToString((VkFormat)image.image_format) << "\n";
    return ss.str();
}
inline std::string ToString(const SpvReflectArrayTraits& array, int indent = 0)
{
    std::string indentStr = std::string(indent, ' ');
    std::ostringstream ss;
    ss << indentStr << "array.dims_count: " << array.dims_count << "\n";
    ss << indentStr << "array.stride: " << array.stride << "\n";
    for (int i = 0; i < array.dims_count; i++)
    {
        ss << "  " << indentStr << "array.dims[" << i << "]: " << array.dims[i] << "\n";
        ss << "  " << indentStr << "array.spec_constant_op_ids[" << i << "]: " << array.spec_constant_op_ids[i] << "\n";
    }
    return ss.str();
}
inline std::string ToString(const SpvReflectBindingArrayTraits& array, int indent = 0)
{
    std::string indentStr = std::string(indent, ' ');
    std::ostringstream ss;
    ss << indentStr << "array.dims_count: " << array.dims_count << "\n";
    for (int i = 0; i < array.dims_count; i++)
        ss << "  " << indentStr << "array.dims[" << i << "]: " << array.dims[i] << "\n";
    return ss.str();
}
inline std::string ToString(SpvReflectTypeDescription* type_description, int indent = 0)
{
    std::string indentStr = std::string(indent, ' ');
    std::ostringstream ss;
    ss << indentStr << "type_description.id: " << type_description->id << "\n";
    ss << indentStr << "type_description.op: " << ToString(type_description->op) << "\n";
    ss << indentStr << "type_description.type_name: " << (type_description->type_name ? type_description->type_name : "") << "\n";
    ss << indentStr << "type_description.struct_member_name: " << (type_description->struct_member_name ? type_description->struct_member_name : "") << "\n";
    ss << indentStr << "type_description.storage_class: " << type_description->storage_class << "\n";
    ss << indentStr << "type_description.type_flags: " << ToStringTypeFlags(type_description->type_flags) << "\n";
    ss << indentStr << "type_description.decoration_flags: " << ToStringDecorationFlags(type_description->decoration_flags) << "\n";
    ss << indentStr << "type_description.numeric:\n" << ToString(type_description->traits.numeric, indent + 2);
    ss << indentStr << "type_description.image:\n" << ToString(type_description->traits.image, indent + 2);
    ss << indentStr << "type_description.array:\n" << ToString(type_description->traits.array, indent + 2);
    return ss.str();
}
inline std::string ToString(const SpvReflectBlockVariable& block, int indent = 0)
{
    std::string indentStr = std::string(indent, ' ');
    std::ostringstream ss;
	ss << indentStr << "block.spirv_id: " << block.spirv_id << "\n";
    ss << indentStr << "block.name: " << (block.name ? block.name : "") << "\n";
	ss << indentStr << "block.offset: " << block.offset << "\n";
	ss << indentStr << "block.absolute_offset: " << block.absolute_offset << "\n";
	ss << indentStr << "block.size: " << block.size << "\n";
	ss << indentStr << "block.padded_size: " << block.padded_size << "\n";
    ss << indentStr << "block.decoration_flags: " << ToStringDecorationFlags(block.decoration_flags) << "\n";
    ss << indentStr << "block.numeric:\n" << ToString(block.numeric, indent + 2);
    ss << indentStr << "block.array:\n" << ToString(block.array, indent + 2);
    ss << indentStr << "block.flags: " << ToStringVariableFlags(block.flags) << "\n";
    ss << indentStr << "block.member_count: " << block.member_count << "\n";
    for (int i = 0; i < block.member_count; i++)
        ss << ToString(block.members[i], indent + 2);
    if (block.type_description)
        ss << indentStr << "block.type_description: " << ToString(block.type_description) << "\n";
    ss << indentStr << "block.word_offset.offset: " << block.word_offset.offset << "\n";
    return ss.str();
}
inline std::string ToString(SpvReflectDescriptorBinding* pBinding, int index = 0, int indent = 0)
{
    std::string indentStr = std::string(indent, ' ');
    std::ostringstream ss;
    ss << indentStr << "pBinding[" << index << "].spirv_id: " << pBinding->spirv_id << "\n";
    ss << indentStr << "pBinding[" << index << "].name: " << (pBinding->name ? pBinding->name : "") << "\n";
    ss << indentStr << "pBinding[" << index << "].binding: " << pBinding->binding << "\n";
    ss << indentStr << "pBinding[" << index << "].input_attachment_index: " << pBinding->input_attachment_index << "\n";
    ss << indentStr << "pBinding[" << index << "].set: " << pBinding->set << "\n";
    ss << indentStr << "pBinding[" << index << "].descriptor_type: " << ToString(pBinding->descriptor_type) << "\n";
    ss << indentStr << "pBinding[" << index << "].resource_type: " << ToString(pBinding->resource_type) << "\n";
    ss << indentStr << "pBinding[" << index << "].image:\n" << ToString(pBinding->image, indent + 2);
    ss << indentStr << "pBinding[" << index << "].block:\n" << ToString(pBinding->block, indent + 2);
    ss << indentStr << "pBinding[" << index << "].array:\n" << ToString(pBinding->array, indent + 2);
    ss << indentStr << "pBinding[" << index << "].count: " << pBinding->count << "\n";
    ss << indentStr << "pBinding[" << index << "].accessed: " << pBinding->accessed << "\n";
    ss << indentStr << "pBinding[" << index << "].uav_counter_id: " << pBinding->uav_counter_id << "\n";
    if (pBinding->uav_counter_binding)
        ss << indentStr << "pBinding[" << index << "].uav_counter_binding:\n" << ToString(pBinding->uav_counter_binding, indent + 2) << "\n";
    ss << indentStr << "pBinding[" << index << "].byte_address_buffer_offset_count: " << pBinding->byte_address_buffer_offset_count << "\n";
    if (pBinding->byte_address_buffer_offsets)
        ss << indentStr << "pBinding[" << index << "].byte_address_buffer_offsets: " << *pBinding->byte_address_buffer_offsets << "\n";
    if (pBinding->type_description)
        ss << indentStr << "pBinding[" << index << "].type_description:\n" << ToString(pBinding->type_description, indent + 2);
    ss << indentStr << "pBinding[" << index << "].word_offset.binding: " << pBinding->word_offset.binding << "\n";
    ss << indentStr << "pBinding[" << index << "].word_offset.set: " << pBinding->word_offset.set << "\n";
    ss << indentStr << "pBinding[" << index << "].decoration_flags: " << ToStringDecorationFlags(pBinding->decoration_flags) << "\n";
    ss << indentStr << "pBinding[" << index << "].user_type: " << ToString(pBinding->user_type) << "\n";
    return ss.str();
}



// Shader in/out struct reflection:
inline std::string ToString(SpvReflectInterfaceVariable* pVariable, int indent = 0)
{
    std::string indentStr = std::string(indent, ' ');
    std::ostringstream ss;
    ss << indentStr << "variable.spirv_id: " << pVariable->spirv_id << "\n";
    ss << indentStr << "variable.name: " << (pVariable->name ? pVariable->name : "") << "\n";
    ss << indentStr << "variable.location: " << pVariable->location << "\n";
    ss << indentStr << "variable.component: " << pVariable->component << "\n";
    ss << indentStr << "variable.storage_class: " << ToString(pVariable->storage_class) << "\n";
    ss << indentStr << "variable.semantic: " << (pVariable->semantic ? pVariable->semantic : "") << "\n";
    ss << indentStr << "variable.decoration_flags: " << ToStringDecorationFlags(pVariable->decoration_flags) << "\n";
	ss << indentStr << "variable.built_in: " << pVariable->built_in << "\n";
    ss << indentStr << "variable.numeric:\n" << ToString(pVariable->numeric, indent + 2);
    ss << indentStr << "variable.array:\n" << ToString(pVariable->array, indent + 2);
    ss << indentStr << "variable.member_count: " << pVariable->member_count << "\n";
    for (int i = 0; i < pVariable->member_count; i++)
        ss << ToString(&pVariable->members[i], indent + 2);
    ss << indentStr << "variable.format: " << ToString(pVariable->format) << "\n";
    if (pVariable->type_description)
        ss << indentStr << "variable.type_description:\n" << ToString(pVariable->type_description, indent + 2);
    ss << indentStr << "variable.word_offset.location: " << pVariable->word_offset.location << "\n\n\n";
    return ss.str();
}
// Descriptor set reflection:
inline std::string ToString(SpvReflectDescriptorSet* pDescriptorSet, int indent = 0)
{
    std::string indentStr = std::string(indent, ' ');
    std::ostringstream ss;
    ss << indentStr << "descriptorSet.set: " << pDescriptorSet->set << "\n";
    ss << indentStr << "descriptorSet.binding_count: " << pDescriptorSet->binding_count << "\n";
    for (int i = 0; i < pDescriptorSet->binding_count; i++)
    {
        ss << indentStr << "descriptorSet.bindings[" << i << "]:\n";
        SpvReflectDescriptorBinding* pDescriptorBinding = pDescriptorSet->bindings[i];
        ss << ToString(pDescriptorBinding, i, indent + 2);
        ss << "\n";
    }
    return ss.str();
}