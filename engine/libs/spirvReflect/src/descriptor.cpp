#include "descriptor.h"
#include "spirvReflectToString.h"
#include <sstream>



namespace emberSpirvReflect
{
    // Public methods:
    // Constructor:
    Descriptor::Descriptor(const SpvReflectDescriptorBinding* const pBinding, VkShaderStageFlagBits shaderStage)
    {
        name = pBinding->name;
        set = pBinding->set;
        binding = pBinding->binding;
        descriptorCount = pBinding->count;
        descriptorType = pBinding->descriptor_type;
        this->shaderStage = shaderStage;
    }



    bool Descriptor::IsEqual(const Descriptor& other)
    {
        // This is intendet to detect same descriptor in different stages:
        // => No comparison between shaderStages.
        bool isEqual = (name == other.name);
        isEqual &= (set == other.set);
        isEqual &= (binding == other.binding);
        isEqual &= (descriptorCount == other.descriptorCount);
        isEqual &= (descriptorType == other.descriptorType);
        return isEqual;
    }



    // Debugging:
    std::string Descriptor::ToString()
    {
        std::ostringstream ss;
        ss << "name: "<< name << ", ";
        ss << "set: " << set << ", ";
        ss << "binding: " << binding << ", ";
        ss << "descriptorCount: " << descriptorCount << ", ";
        ss << "descriptorType: " << emberSpirvReflect::ToString(descriptorType) << ", ";
        //ss << "shaderStage: " << shaderStage; // need acces to VkShaderStage ToString function.
        return ss.str();
    }
}