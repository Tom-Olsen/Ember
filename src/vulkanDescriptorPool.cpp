#include "vulkanDescriptorPool.h"
#include "macros.h"
#include <iostream>



// Constructor:
VulkanDescriptorPool::VulkanDescriptorPool(VulkanLogicalDevice* logicalDevice, VulkanPipelineLayout* pipelineLayout, const std::vector<VulkanUniformBuffer>& uniformBuffers, Texture2d* texture2d, size_t framesInFlight)
{
	this->descriptorCount = static_cast<uint32_t>(framesInFlight);
	this->logicalDevice = logicalDevice;
	this->pipelineLayout = pipelineLayout;

	CreateDescriptorPool();
	CreateDescriptorSets();
	FillDescriptorSets(uniformBuffers, texture2d);
}



// Destructor:
VulkanDescriptorPool::~VulkanDescriptorPool()
{
	vkDestroyDescriptorPool(logicalDevice->device, descriptorPool, nullptr);
}



// Private:
void VulkanDescriptorPool::CreateDescriptorPool()
{
	std::array<VkDescriptorPoolSize, 2> poolSizes{};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = descriptorCount;
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = descriptorCount;

	VkDescriptorPoolCreateInfo poolInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());	// how many types of descriptors (uniform buffers, storage buffers, sampled images, ...)
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = descriptorCount;	// maximum number of descriptor sets that may be allocated

	VKA(vkCreateDescriptorPool(logicalDevice->device, &poolInfo, nullptr, &descriptorPool));
}
void VulkanDescriptorPool::CreateDescriptorSets()
{
	std::vector<VkDescriptorSetLayout> layouts(descriptorCount, pipelineLayout->descriptorSetLayout); // same layout for all frames

	VkDescriptorSetAllocateInfo allocInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = descriptorCount;
	allocInfo.pSetLayouts = layouts.data();

	descriptorSets.resize(descriptorCount);
	VKA(vkAllocateDescriptorSets(logicalDevice->device, &allocInfo, descriptorSets.data()));
}
void VulkanDescriptorPool::FillDescriptorSets(const std::vector<VulkanUniformBuffer>& uniformBuffers, Texture2d* texture2d)
{
	if (uniformBuffers.size() != descriptorCount)
		throw std::runtime_error((std::string)"size of uniformBuffers (" + std::to_string(uniformBuffers.size()) + (std::string)") does not match descriptor count (" + std::to_string(descriptorCount) + (std::string)").");

	for (size_t i = 0; i < descriptorCount; i++)
	{
		VkDescriptorBufferInfo bufferInfo = {};
		bufferInfo.buffer = uniformBuffers[i].buffer->buffer;
		bufferInfo.offset = 0;
		bufferInfo.range = uniformBuffers[i].bufferSize;

		VkDescriptorImageInfo imageInfo = {};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = texture2d->imageView;
		imageInfo.sampler = texture2d->sampler->sampler;

		std::array< VkWriteDescriptorSet, 2> descriptorWrites{};
		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet = descriptorSets[i];
		descriptorWrites[0].dstBinding = 0;			// binding point in shader
		descriptorWrites[0].dstArrayElement = 0;	// can bind arrays, but we only bind one buffer => startIndex = 0
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[0].descriptorCount = 1;	// how many array elements get updateed
		descriptorWrites[0].pBufferInfo = &bufferInfo;	// used for buffer data
		descriptorWrites[0].pImageInfo = nullptr;		// uded for image data
		descriptorWrites[0].pTexelBufferView = nullptr; // uded for buffer views
		
		descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[1].dstSet = descriptorSets[i];
		descriptorWrites[1].dstBinding = 1;			// binding point in shader
		descriptorWrites[1].dstArrayElement = 0;	// can bind arrays, but we only bind one buffer => startIndex = 0
		descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrites[1].descriptorCount = 1;	// how many array elements get updateed
		descriptorWrites[1].pBufferInfo = nullptr;		// used for buffer data
		descriptorWrites[1].pImageInfo = &imageInfo;	// uded for image data
		descriptorWrites[1].pTexelBufferView = nullptr; // uded for buffer views

		vkUpdateDescriptorSets(logicalDevice->device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		// VkCopyDescriptorSet(...) allows for copying of descriptor sets
	}
}