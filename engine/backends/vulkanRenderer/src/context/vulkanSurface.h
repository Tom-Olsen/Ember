#pragma once
#include "emberMath.h"
#include "vulkanColorSpace.h"
#include "vulkanFormat.h"
#include "vulkanPresentMode.h"
#include <memory>
#include <vector>



// Forward declerations:
struct VkExtent2D;
typedef struct VkSurfaceKHR_T* VkSurfaceKHR;
namespace emberBackendInterface
{
	class IWindow;
}



namespace vulkanRendererBackend
{
	// Forward declarations:
	class Instance;
	class PhysicalDevice;



	struct SurfaceFormat
	{
		Format format;
		ColorSpace colorSpace;
	};



	class Surface
	{
	private: // Members:
		VkSurfaceKHR m_surface;
		SurfaceFormat m_surfaceFormat;
		PresentMode m_presentMode;
		Instance* m_pInstance;
		PhysicalDevice* m_pPhysicalDevice;

	public: // Methods:
		Surface(Instance* pInstance, PhysicalDevice* pPhysicalDevice, emberBackendInterface::IWindow* pIWindow, bool vSyncEnabled);
		~Surface();

		// Non-copyable:
		Surface(const Surface&) = delete;
		Surface& operator=(const Surface&) = delete;

		// Movable:
		Surface(Surface&& other) noexcept;
		Surface& operator=(Surface&& other) noexcept;

		// Getters:
		const VkSurfaceKHR GetVkSurfaceKHR() const;
		const SurfaceFormat GetSurfaceFormat() const;
		const PresentMode GetPresentMode() const;
		Uint2 GetCurrentExtent() const;
		Uint2 GetMinImageExtent() const;
		Uint2 GetMaxImageExtent() const;
		uint32_t GetMinImageCount() const;
		uint32_t GetMaxImageCount() const;

	private: // Methods:
		void Cleanup();
		void MoveFrom(Surface& other) noexcept;
	};
}