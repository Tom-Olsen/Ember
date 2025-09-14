#pragma once
#include <memory>



namespace emberBackendInterface
{
    class IRenderer
    {
    public:
        // Virtual destructor for v-table:
        virtual ~IRenderer() = default;

        virtual void RenderFrame(int windowWidth, int windowHeight, float time, float deltaTime) = 0;
    };
}