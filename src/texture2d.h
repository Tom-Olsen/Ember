#pragma once
#ifndef __INCLUDE_GUARD_texture_h__
#define __INCLUDE_GUARD_texture_h__
#include <memory>
#include <string>
#include "vulkanContext.h"
#include "vmaBuffer.h"
#include "vmaImage.h"
#include "stb_image.h"



class Texture2d
{
public: // Members:
	int width;
	int height;
	int channels;
	std::shared_ptr<VmaImage> image;
	std::string name;

private: // Members:
	VulkanContext* context;

public: // Methods:
	Texture2d(VulkanContext* context, char const* filename, std::string name);
	~Texture2d();

public: // Getters:
	uint64_t GetWidth();
	uint64_t GetHeight();

private: // Methods:
	void CreateImage(const VkImageSubresourceRange& subresourceRange, uint32_t width, uint32_t height);
	void TransitionImageLayout(const VkImageSubresourceRange& subresourceRange, VmaBuffer& stagingBuffer);
};



#endif // __INCLUDE_GUARD_texture_h__