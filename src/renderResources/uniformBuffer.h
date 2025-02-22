#ifndef __INCLUDE_GUARD_uniformBuffer_h__
#define __INCLUDE_GUARD_uniformBuffer_h__
#include "vk_mem_alloc.h"
#include <vulkan/vulkan.h>
#include <memory>
#include <string>
#include <vector>



namespace emberEngine
{
	// Forward declarations:
	struct UniformBufferBlock;
	class VmaBuffer;
	struct VulkanContext;



	class UniformBuffer
	{
	private: // Members:
		std::shared_ptr<VmaBuffer> m_buffer;
		void* m_pDeviceData;
		std::vector<char> m_hostData;
		UniformBufferBlock* m_pUniformBufferBlock;
		VulkanContext* m_pContext;

	public: // Methods:
		UniformBuffer(VulkanContext* pContext, UniformBufferBlock* pUniformBufferBlock);
		~UniformBuffer();

		void UpdateBuffer();

		// Getters:
		uint32_t GetSize() const;
		const std::shared_ptr<VmaBuffer>& GetVmaBuffer() const;
		template<typename T>
		T GetValue(const std::string& memberName) const;
		template<typename T>
		T GetValue(const std::string& arrayName, uint32_t arrayIndex) const;
		template<typename T>
		T GetValue(const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName) const;
		template<typename T>
		T GetValue(const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex) const;

		// Setters:
		template<typename T>
		bool SetValue(const std::string& memberName, const T& value);
		template<typename T>
		bool SetValue(const std::string& arrayName, uint32_t arrayIndex, const T& value);
		template<typename T>
		bool SetValue(const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const T& value);
		template<typename T>
		bool SetValue(const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const T& value);

	private: // Methods:
		template<typename T>
		bool CheckAndUpdateData(const T& value, uint32_t offset, uint32_t size);
		template<typename T>
		T GetData(uint32_t offset, uint32_t size) const;
	};
}



#endif // __INCLUDE_GUARD_uniformBuffer_h__