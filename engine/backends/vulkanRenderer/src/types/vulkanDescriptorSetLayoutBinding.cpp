#include "vulkanDescriptorSetLayoutBinding.h"
#include <sstream>



namespace vulkanRendererBackend
{
	std::string DescriptorSetLayoutBinding::ToString()
	{
		std::ostringstream ss;
		ss << "binding: " << binding << ", descriptorType: " << DescriptorTypes::ToString(descriptorType)
		   << ", descriptorCount: " << descriptorCount << ", shaderStage: " << ShaderStages::ToString(shaderStage)
		   << ", sampler: " << pImmutableSamplers;
		return ss.str();
	}
}