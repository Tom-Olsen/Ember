#include "vulkanVertexinputBindingDescription.h"
#include <sstream>



namespace vulkanRendererBackend
{
	std::string VertexInputBindingDescription::ToString()
	{
		std::ostringstream ss;
		ss << "binding: " << binding << ", stride: " << stride << ", inputRate: " << VertexInputRates::ToString(inputRate);
		return ss.str();
	}
}