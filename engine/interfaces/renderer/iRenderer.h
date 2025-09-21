#pragma once



namespace emberBackendInterface
{
    class IRenderer
    {
    public: // Methods:
        // Virtual destructor for v-table:
        virtual ~IRenderer() = default;

        virtual void RenderFrame(int windowWidth, int windowHeight, float time, float deltaTime) = 0;
    };
}