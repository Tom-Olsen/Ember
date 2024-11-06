#pragma once
#ifndef __INCLUDE_GUARD_vulkanUniformBuffer_h__
#define __INCLUDE_GUARD_vulkanUniformBuffer_h__
#include <vulkan/vulkan.h>
#include <memory>
#include "vulkanContext.h"
#include "vmaBuffer.h"
#include "vulkanCommand.h"
#include "mathf.h"
#include "directionalLight.h"



class VulkanUniformBuffer
{
public: // Members:
	uint64_t size;	// in bytes
	std::shared_ptr<VmaBuffer> buffer; // TODO: using unique ptr here causes a crash, why?

private: // Members:
	VulkanContext* context;
	void* data;

public: // Methods:
	VulkanUniformBuffer(VulkanContext* context, uint64_t size);
	~VulkanUniformBuffer();
	template<typename T>
	void UpdateBuffer(const T& datastruct);
};



// Vulkan expects the data in structures to be aligned in memory in a specific way, for example:
// Scalars have to be aligned by 4 bytes.
// A vec2 must be aligned by 8 bytes
// A vec3 or vec4 must be aligned by 16 bytes
// A nested structure must be aligned by the base alignment of its members rounded up to a multiple of 16.
// A mat4 matrix must have the same alignment as a vec4.
// Full list of alignment requirements in: https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap15.html#interfaces-resources-layout
// 15.8.4. Offset and Stride Assignment / Alignment Requirements
struct alignas(16) RenderMatrizes
{
	alignas(16) Float4x4 modelMatrix;		// mesh local to world matrix
	alignas(16) Float4x4 viewMatrix;		// camera world to local matrix
	alignas(16) Float4x4 projMatrix;		// camera projection matrix
	alignas(16) Float4x4 normalMatrix;		// rotation matrix for normals and directions
	alignas(16) Float4x4 localToClipMatrix;	// local to camera clip space matrix: (projection * view * model)
	RenderMatrizes();
	void UpdateLocalToClipMatrix();
};



//static const uint32_t MAX_D_LIGHTS = 2;
//static const uint32_t MAX_S_LIGHTS = 5;
//static const uint32_t MAX_P_LIGHTS = 10;
//struct alignas(16) DLight
//{
//	alignas(16) Float4x4 viewMatrix;		// light world to local matrix
//	alignas(16) Float4x4 projMatrix;		// light projection matrix
//	alignas(16) Float4x4 worldToClipMatrix;	// world to light clip space matrix: (projection * view)
//	alignas(16) Float4 directionIntensity;	// direction (xyz) and intensity (w) of the light
//	alignas(16) Float4 color;				// light color
//};

struct alignas(16) LightData
{
	alignas(16) Float4x4 viewMatrix;		// light world to local matrix
	alignas(16) Float4x4 projMatrix;		// light projection matrix
	alignas(16) Float4x4 worldToClipMatrix;	// world to light clip space matrix: (projection * view)
	alignas(16) Float4 directionIntensity;	// direction (xyz) and intensity (w) of the light
	alignas(16) Float4 color;				// light color
	LightData(DirectionalLight* directionalLights);
	//alignas(16) DLight dLights[MAX_D_LIGHTS];		// Array of directional lights
	LightData();
	//LightData(std::array<DirectionalLight*, MAX_D_LIGHTS> directionalLights);
};



#endif // __INCLUDE_GUARD_vulkanUniformBuffer_h__