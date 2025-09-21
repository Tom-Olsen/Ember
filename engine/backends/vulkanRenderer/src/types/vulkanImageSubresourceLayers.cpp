#include "vulkanImageSubresourceLayers.h"
#include <sstream>



namespace vulkanRendererBackend
{
	std::string ImageSubresourceLayers::ToString() const
	{
		std::ostringstream ss;
		ss << "aspectMask: " << ImageAspectFlags::ToString(aspectMask) << ", mipLevel: " << mipLevel
		   << ", baseArrayLayer: " << baseArrayLayer << ", layerCount: " << layerCount;
		return ss.str();
	}
}