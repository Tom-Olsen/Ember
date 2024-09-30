#include "material.h"
#include "macros.h"
#include <fstream>
#include <vector>
#include <iostream> // TODO: Remove this



// Constructor:
Material::Material(const std::string& vertexShaderFilename, const std::string& fragmentShaderFilename)
{
	this->vertexShaderFilename = vertexShaderFilename;
	this->fragmentShaderFilename = fragmentShaderFilename;
}



// Destructor:
Material::~Material()
{

}



// Public:
void Material::PrintVertexShader()
{
    std::ifstream file(vertexShaderFilename + std::string(".hlsl"));
    if (!file.is_open())
        LOG_CRITICAL("Error opening shader file: {}", vertexShaderFilename + std::string(".hlsl"));

    std::string line;
    while (std::getline(file, line)) { // Read each line from the file
        std::cout << line << std::endl; // Print the line to the terminal
    }

    file.close(); // Close the file after reading
}
void Material::PrintFragmentShader()
{
    std::ifstream file(fragmentShaderFilename + std::string(".hlsl"));
    if (!file.is_open())
        LOG_CRITICAL("Error opening shader file: {}", fragmentShaderFilename + std::string(".hlsl"));

    std::string line;
    while (std::getline(file, line)) { // Read each line from the file
        std::cout << line << std::endl; // Print the line to the terminal
    }

    file.close(); // Close the file after reading
}



// Private:
VkShaderModule Material::CreateShaderModule(std::string shaderFilename, VulkanLogicalDevice* logicalDevice)
{
    // Open shader file:
    std::ifstream file(shaderFilename + std::string(".spv"), std::ios::binary);
    if (!file.is_open())
        LOG_CRITICAL("Error opening shader file: {}", shaderFilename + std::string(".spv"));

    // Get file size:
    file.seekg(0, std::ios::end);
    size_t fileSize = static_cast<size_t>(file.tellg());
    file.seekg(0, std::ios::beg);

    // Copy data to buffer:
    std::vector<char> buffer(fileSize);
    file.read(buffer.data(), fileSize);

    // Create shader module:
    VkShaderModuleCreateInfo createInfo =
    {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,   // type of this struct
        .pNext = nullptr,                                       // pointer to extension-specific data
        .flags = 0,                                             // flags (none)
        .codeSize = fileSize,                                   // size of code in bytes
        .pCode = (uint32_t*)(buffer.data())                     // pointer to code
    };
    VkShaderModule shaderModule;
    VKA(vkCreateShaderModule(logicalDevice->device, &createInfo, nullptr, &shaderModule));

    // Close file and return:
    file.close();
    return shaderModule;
}