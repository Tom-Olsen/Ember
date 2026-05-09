#pragma once
#include "commonComputeAccessMask.h"
#include "vulkanAccessMask.h"



namespace vulkanRendererBackend
{
    [[nodiscard]] AccessMask ComputeAccessFlagsCommonToVulkan(emberCommon::ComputeAccessFlag accessFlags);
}