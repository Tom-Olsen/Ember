#include "vmaBuffer.h"
#include "logger.h"
#include "vmaImage.h"
#include "vulkanCommand.h"
#include "vulkanContext.h"
#include "vulkanMacros.h"



namespace emberEngine
{
	// Static private members:
	uint32_t VmaBuffer::s_index = 0;



	// Constructors/Destructor:
	VmaBuffer::VmaBuffer(const std::string& name, const VkBufferCreateInfo& bufferInfo, const VmaAllocationCreateInfo& allocInfo)
	{
		m_name = name + std::to_string(s_index);
		s_index++;
		m_bufferInfo = bufferInfo;
		m_allocInfo = allocInfo;
		VKA(vmaCreateBuffer(VulkanContext::GetVmaAllocator(), &m_bufferInfo, &m_allocInfo, &m_buffer, &m_allocation, nullptr));

		#ifdef VALIDATION_LAYERS_ACTIVE
		VulkanContext::pAllocationTracker->AddVmaBuffer(this);
		#endif
	}
	VmaBuffer::VmaBuffer(VmaBuffer&& other) noexcept
		: m_buffer(other.m_buffer),
		m_allocation(other.m_allocation),
		m_bufferInfo(other.m_bufferInfo),
		m_allocInfo(other.m_allocInfo)
	{
		other.m_buffer = VK_NULL_HANDLE;
		other.m_allocation = nullptr;
	}
	VmaBuffer& VmaBuffer::operator=(VmaBuffer&& other) noexcept
	{
		if (this != &other)
		{
			// Free current resources
			m_buffer = VK_NULL_HANDLE;
			m_allocation = nullptr;

			// Move data from other
			m_name = other.m_name;
			m_buffer = other.m_buffer;
			m_allocation = other.m_allocation;
			m_bufferInfo = other.m_bufferInfo;
			m_allocInfo = other.m_allocInfo;

			// Reset other
			other.m_buffer = VK_NULL_HANDLE;
			other.m_allocation = nullptr;
		}
		return *this;
	}
	VmaBuffer::~VmaBuffer()
	{
		vmaDestroyBuffer(VulkanContext::GetVmaAllocator(), m_buffer, m_allocation);

		#ifdef VALIDATION_LAYERS_ACTIVE
		VulkanContext::pAllocationTracker->RemoveVmaBuffer(this);
		#endif
	}



	// Public methods:
	// Getters:
	const std::string& VmaBuffer::GetName() const
	{
		return m_name;
	}
	const VkBuffer& VmaBuffer::GetVkBuffer() const
	{
		return m_buffer;
	}
	const VmaAllocation& VmaBuffer::GetVmaAllocation() const
	{
		return m_allocation;
	}
	const VkBufferCreateInfo& VmaBuffer::GetVkBufferCreateInfo() const
	{
		return m_bufferInfo;
	}
	const VmaAllocationCreateInfo& VmaBuffer::GetVmaAllocationCreateInfo() const
	{
		return m_allocInfo;
	}
	uint64_t VmaBuffer::GetSize()
	{
		return m_bufferInfo.size;
	}



	// Static methods:
	void VmaBuffer::CopyBufferToBuffer(VmaBuffer* srcBuffer, VmaBuffer* dstBuffer, VkDeviceSize bufferSize, const VulkanQueue& queue)
	{
		VulkanCommand command = VulkanCommand::BeginSingleTimeCommand(queue);

		// Queue copy command:
		VkBufferCopy copyRegion = {};
		copyRegion.srcOffset = 0;
		copyRegion.dstOffset = 0;
		copyRegion.size = bufferSize;
		vkCmdCopyBuffer(command.GetVkCommandBuffer(), srcBuffer->GetVkBuffer(), dstBuffer->GetVkBuffer(), 1, &copyRegion);

		VulkanCommand::EndSingleTimeCommand(command, queue);
	}
	void VmaBuffer::CopyBufferToImage(VmaBuffer* srcBuffer, VmaImage* dstImage, const VulkanQueue& queue, uint32_t layerCount)
	{
		VulkanCommand command = VulkanCommand::BeginSingleTimeCommand(queue);

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

		VulkanCommand::EndSingleTimeCommand(command, queue);
	}
	VmaBuffer VmaBuffer::StagingBuffer(uint64_t size, void* inputData)
	{
		// Create buffer:
		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VmaAllocationCreateInfo allocInfo = {};
		allocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY; // no need for CPU-GPU synchronization
		allocInfo.flags = 0;
		allocInfo.requiredFlags = 0;
		allocInfo.preferredFlags = 0;

		VmaBuffer stagingBuffer = VmaBuffer("stagingBuffer", bufferInfo, allocInfo);

		// Load data into buffer:
		void* data;
		VKA(vmaMapMemory(VulkanContext::GetVmaAllocator(), stagingBuffer.GetVmaAllocation(), &data));
		memcpy(data, inputData, static_cast<size_t>(size));
		vmaUnmapMemory(VulkanContext::GetVmaAllocator(), stagingBuffer.GetVmaAllocation());

		return stagingBuffer;
	}
	VmaBuffer VmaBuffer::StagingBuffer(const std::vector<uint64_t>& sizes, const std::vector<void*>& inputDatas)
	{
		// Check if sizes and inputDatas have the same size:
		if (sizes.size() != inputDatas.size())
			throw std::runtime_error("sizes and inputDatas must have the same size!");

		// Calculate total buffer size = sum of all buffer sizes:
		uint64_t totalSize = 0;
		for (uint64_t size : sizes)
			totalSize += size;

		// Create buffer:
		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = totalSize;
		bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VmaAllocationCreateInfo allocInfo = {};
		allocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY; // no need for CPU-GPU synchronization
		allocInfo.flags = 0;
		allocInfo.requiredFlags = 0;
		allocInfo.preferredFlags = 0;

		VmaBuffer stagingBuffer = VmaBuffer("stagingBuffer", bufferInfo, allocInfo);

		// Load data into buffer:
		void* data;
		VKA(vmaMapMemory(VulkanContext::GetVmaAllocator(), stagingBuffer.GetVmaAllocation(), &data));
		uint64_t offset = 0;
		for (uint64_t i = 0; i < inputDatas.size(); i++)
		{
			if (inputDatas[i] != nullptr && sizes[i] > 0)
			{
				memcpy(static_cast<char*>(data) + offset, inputDatas[i], static_cast<size_t>(sizes[i]));
				offset += sizes[i];
			}
		}
		vmaUnmapMemory(VulkanContext::GetVmaAllocator(), stagingBuffer.GetVmaAllocation());

		return stagingBuffer;
	}
}