#include "vulkanDescriptorPool.h"
#include "macros.h"



// Constructor:
VulkanDescriptorPool::VulkanDescriptorPool(VulkanLogicalDevice* logicalDevice, VulkanPipelineLayout* pipelineLayout, const std::vector<VulkanUniformBuffer> uniformBuffers, size_t framesInFlight)
{
	this->descriptorCount = static_cast<uint32_t>(framesInFlight);
	this->logicalDevice = logicalDevice;
	this->pipelineLayout = pipelineLayout;

	CreateDescriptorPool();
	CreateDescriptorSets();
	FillDescriptorSets(uniformBuffers);
}



// Destructor:
VulkanDescriptorPool::~VulkanDescriptorPool()
{
	vkDestroyDescriptorPool(logicalDevice->device, descriptorPool, nullptr);
}



// Private:
void VulkanDescriptorPool::CreateDescriptorPool()
{
	VkDescriptorPoolSize poolSize = {};
	poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize.descriptorCount = descriptorCount;// how many descriptors of this type may be allocated

	VkDescriptorPoolCreateInfo poolInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
	poolInfo.poolSizeCount = 1;		// how many types of descriptors (uniform buffers, storage buffers, sampled images, ...)
	poolInfo.pPoolSizes = &poolSize;
	poolInfo.maxSets = descriptorCount;		// maximum number of descriptor sets that may be allocated

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
void VulkanDescriptorPool::FillDescriptorSets(const std::vector<VulkanUniformBuffer> uniformBuffers)
{
	if (uniformBuffers.size() != descriptorCount)
		throw std::runtime_error((std::string)"size of uniformBuffers (" + std::to_string(uniformBuffers.size()) + (std::string)") does not match descriptor counr (" + std::to_string(descriptorCount) + (std::string)").");

	for (size_t i = 0; i < descriptorCount; i++)
	{
		VkDescriptorBufferInfo bufferInfo = {};
		bufferInfo.buffer = uniformBuffers[i].buffer->buffer;
		bufferInfo.offset = 0;
		bufferInfo.range = uniformBuffers[i].bufferSize;

		VkWriteDescriptorSet descriptorWrite = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
		descriptorWrite.dstSet = descriptorSets[i];
		descriptorWrite.dstBinding = 0;			// binding point in shader
		descriptorWrite.dstArrayElement = 0;	// can bind arrays, but we only bind one buffer => startIndex = 0
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrite.descriptorCount = 1;	// how many array elements get updateed
		descriptorWrite.pBufferInfo = &bufferInfo;	// used for buffer data
		descriptorWrite.pImageInfo = nullptr;		// uded for image data
		descriptorWrite.pTexelBufferView = nullptr; // uded for buffer views

		vkUpdateDescriptorSets(logicalDevice->device, 1, &descriptorWrite, 0, nullptr);
		// VkCopyDescriptorSet(...) allows for copying of descriptor sets
	}
}