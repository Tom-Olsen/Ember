#pragma once



// Forward declarations:
typedef struct VmaAllocator_T* VmaAllocator;



namespace vulkanRendererBackend
{
	// Forward declarations:
	class Instance;
	class LogicalDevice;
	class PhysicalDevice;



	class MemoryAllocator
	{
	private: // Members:
		VmaAllocator m_pAllocator;

	public: // Methods:
		MemoryAllocator(Instance* pInstance, LogicalDevice* pLogicalDevice, PhysicalDevice* pPhysicalDevice);
		~MemoryAllocator();

		// Non-copyable:
		MemoryAllocator(const MemoryAllocator&) = delete;
		MemoryAllocator& operator=(const MemoryAllocator&) = delete;

		// Movable:
		MemoryAllocator(MemoryAllocator&& other) noexcept;
		MemoryAllocator& operator=(MemoryAllocator&& other) noexcept;

		const VmaAllocator& GetVmaAllocator() const;

	private: // Methods:
		void Cleanup();
		void MoveFrom(MemoryAllocator& other) noexcept;
	};
}