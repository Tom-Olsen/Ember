#pragma once
#ifndef __INCLUDE_GUARD_texture2d_h__
#define __INCLUDE_GUARD_texture2d_h__
#include <memory>
#include <string>
#include <filesystem>
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
	std::unique_ptr<VmaImage> image;
	std::string name;

protected: // Members:
	VulkanContext* context;

public: // Methods:
	Texture2d(VulkanContext* context, VmaImage* image, std::string name);
	Texture2d(VulkanContext* context, const std::filesystem::path& filePath, std::string name);
	~Texture2d();

public: // Getters:
	uint64_t GetWidth();
	uint64_t GetHeight();

protected: // Methods:
	Texture2d();
	void CreateImage(const VkImageSubresourceRange& subresourceRange, uint32_t width, uint32_t height, VkImageCreateFlagBits imageFlags);
	void TransitionImageLayout(const VkImageSubresourceRange& subresourceRange, VmaBuffer& stagingBuffer);
};



#endif // __INCLUDE_GUARD_texture2d_h__