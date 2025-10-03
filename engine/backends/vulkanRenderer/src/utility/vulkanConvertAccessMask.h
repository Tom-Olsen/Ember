#pragma once
#include "commonAccessMask.h"
#include "vulkanAccessMask.h"



namespace vulkanRendererBackend
{
    // emberCommon::AccessMask -> vulkanRendererBackend::AccessMask:
    [[nodiscard]] vulkanRendererBackend::AccessMask AccessMaskCommonToVulkan(emberCommon::AccessMask accessMask);
}