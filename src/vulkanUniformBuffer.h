#pragma once
#ifndef __INCLUDE_GUARD_vulkanUniformBuffer_h__
#define __INCLUDE_GUARD_vulkanUniformBuffer_h__
#include <vulkan/vulkan.h>
#include <memory>
#include "vulkanContext.h"
#include "vmaBuffer.h"
#include "vulkanCommand.h"
#include "glmTypes.h"



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
	alignas(16) Float4x4 modelMatrix;	// mesh local to world matrix
	alignas(16) Float4x4 viewMatrix;	// camera world to local matrix
	alignas(16) Float4x4 projMatrix;	// camera projection matrix
	alignas(16) Float4x4 normalMatrix;	// rotation matrix for normals and directions
	alignas(16) Float4x4 mvpMatrix;		// local to clip space matrix: (model * view * projection)

	RenderMatrizes()
	{
		modelMatrix = Float4x4(1.0f);
		viewMatrix = Float4x4(1.0f);
		projMatrix = Float4x4(1.0f);
		normalMatrix = Float4x4(1.0f);
		mvpMatrix = Float4x4(1.0f);
	}

	void UpdateMvpMatrix()
	{
		mvpMatrix = projMatrix * viewMatrix * modelMatrix;
	}
};



static const uint32_t MAX_D_LIGHTS = 2;
static const uint32_t MAX_S_LIGHTS = 5;
static const uint32_t MAX_P_LIGHTS = 10;
struct alignas(16) DLight
{
	alignas(4)  float intensity;		// Intensity of the light
	alignas(16) Float4 color;			// Color of the light
	alignas(16) Float4x4 viewMatrix;	// Light world to local matrix
	alignas(16) Float4x4 projMatrix;	// Light projection matrix
};
struct alignas(16) SLight
{
	alignas(4)  float intensity;		// Intensity of the light
	alignas(4)  float innerCone;		// Inner cone angle in radians (full intensity)
	alignas(4)  float outerCone;		// Outer cone angle in radians (fall-off)
	alignas(16) Float4 color;			// Color of the light
	alignas(16) Float4x4 viewMatrix;	// Light world to local matrix
	alignas(16) Float4x4 projMatrix;	// Light projection matrix
};
struct alignas(16) PLight
{
	alignas(4)  float intensity;		// Intensity of the light
	alignas(4)  float radius;			// Attenuation radius
	alignas(16) Float4 color;			// Color of the light
	alignas(16) Float4x4 viewMatrix;	// Light world to local matrix
	alignas(16) Float4x4 projMatrix;	// Light projection matrix
};
struct alignas(16) LightData
{
	DLight dLights[MAX_D_LIGHTS];		// Array of directional lights
	alignas(4) uint32_t dLightsCound;	// Number of active directional lights

	SLight sLights[MAX_S_LIGHTS];		// Array of spot lights
	alignas(4) uint32_t sLightsCound;	// Number of active spot lights

	PLight pLights[MAX_P_LIGHTS];		// Array of point lights
	alignas(4) uint32_t pLightsCound;	// Number of active point lights
};



#endif // __INCLUDE_GUARD_vulkanUniformBuffer_h__