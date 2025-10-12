#pragma once



namespace emberBackendInterface
{
    class IEditor
    {
    public:
        // Virtual destructor for v-table:
        virtual ~IEditor() = default;

        virtual void Render() const = 0;
        virtual void* GetFocusedWindow() const = 0;
        virtual bool GetWindowWantCaptureEvents() const = 0;
    };
}