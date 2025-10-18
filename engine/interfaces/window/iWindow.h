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
        virtual Int2 GetSize() const = 0;
        virtual uint32_t GetWindowID() const = 0;
    };
}