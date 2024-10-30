#include "vmaBuffer.h"
#include "vulkanCommand.h"
#include "vulkanHelper.h"
#include "memory"
#include "macros.h"



// Constructor:
VmaBuffer::VmaBuffer(VulkanContext* context, const VkBufferCreateInfo& bufferInfo, const VmaAllocationCreateInfo& allocInfo)
{
	this->context = context;
	this->bufferInfo = bufferInfo;
	this->allocInfo = allocInfo;
	VKA(vmaCreateBuffer(context->Allocator(), &bufferInfo, &allocInfo, &buffer, &allocation, nullptr));
}



// Destructor:
VmaBuffer::~VmaBuffer()
{
	vmaDestroyBuffer(context->Allocator(), buffer, allocation);
}



// Public methods:
uint64_t VmaBuffer::GetSize()
{
	return this->bufferInfo.size;
}



// Static methods:
void VmaBuffer::CopyBufferToBuffer(VulkanContext* context, VmaBuffer* srcBuffer, VmaBuffer* dstBuffer, VkDeviceSize bufferSize, const VulkanQueue& queue)
{
	VulkanCommand command = VulkanHelper::BeginSingleTimeCommand(context, queue);

	// Queue copy command:
	VkBufferCopy copyRegion = {};
	copyRegion.srcOffset = 0;
	copyRegion.dstOffset = 0;
	copyRegion.size = bufferSize;
	vkCmdCopyBuffer(command.buffer, srcBuffer->buffer, dstBuffer->buffer, 1, &copyRegion);

	VulkanHelper::EndSingleTimeCommand(context, command, queue);
}
VmaBuffer VmaBuffer::StagingBuffer(VulkanContext* context, uint64_t size, void* inputData)
{
	// Create buffer:
	VkBufferCreateInfo bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
	bufferInfo.size = size;
	bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VmaAllocationCreateInfo allocInfo = {};
	allocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY; // no need for CPU-GPU synchronization
	allocInfo.flags = 0;
	allocInfo.requiredFlags = 0;
	allocInfo.preferredFlags = 0;

	VmaBuffer stagingBuffer = VmaBuffer(context, bufferInfo, allocInfo);

	// Load data into buffer:
	void* data;
	VKA(vmaMapMemory(context->Allocator(), stagingBuffer.allocation, &data));
	memcpy(data, inputData, static_cast<size_t>(size));
	vmaUnmapMemory(context->Allocator(), stagingBuffer.allocation);

	return stagingBuffer;
}
VmaBuffer VmaBuffer::StagingBuffer(VulkanContext* context, const std::vector<uint64_t>& sizes, const std::vector<void*>& inputDatas)
{
	// Check if sizes and inputDatas have the same size:
	if (sizes.size() != inputDatas.size())
		throw std::runtime_error("sizes and inputDatas must have the same size!");

	// Calculate total buffer size = sum of all buffer sizes:
	uint64_t totalSize = 0;
	for (uint64_t size : sizes)
		totalSize += size;

	// Create buffer:
	VkBufferCreateInfo bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
	bufferInfo.size = totalSize;
	bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VmaAllocationCreateInfo allocInfo = {};
	allocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY; // no need for CPU-GPU synchronization
	allocInfo.flags = 0;
	allocInfo.requiredFlags = 0;
	allocInfo.preferredFlags = 0;

	VmaBuffer stagingBuffer = VmaBuffer(context, bufferInfo, allocInfo);

	// Load data into buffer:
	void* data;
	VKA(vmaMapMemory(context->Allocator(), stagingBuffer.allocation, &data));
	uint64_t offset = 0;
	for (uint64_t i = 0; i < inputDatas.size(); i++)
	{
		if (inputDatas[i] != nullptr && sizes[i] > 0)
		{
			memcpy(static_cast<char*>(data) + offset, inputDatas[i], static_cast<size_t>(sizes[i]));
			offset += sizes[i];
		}
	}
	vmaUnmapMemory(context->Allocator(), stagingBuffer.allocation);

	return stagingBuffer;
}