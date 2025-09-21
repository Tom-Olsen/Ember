#include "vulkanImageSubresourceRange.h"
#include <sstream>



namespace vulkanRendererBackend
{
    std::string ImageSubresourceRange::ToString() const
    {
		std::ostringstream ss;
		ss << "aspectMask: " << ImageAspectFlags::ToString(aspectMask) << ", baseMipLevel: " << baseMipLevel
		   << ", levelCount: " << levelCount << ", baseArrayLayer: " << baseArrayLayer << ", layerCount: " << layerCount;
		return ss.str();
    }
}