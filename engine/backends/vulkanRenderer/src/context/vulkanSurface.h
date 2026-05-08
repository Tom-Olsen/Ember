#pragma once
#include "emberMath.h"
#include "vulkanFormat.h"
#include <cstdint>
#include <memory>
#include <vector>
#include <vulkan/vulkan.h>



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
		VkColorSpaceKHR colorSpace;
	};



	class Surface
	{
	private: // Members:
		VkSurfaceKHR m_surface;
		SurfaceFormat m_surfaceFormat;
		VkPresentModeKHR m_presentMode;
		Instance* m_pInstance;
		PhysicalDevice* m_pPhysicalDevice;
		emberBackendInterface::IWindow* m_pIWindow;

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
		const VkPresentModeKHR GetPresentMode() const;
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