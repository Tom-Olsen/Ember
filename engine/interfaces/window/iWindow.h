#pragma once
#include "commonEvent.h"
#include "emberMath.h"
#include <array>
#include <vector>



// Forward decleration:
typedef struct VkInstance_T* VkInstance;
struct VkAllocationCallbacks;
typedef struct VkSurfaceKHR_T* VkSurfaceKHR;
namespace emberBackendInterface
{
    class IGui;
}



namespace emberBackendInterface
{
    class IWindow
    {
    public:
        // Virtual destructor for v-table:
        virtual ~IWindow() = default;

        // Window Methods:
        virtual void LinkIGuiHandle(emberBackendInterface::IGui* pIGui) = 0;
        virtual std::vector<emberCommon::Event> PollEvents() = 0;
		virtual void AddWindowInstanceExtensions(std::vector<const char*>& instanceExtensions) const = 0;
        virtual void CreateSurface(VkInstance vkInstance, const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pVkSurfaceKHR) const = 0;

        // Getters:
        virtual void* GetNativeHandle() const = 0;
        virtual bool GetIsMinimized() const = 0;
        virtual bool GetIsResized() const = 0;

        // Logical client-area size in window coordinates.
        // Use this for input, ImGui layout, hit testing, and any math that should match mouse coordinates.
        // On high-DPI displays this may be smaller than the real drawable pixel size.
        virtual Int2 GetSize() const = 0;

        // Real drawable client-area size in physical pixels.
        // Use this for swapchain extent, framebuffer size, viewport/scissor setup, and other rasterization work.
        // On high-DPI displays this may be larger than GetSize().
        virtual Int2 GetSizeInPixels() const = 0;

        virtual uint32_t GetWindowID() const = 0;

        // Setters:
        virtual void ResetWindowResized() = 0;
    };
}
