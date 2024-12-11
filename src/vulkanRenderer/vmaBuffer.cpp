#include "vmaBuffer.h"
#include "vmaImage.h"
#include "vulkanCommand.h"
#include "vulkanContext.h"
#include "vulkanMacros.h"



// Constructors/Destructor:
VmaBuffer::VmaBuffer()
{
	m_buffer = VK_NULL_HANDLE;
	m_allocation = VK_NULL_HANDLE;
	m_pBufferInfo = nullptr;
	m_pAllocInfo = nullptr;
	m_pContext = nullptr;
}
VmaBuffer::VmaBuffer(VulkanContext* pContext, VkBufferCreateInfo* pBufferInfo, VmaAllocationCreateInfo* pAllocInfo)
{
	m_pContext = pContext;
	m_pBufferInfo = std::unique_ptr<VkBufferCreateInfo>(pBufferInfo);
	m_pAllocInfo = std::unique_ptr<VmaAllocationCreateInfo>(pAllocInfo);
	VKA(vmaCreateBuffer(m_pContext->GetVmaAllocator(), m_pBufferInfo.get(), m_pAllocInfo.get(), &m_buffer, &m_allocation, nullptr));
}
VmaBuffer::VmaBuffer(VmaBuffer&& other) noexcept
	: m_buffer(other.m_buffer),
	m_allocation(other.m_allocation),
	m_pBufferInfo(std::move(other.m_pBufferInfo)),
	m_pAllocInfo(std::move(other.m_pAllocInfo)),
	m_pContext(other.m_pContext)
{
	other.m_buffer = VK_NULL_HANDLE;
	other.m_allocation = nullptr;
	other.m_pContext = nullptr;
}
VmaBuffer& VmaBuffer::operator=(VmaBuffer&& other) noexcept
{
	if (this != &other)
	{
		// Free current resources
		m_buffer = VK_NULL_HANDLE;
		m_allocation = nullptr;

		// Move data from other
		m_buffer = other.m_buffer;
		m_allocation = other.m_allocation;
		m_pBufferInfo = std::move(other.m_pBufferInfo);
		m_pAllocInfo = std::move(other.m_pAllocInfo);
		m_pContext = other.m_pContext;

		// Reset other
		other.m_buffer = VK_NULL_HANDLE;
		other.m_allocation = nullptr;
		other.m_pContext = nullptr;
	}
	return *this;
}
VmaBuffer::~VmaBuffer()
{
	vmaDestroyBuffer(m_pContext->GetVmaAllocator(), m_buffer, m_allocation);
}



// Public methods:
// Getters:
const VkBuffer& VmaBuffer::GetVkBuffer() const
{
	return m_buffer;
}
const VmaAllocation& VmaBuffer::GetVmaAllocation() const
{
	return m_allocation;
}
const VkBufferCreateInfo* const VmaBuffer::GetVkBufferCreateInfo() const
{
	return m_pBufferInfo.get();
}
const VmaAllocationCreateInfo* const VmaBuffer::GetVmaAllocationCreateInfo() const
{
	return m_pAllocInfo.get();
}
uint64_t VmaBuffer::GetSize()
{
	return m_pBufferInfo->size;
}



