#pragma once
#include "commonComputeBarrierFlag.h"
#include "vulkanAccessMask.h"



namespace vulkanRendererBackend
{
    [[nodiscard]] AccessMask ComputeBarrierFlagsToVulkanAccessMask(emberCommon::ComputeBarrierFlag barrierFlags);
}