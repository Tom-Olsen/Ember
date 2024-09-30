#pragma once
#ifndef __INCLUDE_GUARD_material_h__
#define __INCLUDE_GUARD_material_h__
#include <string>
#include "vulkanLogicalDevice.h"



class Material
{
public: // Members:
	std::string vertexShaderFilename;	// Path to vertex shader file, without extension.
	std::string fragmentShaderFilename;	// Path to fragment shader file, without extension.

private: // Members:

public: // Methods:
	Material(const std::string& vertexShaderFilename, const std::string& fragmentShaderFilename);
	~Material();
	void PrintVertexShader();
	void PrintFragmentShader();

private: // Methods:
	VkShaderModule CreateShaderModule(std::string shaderFilename, VulkanLogicalDevice* logicalDevice);
};



#endif // __INCLUDE_GUARD_material_h__