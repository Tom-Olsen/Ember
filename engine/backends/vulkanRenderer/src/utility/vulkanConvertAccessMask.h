#pragma once
#include "commonAccessMask.h"
#include "vulkanAccessMask.h"



namespace vulkanRendererBackend
{
    //// AccessMask -> emberCommon::AccessMask:
	//const emberCommon::AccessMask& VulkanToCommonAccessMask(const AccessMask& accessMask);
	//const emberCommon::TopOfPipeAccessMask& VulkanToCommonTopOfPipeAccessMask(const AccessMask& accessMask);
	//const emberCommon::TransferAccessMask& VulkanToCommonTransferAccessMask(const AccessMask& accessMask);
	//const emberCommon::ComputeShaderAccessMask& VulkanToCommonComputeShaderAccessMask(const AccessMask& accessMask);
	//const emberCommon::VertexAttributeInputAccessMask& VulkanToCommonVertexAttributeInputAccessMask(const AccessMask& accessMask);
	//const emberCommon::VertexInputAccessMask& VulkanToCommonVertexInputAccessMask(const AccessMask& accessMask);
	//const emberCommon::VertexShaderAccessMask& VulkanToCommonVertexShaderAccessMask(const AccessMask& accessMask);
	//const emberCommon::EarlyFragmentTestAccessMask& VulkanToCommonEarlyFragmentTestAccessMask(const AccessMask& accessMask);
	//const emberCommon::FragmentShaderAccessMask& VulkanToCommonFragmentShaderAccessMask(const AccessMask& accessMask);
	//const emberCommon::ColorAttachmentOutputAccessMask& VulkanToCommonColorAttachmentOutputAccessMask(const AccessMask& accessMask);
	//const emberCommon::CopyAccessMask& VulkanToCommonCopyAccessMask(const AccessMask& accessMask);
	//const emberCommon::NoneAccessMask& VulkanToCommonNoneAccessMask(const AccessMask& accessMask);
	//const emberCommon::BottomOfPipeAccessMask& VulkanToCommonBottomOfPipeAccessMask(const AccessMask& accessMask);

    // emberCommon::AccessMask -> AccessMask:
    const AccessMask CommonToVulkanAccessMask(emberCommon::AccessMask accessMask);
}