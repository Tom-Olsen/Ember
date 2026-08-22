#include "vulkanGpuResourceRegistry.h"
#include "vulkanBuffer.h"
#include "vulkanShader.h"
#include "vulkanTexture.h"



namespace vulkanRendererBackend
{
	// Public methods:
	// Constructor/Destructor:
	template<typename T>
	GpuResourceRegistry<T>::GpuResourceRegistry()
	{

	}
	template<typename T>
	GpuResourceRegistry<T>::~GpuResourceRegistry()
	{
		
	}



	template<typename T>
	GpuResourceHandle GpuResourceRegistry<T>::Register(T* pResource)
	{
		uint32_t index = m_indexAllocator.Allocate();
		if (index == m_slots.size())
		{
			m_slots.push_back(Slot{pResource, 0});
			return GpuResourceHandle(index, 0);
		}
		else
		{
			Slot& slot = m_slots[index];
			slot.pResource = pResource;
			slot.generation++;
			return GpuResourceHandle(index, slot.generation);
		}
	}
	template<typename T>
	bool GpuResourceRegistry<T>::Unregister(const GpuResourceHandle& handle, const T* pResource)
	{
		if (!handle.IsValid() || handle.index >= m_slots.size())
			return false;

		Slot& slot = m_slots[handle.index];
		if (slot.generation != handle.generation || slot.pResource != pResource)
			return false;

		slot.pResource = nullptr;
		m_indexAllocator.Free(handle.index);
		return true;
	}
	template<typename T>
	bool GpuResourceRegistry<T>::Rebind(const GpuResourceHandle& handle, T* pResource)
	{
		if (!handle.IsValid() || handle.index >= m_slots.size())
			return false;

		Slot& slot = m_slots[handle.index];
		if (slot.generation != handle.generation || slot.pResource == nullptr)
			return false;

		slot.pResource = pResource;
		return true;
	}
	template<typename T>
	T* GpuResourceRegistry<T>::TryGet(const GpuResourceHandle& handle) const
	{
		if (!handle.IsValid() || handle.index >= m_slots.size())
			return nullptr;

		const Slot& slot = m_slots[handle.index];
		if (slot.generation != handle.generation)
			return nullptr;
		return slot.pResource;
	}



	template class GpuResourceRegistry<Buffer>;
	template class GpuResourceRegistry<Shader>;
	template class GpuResourceRegistry<Texture>;
}