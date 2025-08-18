#ifndef __INCLUDE_GUARD_uniformBuffer_h__
#define __INCLUDE_GUARD_uniformBuffer_h__
#include "buffer.h"
#include <string>
#include <vector>



namespace emberEngine
{
	// Forward declarations:
	namespace vulkanBackend
	{
		struct UniformBufferBlock;
	}



	/// <summary>
	/// Buffer specialization: <para/>
	/// -VkBufferUsageFlags			= uniform <para/>
	/// -VmaMemoryUsage				= prefer host <para/>
	/// -VmaAllocationCreateFlags	= mapped, host access sequential write
	/// </summary>
	class UniformBuffer : public Buffer
	{
	private: // Members:
		void* m_pDeviceData;
		std::vector<char> m_hostData;
		vulkanBackend::UniformBufferBlock* m_pUniformBufferBlock;

	public: // Methods:
		UniformBuffer(vulkanBackend::UniformBufferBlock* pUniformBufferBlock, std::string name);
		~UniformBuffer();

		void UpdateBuffer();

		// Getters:
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