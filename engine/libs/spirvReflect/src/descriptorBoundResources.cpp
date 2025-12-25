#include "descriptorBoundResources.h"
#include <sstream>



namespace emberSpirvReflect
{
    // Public methods:
    // Constructor:
    DescriptorBoundResources::DescriptorBoundResources()
    {

    }



    // Debuggig:
    std::string DescriptorBoundResources::ToString() const
    {
        std::ostringstream ss;
        //ss << "descriptorSetBindingNames:(" << bindingCount << ")\n";
        //ss << "\nvkDescriptorSetLayoutBindings:\n";
        //for (int i = 0; i < bindingCount; i++)
        //{
        //    ss << "  " << descriptorSetBindingNames[i] << ": ";
        //    ss << "binding=" << vkDescriptorSetLayoutBindings[i].binding << ", ";
        //    ss << "count=" << vkDescriptorSetLayoutBindings[i].descriptorCount << ", ";
        //    ss << "type=" << DescriptorTypes::ToString(vkDescriptorSetLayoutBindings[i].descriptorType) << ", ";
        //    ss << "stage=" << ShaderStages::ToString(vkDescriptorSetLayoutBindings[i].shaderStage);
        //
        //    auto it = uniformBufferBlockMap.find(descriptorSetBindingNames[i]);
        //    if (it != uniformBufferBlockMap.end())
        //    {
        //        ss << "\n" << it->second->ToString(4);
        //        continue;
        //    }
        //
        //    auto itt = sampleViewTypeMap.find(descriptorSetBindingNames[i]);
        //    if (itt != sampleViewTypeMap.end())
        //    {
        //        ss << ", viewType=" << ImageViewTypes::ToString(itt->second) << "\n";
        //        continue;
        //    }
        //
        //    auto ittt = storageViewTypeMap.find(descriptorSetBindingNames[i]);
        //    if (ittt != storageViewTypeMap.end())
        //    {
        //        ss << ", viewType=" << ImageViewTypes::ToString(ittt->second) << "\n";
        //        continue;
        //    }
        //
        //    ss << "\n"; // extra newline for samplers (they don't add one themselves).
        //}
        return ss.str();
    }
}