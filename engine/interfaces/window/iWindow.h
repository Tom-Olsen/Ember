#ifndef __INCLUDE_GUARD_iWindow_h__
#define __INCLUDE_GUARD_iWindow_h__
#include "iMath.h"
#include <array>
#include <vector>



// Forward decleration:
typedef struct VkInstance_T* VkInstance;
struct VkAllocationCallbacks;
typedef struct VkSurfaceKHR_T* VkSurfaceKHR;
namespace emberEngine
{
    struct Event;
}



namespace emberBackendInterface
{
    class IWindow
    {
    public:
        IWindow() = default;
        virtual ~IWindow() = default;

        // Non-copyable:
        IWindow(const IWindow&) = delete;
        IWindow& operator=(const IWindow&) = delete;

		// Movable: (must be implemented by derived classes)
        IWindow(IWindow&&) noexcept = default;
        IWindow& operator=(IWindow&&) noexcept = default;

        // Window Methods:
        virtual void LinkDearImGui(void* pDearImGui) = 0;
        virtual std::vector<emberEngine::Event> PollEvents() = 0;
		virtual void AddWindowInstanceExtensions(std::vector<const char*>& instanceExtensions) const = 0;
        virtual void CreateSurface(VkInstance vkInstance, const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pVkSurfaceKHR) const = 0;

        // Getters:
        virtual bool GetIsMinimized() const = 0;
        virtual bool GetIsResized() const = 0;
        virtual void* GetNativeHandle() const = 0;
        virtual iMath::Int2 GetSize() const = 0;
        virtual uint32_t GetWindowID() const = 0;
    };
}



#endif // __INCLUDE_GUARD_iWindow_h__