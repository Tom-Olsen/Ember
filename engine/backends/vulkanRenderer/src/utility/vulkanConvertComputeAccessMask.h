#pragma once
#include "iCompute.h"
#include "vulkanAccessMask.h"



namespace vulkanRendererBackend
{
    [[nodiscard]] AccessMask ComputeBarrierFlagsToVulkanAccessMask(emberBackendInterface::ComputeBarrierFlag barrierFlags);
}