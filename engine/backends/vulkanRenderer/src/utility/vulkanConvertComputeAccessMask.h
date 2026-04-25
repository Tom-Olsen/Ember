#pragma once
#include "commonComputeAccessMask.h"
#include "vulkanAccessMask.h"



namespace vulkanRendererBackend
{
    [[nodiscard]] vulkanRendererBackend::AccessMask ComputeAccessFlagsCommonToVulkan(emberCommon::ComputeAccessFlag accessFlags);
}