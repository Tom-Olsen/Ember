#include "vulkanConvertAccessMask.h"
#include <unordered_map>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	//const emberCommon::AccessMask& VulkanToCommonAccessMask(const AccessMask& accessMask)
	//{
	//	if (dynamic_cast<const emberCommon::TopOfPipeAccessMask&>(accessMask)) return VulkanToCommonTopOfPipeAccessMask(accessMask);
	//	else if (dynamic_cast<const emberCommon::TransferAccessMask&>(accessMask)) return VulkanToCommonTransferAccessMask(accessMask);
	//	else if (dynamic_cast<const emberCommon::ComputeShaderAccessMask&>(accessMask)) return VulkanToCommonComputeShaderAccessMask(accessMask);
	//	else if (dynamic_cast<const emberCommon::VertexAttributeInputAccessMask&>(accessMask)) return VulkanToCommonVertexAttributeInputAccessMask(accessMask);
	//	else if (dynamic_cast<const emberCommon::VertexInputAccessMask&>(accessMask)) return VulkanToCommonVertexInputAccessMask(accessMask);
	//	else if (dynamic_cast<const emberCommon::VertexShaderAccessMask&>(accessMask)) return VulkanToCommonVertexShaderAccessMask(accessMask);
	//	else if (dynamic_cast<const emberCommon::EarlyFragmentTestAccessMask&>(accessMask)) return VulkanToCommonEarlyFragmentTestAccessMask(accessMask);
	//	else if (dynamic_cast<const emberCommon::FragmentShaderAccessMask&>(accessMask)) return VulkanToCommonFragmentShaderAccessMask(accessMask);
	//	else if (dynamic_cast<const emberCommon::ColorAttachmentOutputAccessMask&>(accessMask)) return VulkanToCommonColorAttachmentOutputAccessMask(accessMask);
	//	else if (dynamic_cast<const emberCommon::CopyAccessMask&>(accessMask)) return VulkanToCommonCopyAccessMask(accessMask);
	//	else if (dynamic_cast<const emberCommon::NoneAccessMask&>(accessMask)) return VulkanToCommonNoneAccessMask(accessMask);
	//	else if (dynamic_cast<const emberCommon::BottomOfPipeAccessMask&>(accessMask)) return VulkanToCommonBottomOfPipeAccessMask(accessMask);
	//}
	//const emberCommon::TopOfPipeAccessMask& VulkanToCommonTopOfPipeAccessMask(const AccessMask& accessMask)
	//{
	//	switch (accessMask)
	//	{
	//		case VK_ACCESS_2_NONE:	return emberCommon::AccessMasks::topOfPipe_none;
	//		default					return emberCommon::AccessMasks::none_none;
	//	}
	//}
	//const emberCommon::TransferAccessMask& VulkanToCommonTransferAccessMask(const AccessMask& accessMask)
	//{
	//	switch (accessMask)
	//	{
	//		case VK_ACCESS_2_TRANSFER_READ_BIT:						return emberCommon::AccessMasks::transfer_transferRead;
	//		case VK_ACCESS_2_TRANSFER_WRITE_BIT:					return emberCommon::AccessMasks::transfer_transferWrite;
	//		case VK_ACCESS_2_NONE:									return emberCommon::AccessMasks::transfer_none;
	//		default													return emberCommon::AccessMasks::none_none;
	//	}
	//}
	//const emberCommon::ComputeShaderAccessMask& VulkanToCommonComputeShaderAccessMask(const AccessMask& accessMask)
	//{
	//	switch (accessMask)
	//	{
	//		case VK_ACCESS_2_UNIFORM_READ_BIT:						return emberCommon::AccessMasks::computeShader_uniformRead;
	//		case VK_ACCESS_2_SHADER_READ_BIT:						return emberCommon::AccessMasks::computeShader_shaderRead;
	//		case VK_ACCESS_2_SHADER_WRITE_BIT:						return emberCommon::AccessMasks::computeShader_shaderWrite;
	//		case VK_ACCESS_2_SHADER_SAMPLED_READ_BIT:				return emberCommon::AccessMasks::computeShader_sampledRead;
	//		case VK_ACCESS_2_SHADER_STORAGE_READ_BIT:				return emberCommon::AccessMasks::computeShader_storageRead;
	//		case VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT:				return emberCommon::AccessMasks::computeShader_storageWrite;
	//		case VK_ACCESS_2_MEMORY_READ_BIT:						return emberCommon::AccessMasks::computeShader_memoryRead;
	//		case VK_ACCESS_2_MEMORY_WRITE_BIT:						return emberCommon::AccessMasks::computeShader_memoryWrite;
	//		case VK_ACCESS_2_ACCELERATION_STRUCTURE_READ_BIT_KHR:	return emberCommon::AccessMasks::computeShader_accelerationStructureRead;
	//		case VK_ACCESS_2_DESCRIPTOR_BUFFER_READ_BIT_EXT:		return emberCommon::AccessMasks::computeShader_descriptorBufferRead;
	//		case VK_ACCESS_2_NONE:									return emberCommon::AccessMasks::computeShader_none;
	//		default													return emberCommon::AccessMasks::none_none;
	//	}
	//}
	//const emberCommon::VertexAttributeInputAccessMask& VulkanToCommonVertexAttributeInputAccessMask(const AccessMask& accessMask)
	//{
	//	switch (accessMask)
	//	{
	//		case VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT:				return emberCommon::AccessMasks::vertexAttributeInput_vertexAttributeRead;
	//		case VK_ACCESS_2_NONE:									return emberCommon::AccessMasks::vertexAttributeInput_none;
	//		default													return emberCommon::AccessMasks::none_none;
	//	}
	//}
	//const emberCommon::VertexInputAccessMask& VulkanToCommonVertexInputAccessMask(const AccessMask& accessMask)
	//{
	//	switch (accessMask)
	//	{
	//		case VK_ACCESS_2_INDEX_READ_BIT:						return emberCommon::AccessMasks::vertexInput_indexRead;
	//		case VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT:				return emberCommon::AccessMasks::vertexInput_vertexAttributeRead;
	//		case VK_ACCESS_2_NONE:									return emberCommon::AccessMasks::vertexInput_none;
	//		default													return emberCommon::AccessMasks::none_none;
	//	}
	//}
	//const emberCommon::VertexShaderAccessMask& VulkanToCommonVertexShaderAccessMask(const AccessMask& accessMask)
	//{
	//	switch (accessMask)
	//	{
	//		case VK_ACCESS_2_UNIFORM_READ_BIT:						return emberCommon::AccessMasks::vertexShader_uniformRead;
	//		case VK_ACCESS_2_SHADER_READ_BIT:						return emberCommon::AccessMasks::vertexShader_shaderRead;
	//		case VK_ACCESS_2_SHADER_WRITE_BIT:						return emberCommon::AccessMasks::vertexShader_shaderWrite;
	//		case VK_ACCESS_2_SHADER_SAMPLED_READ_BIT:				return emberCommon::AccessMasks::vertexShader_sampledRead;
	//		case VK_ACCESS_2_SHADER_STORAGE_READ_BIT:				return emberCommon::AccessMasks::vertexShader_storageRead;
	//		case VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT:				return emberCommon::AccessMasks::vertexShader_storageWrite;
	//		case VK_ACCESS_2_ACCELERATION_STRUCTURE_READ_BIT_KHR:	return emberCommon::AccessMasks::vertexShader_accelerationStructureRead;
	//		case VK_ACCESS_2_DESCRIPTOR_BUFFER_READ_BIT_EXT:		return emberCommon::AccessMasks::vertexShader_descriptorBufferRead;
	//		case VK_ACCESS_2_NONE:									return emberCommon::AccessMasks::vertexShader_none;
	//		default													return emberCommon::AccessMasks::none_none;
	//	}
	//}
	//const emberCommon::EarlyFragmentTestAccessMask& VulkanToCommonEarlyFragmentTestAccessMask(const AccessMask& accessMask)
	//{
	//	switch (accessMask)
	//	{
	//		case VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT:		return emberCommon::AccessMasks::earlyFragmentTest_depthStencilAttachmentRead;
	//		case VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT:	return emberCommon::AccessMasks::earlyFragmentTest_depthStencilAttachmentWrite;
	//		case VK_ACCESS_2_NONE:									return emberCommon::AccessMasks::earlyFragmentTest_none;
	//		default													return emberCommon::AccessMasks::none_none;
	//	}
	//}
	//const emberCommon::FragmentShaderAccessMask& VulkanToCommonFragmentShaderAccessMask(const AccessMask& accessMask)
	//{
	//	switch (accessMask)
	//	{
	//		case VK_ACCESS_2_UNIFORM_READ_BIT:						return emberCommon::AccessMasks::fragmentShader_uniformRead;
	//		case VK_ACCESS_2_SHADER_READ_BIT:						return emberCommon::AccessMasks::fragmentShader_shaderRead;
	//		case VK_ACCESS_2_SHADER_WRITE_BIT:						return emberCommon::AccessMasks::fragmentShader_shaderWrite;
	//		case VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT:				return emberCommon::AccessMasks::fragmentShader_colorAttachmentRead;
	//		case VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT:		return emberCommon::AccessMasks::fragmentShader_depthStencilAttachmentRead;
	//		case VK_ACCESS_2_SHADER_SAMPLED_READ_BIT:				return emberCommon::AccessMasks::fragmentShader_sampledRead;
	//		case VK_ACCESS_2_SHADER_STORAGE_READ_BIT:				return emberCommon::AccessMasks::fragmentShader_storageRead;
	//		case VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT:				return emberCommon::AccessMasks::fragmentShader_storageWrite;
	//		case VK_ACCESS_2_ACCELERATION_STRUCTURE_READ_BIT_KHR:	return emberCommon::AccessMasks::fragmentShader_accelerationStructureRead;
	//		case VK_ACCESS_2_DESCRIPTOR_BUFFER_READ_BIT_EXT:		return emberCommon::AccessMasks::fragmentShader_descriptorBufferRead;
	//		case VK_ACCESS_2_NONE:									return emberCommon::AccessMasks::fragmentShader_none;
	//		default													return emberCommon::AccessMasks::none_none;
	//	}
	//}
	//const emberCommon::ColorAttachmentOutputAccessMask& VulkanToCommonColorAttachmentOutputAccessMask(const AccessMask& accessMask)
	//{
	//	switch (accessMask)
	//	{
	//		case VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT:				return emberCommon::AccessMasks::colorAttachmentOutput_colorAttachmentRead;
	//		case VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT:			return emberCommon::AccessMasks::colorAttachmentOutput_colorAttachmentWrite;
	//		case VK_ACCESS_2_NONE:									return emberCommon::AccessMasks::colorAttachmentOutput_none;
	//		default													return emberCommon::AccessMasks::none_none;
	//	}
	//}
	//const emberCommon::CopyAccessMask& VulkanToCommonCopyAccessMask(const AccessMask& accessMask)
	//{
	//	switch (accessMask)
	//	{
	//		case VK_ACCESS_2_TRANSFER_READ_BIT:						return emberCommon::AccessMasks::copy_transferRead;
	//		case VK_ACCESS_2_TRANSFER_WRITE_BIT:					return emberCommon::AccessMasks::copy_transferWrite;
	//		case VK_ACCESS_2_NONE:									return emberCommon::AccessMasks::copy_none;
	//		default													return emberCommon::AccessMasks::none_none;
	//	}
	//}
	//const emberCommon::NoneAccessMask& VulkanToCommonNoneAccessMask(const AccessMask& accessMask)
	//{
	//	switch (accessMask)
	//	{
	//		case VK_ACCESS_2_NONE:									return emberCommon::AccessMasks::none_none;
	//		default													return emberCommon::AccessMasks::none_none;
	//	}
	//}
	//const emberCommon::BottomOfPipeAccessMask& VulkanToCommonBottomOfPipeAccessMask(const AccessMask& accessMask)
	//{
	//	switch (accessMask)
	//	{
	//		case VK_ACCESS_2_NONE:									return emberCommon::AccessMasks::bottomOfPipe_none;
	//		default													return emberCommon::AccessMasks::none_none;
	//	}
	//}



	const AccessMask CommonToVulkanAccessMask(emberCommon::AccessMask accessMask)
	{
		const static std::unordered_map<emberCommon::AccessMaskFlag, AccessMask> map =
		{
			{ emberCommon::AccessMaskFlag::none, VK_ACCESS_2_NONE },
			{ emberCommon::AccessMaskFlag::transferRead, VK_ACCESS_2_TRANSFER_READ_BIT },
			{ emberCommon::AccessMaskFlag::transferWrite, VK_ACCESS_2_TRANSFER_WRITE_BIT },
			{ emberCommon::AccessMaskFlag::uniformRead, VK_ACCESS_2_UNIFORM_READ_BIT },
			{ emberCommon::AccessMaskFlag::shaderRead, VK_ACCESS_2_SHADER_READ_BIT },
			{ emberCommon::AccessMaskFlag::shaderWrite, VK_ACCESS_2_SHADER_WRITE_BIT },
			{ emberCommon::AccessMaskFlag::sampledRead, VK_ACCESS_2_SHADER_SAMPLED_READ_BIT },
			{ emberCommon::AccessMaskFlag::storageRead, VK_ACCESS_2_SHADER_STORAGE_READ_BIT },
			{ emberCommon::AccessMaskFlag::storageWrite, VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT },
			{ emberCommon::AccessMaskFlag::memoryRead, VK_ACCESS_2_MEMORY_READ_BIT },
			{ emberCommon::AccessMaskFlag::memoryWrite, VK_ACCESS_2_MEMORY_WRITE_BIT },
			{ emberCommon::AccessMaskFlag::accelerationStructureRead, VK_ACCESS_2_ACCELERATION_STRUCTURE_READ_BIT_KHR },
			{ emberCommon::AccessMaskFlag::descriptorBufferRead, VK_ACCESS_2_DESCRIPTOR_BUFFER_READ_BIT_EXT },
			{ emberCommon::AccessMaskFlag::vertexAttributeRead, VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT },
			{ emberCommon::AccessMaskFlag::indexRead, VK_ACCESS_2_INDEX_READ_BIT },
			{ emberCommon::AccessMaskFlag::depthStencilAttachmentRead, VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT },
			{ emberCommon::AccessMaskFlag::depthStencilAttachmentWrite, VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT },
			{ emberCommon::AccessMaskFlag::colorAttachmentRead, VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT },
			{ emberCommon::AccessMaskFlag::colorAttachmentWrite, VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT }
		};
		auto it = map.find(accessMask.flag);
		return it != map.end() ? it->second : AccessMasks::None::none;
	}
}