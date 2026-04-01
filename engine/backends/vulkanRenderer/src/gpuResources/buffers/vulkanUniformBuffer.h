#pragma once
#include "bufferLayout.h"
#include "emberMath.h"
#include "vulkanBuffer.h"
#include <string>
#include <vector>



namespace vulkanRendererBackend
{
	/// <summary>
	/// Manages cpu data and a gpu ring buffer that can fit framesInFlight many copies of the cpu data. <para/>
	/// Buffer specialization: <para/>
	/// -VkBufferUsageFlags			= uniform <para/>
	/// -VmaMemoryUsage				= prefer host <para/>
	/// -VmaAllocationCreateFlags	= mapped, host access sequential write
	/// </summary>
	class UniformBuffer : public Buffer
	{
	friend class DescriptorSetBinding;	// DescriptorSetBinding needs access to templated SetValue(...) methods.

	private: // Members:
		uint8_t* m_pDeviceData;
		std::vector<char> m_hostData;
		uint32_t m_alignedSize;
		emberBufferLayout::BufferLayout m_bufferLayout;

	public: // Methods:
		// Constructor/Destructor:
		UniformBuffer(emberBufferLayout::BufferLayout bufferLayout);
		~UniformBuffer();

		// Non-copyable:
		UniformBuffer(const UniformBuffer&) = delete;
		UniformBuffer& operator=(const UniformBuffer&) = delete;

		// Movable:
		UniformBuffer(UniformBuffer&&) noexcept = default;
		UniformBuffer& operator=(UniformBuffer&&) noexcept = default;

		// Update buffer:
		void UpdateBuffer(uint32_t frameIndex);

		// Getters:
		uint32_t GetAlignedSubBufferSize();
		uint32_t GetBufferOffset(uint32_t frameIndex);
		// Simple members:
		int GetInt(const std::string& memberName) const;
		bool GetBool(const std::string& memberName) const;
		float GetFloat(const std::string& memberName) const;
		Float2 GetFloat2(const std::string& memberName) const;
		Float3 GetFloat3(const std::string& memberName) const;
		Float4 GetFloat4(const std::string& memberName) const;
		Float4x4 GetFloat4x4(const std::string& memberName) const;
		// Array members:
		int GetInt(const std::string& arrayName, uint32_t arrayIndex) const;
		bool GetBool(const std::string& arrayName, uint32_t arrayIndex) const;
		float GetFloat(const std::string& arrayName, uint32_t arrayIndex) const;
		Float2 GetFloat2(const std::string& arrayName, uint32_t arrayIndex) const;
		Float3 GetFloat3(const std::string& arrayName, uint32_t arrayIndex) const;
		Float4 GetFloat4(const std::string& arrayName, uint32_t arrayIndex) const;
		Float4x4 GetFloat4x4(const std::string& arrayName, uint32_t arrayIndex) const;
		// Array of structs:
		int GetInt(const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName) const;
		bool GetBool(const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName) const;
		float GetFloat(const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName) const;
		Float2 GetFloat2(const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName) const;
		Float3 GetFloat3(const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName) const;
		Float4 GetFloat4(const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName) const;
		Float4x4 GetFloat4x4(const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName) const;
		// Array of arrays:
		int GetInt(const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex) const;
		bool GetBool(const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex) const;
		float GetFloat(const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex) const;
		Float2 GetFloat2(const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex) const;
		Float3 GetFloat3(const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex) const;
		Float4 GetFloat4(const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex) const;
		Float4x4 GetFloat4x4(const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex) const;

		// Setters:
		// Simple members:
		bool SetInt(const std::string& memberName, int value);
		bool SetBool(const std::string& memberName, bool value);
		bool SetFloat(const std::string& memberName, float value);
		bool SetFloat2(const std::string& memberName, const Float2& value);
		bool SetFloat3(const std::string& memberName, const Float3& value);
		bool SetFloat4(const std::string& memberName, const Float4& value);
		bool SetFloat4x4(const std::string& memberName, const Float4x4& value);
		// Array members:
		bool SetInt(const std::string& arrayName, uint32_t arrayIndex, int value);
		bool SetBool(const std::string& arrayName, uint32_t arrayIndex, bool value);
		bool SetFloat(const std::string& arrayName, uint32_t arrayIndex, float value);
		bool SetFloat2(const std::string& arrayName, uint32_t arrayIndex, const Float2& value);
		bool SetFloat3(const std::string& arrayName, uint32_t arrayIndex, const Float3& value);
		bool SetFloat4(const std::string& arrayName, uint32_t arrayIndex, const Float4& value);
		bool SetFloat4x4(const std::string& arrayName, uint32_t arrayIndex, const Float4x4& value);
		// Array of structs members:
		bool SetInt(const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, int value);
		bool SetBool(const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, bool value);
		bool SetFloat(const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, float value);
		bool SetFloat2(const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const Float2& value);
		bool SetFloat3(const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const Float3& value);
		bool SetFloat4(const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const Float4& value);
		bool SetFloat4x4(const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const Float4x4& value);
		// Array of arrays members:
		bool SetInt(const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, int value);
		bool SetBool(const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, bool value);
		bool SetFloat(const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, float value);
		bool SetFloat2(const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const Float2& value);
		bool SetFloat3(const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const Float3& value);
		bool SetFloat4(const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const Float4& value);
		bool SetFloat4x4(const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const Float4x4& value);

	private: // Methods:
		// Getter templates, used in actual getters:
		template<typename T>
		T GetValue(const std::string& memberName) const;
		template<typename T>
		T GetValue(const std::string& arrayName, uint32_t arrayIndex) const;
		template<typename T>
		T GetValue(const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName) const;
		template<typename T>
		T GetValue(const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex) const;

		// Setter templates, used in actual setters:
		template<typename T>
		bool SetValue(const std::string& memberName, const T& value);
		template<typename T>
		bool SetValue(const std::string& arrayName, uint32_t arrayIndex, const T& value);
		template<typename T>
		bool SetValue(const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const T& value);
		template<typename T>
		bool SetValue(const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const T& value);

		// Templates for interal use:
		template<typename T>
		T GetData(uint32_t offset, uint32_t size) const;
		template<typename T>
		bool CheckAndUpdateData(const T& value, uint32_t offset, uint32_t size);
	};
}