#pragma once
#include "indexAllocator.h"
#include "vulkanGpuResourceHandle.h"
#include <cstdint>
#include <vector>



namespace vulkanRendererBackend
{
	class Buffer;
	class Texture;



	/// <summary>
	/// Tracks whether non-owning references to backend resources are still alive.
	/// This registry does not own the resources.
	/// </summary>
	template<typename T>
	class GpuResourceRegistry
	{
	private: // Structs:
		struct Slot
		{
			T* pResource;
			uint64_t generation;
		};

	private: // Members:
		std::vector<Slot> m_slots;
		emberDataStructures::IndexAllocator m_indexAllocator;

	public: // Methods:
		// Constructor/Destructor:
		GpuResourceRegistry();
		~GpuResourceRegistry();

		GpuResourceHandle Register(T* pResource);
		bool Unregister(const GpuResourceHandle& handle, const T* pResource);
		bool Rebind(const GpuResourceHandle& handle, T* pResource);
		T* TryGet(const GpuResourceHandle& handle) const;
	};
}