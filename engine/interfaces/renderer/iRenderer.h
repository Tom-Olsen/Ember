#ifndef __INCLUDE_GUARD_renderer_h__
#define __INCLUDE_GUARD_renderer_h__
#include "buffer.h"
#include <memory>



namespace emberBackendInterface
{
    class IRenderer
    {
    public:
        virtual ~IRenderer() = default;

        // Factories:
        virtual std::unique_ptr<Buffer> CreateBuffer(const BufferInfo& bufferInfo) = 0;
        // ...
    };
}



#endif // __INCLUDE_GUARD_renderer_h__