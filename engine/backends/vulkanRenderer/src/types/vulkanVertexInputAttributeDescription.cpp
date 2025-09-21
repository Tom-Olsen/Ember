#include "vulkanVertexInputAttributeDescription.h"
#include <sstream>



namespace vulkanRendererBackend
{
	std::string VertexInputAttributeDescription::ToString()
	{
		std::ostringstream ss;
		ss << "location: " << location << ", binding: " << binding << ", format: " << Formats::ToString(format) << ", offset: " << offset;
		return ss.str();
	}
}