// Static methods:
void VmaBuffer::CopyBufferToBuffer(VulkanContext* m_pContext, VmaBuffer* srcBuffer, VmaBuffer* dstBuffer, VkDeviceSize bufferSize, const VulkanQueue& queue)
{
	VulkanCommand command = VulkanCommand::BeginSingleTimeCommand(m_pContext, queue);

	// Queue copy command:
	VkBufferCopy copyRegion = {};
	copyRegion.srcOffset = 0;
	copyRegion.dstOffset = 0;
	copyRegion.size = bufferSize;
	vkCmdCopyBuffer(command.GetVkCommandBuffer(), srcBuffer->GetVkBuffer(), dstBuffer->GetVkBuffer(), 1, &copyRegion);

	VulkanCommand::EndSingleTimeCommand(m_pContext, command, queue);
}
void VmaBuffer::CopyBufferToImage(VulkanContext* m_pContext, VmaBuffer* srcBuffer, VmaImage* dstImage, const VulkanQueue& queue, uint32_t layerCount)
{
	VulkanCommand command = VulkanCommand::BeginSingleTimeCommand(m_pContext, queue);

	VkBufferImageCopy region = {};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;
	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = layerCount;
	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = dstImage->GetExtent();

	vkCmdCopyBufferToImage(
		command.GetVkCommandBuffer(),
		srcBuffer->GetVkBuffer(),
		dstImage->GetVkImage(),
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1,
		&region);

	VulkanCommand::EndSingleTimeCommand(m_pContext, command, queue);
}
VmaBuffer VmaBuffer::StagingBuffer(VulkanContext* m_pContext, uint64_t size, void* inputData)
{
	// Create buffer:
	VkBufferCreateInfo* pBufferInfo = new VkBufferCreateInfo();
	pBufferInfo->sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	pBufferInfo->size = size;
	pBufferInfo->usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	pBufferInfo->sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VmaAllocationCreateInfo* pAllocInfo = new VmaAllocationCreateInfo();
	pAllocInfo->usage = VMA_MEMORY_USAGE_CPU_ONLY; // no need for CPU-GPU synchronization
	pAllocInfo->flags = 0;
	pAllocInfo->requiredFlags = 0;
	pAllocInfo->preferredFlags = 0;

	VmaBuffer stagingBuffer = VmaBuffer(m_pContext, pBufferInfo, pAllocInfo);

	// Load data into buffer:
	void* data;
	VKA(vmaMapMemory(m_pContext->GetVmaAllocator(), stagingBuffer.GetVmaAllocation(), &data));
	memcpy(data, inputData, static_cast<size_t>(size));
	vmaUnmapMemory(m_pContext->GetVmaAllocator(), stagingBuffer.GetVmaAllocation());

	return stagingBuffer;
}
VmaBuffer VmaBuffer::StagingBuffer(VulkanContext* m_pContext, const std::vector<uint64_t>& sizes, const std::vector<void*>& inputDatas)
{
	// Check if sizes and inputDatas have the same size:
	if (sizes.size() != inputDatas.size())
		throw std::runtime_error("sizes and inputDatas must have the same size!");

	// Calculate total buffer size = sum of all buffer sizes:
	uint64_t totalSize = 0;
	for (uint64_t size : sizes)
		totalSize += size;

	// Create buffer:
	VkBufferCreateInfo* pBufferInfo = new VkBufferCreateInfo();
	pBufferInfo->sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	pBufferInfo->size = totalSize;
	pBufferInfo->usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	pBufferInfo->sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VmaAllocationCreateInfo* pAllocInfo = new VmaAllocationCreateInfo();
	pAllocInfo->usage = VMA_MEMORY_USAGE_CPU_ONLY; // no need for CPU-GPU synchronization
	pAllocInfo->flags = 0;
	pAllocInfo->requiredFlags = 0;
	pAllocInfo->preferredFlags = 0;

	VmaBuffer stagingBuffer = VmaBuffer(m_pContext, pBufferInfo, pAllocInfo);

	// Load data into buffer:
	void* data;
	VKA(vmaMapMemory(m_pContext->GetVmaAllocator(), stagingBuffer.GetVmaAllocation(), &data));
	uint64_t offset = 0;
	for (uint64_t i = 0; i < inputDatas.size(); i++)
	{
		if (inputDatas[i] != nullptr && sizes[i] > 0)
		{
			memcpy(static_cast<char*>(data) + offset, inputDatas[i], static_cast<size_t>(sizes[i]));
			offset += sizes[i];
		}
	}
	vmaUnmapMemory(m_pContext->GetVmaAllocator(), stagingBuffer.GetVmaAllocation());

	return stagingBuffer;
}