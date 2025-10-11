#ifndef __INCLUDE_GUARD_iEditor_h__
#define __INCLUDE_GUARD_iEditor_h__



namespace emberBackendInterface
{
    class IEditor
    {
    public:
        IEditor() = default;
        virtual ~IEditor() = default;

        // Non-copyable:
        IEditor(const IEditor&) = delete;
        IEditor& operator=(const IEditor&) = delete;

        // Movable: (must be implemented by derived classes)
        IEditor(IEditor&&) noexcept = default;
        IEditor& operator=(IEditor&&) noexcept = default;

        virtual void Render() const = 0;
        virtual void* GetFocusedWindow() const = 0;
        virtual bool GetWindowWantCaptureEvents() const = 0;
    };
}



#endif // __INCLUDE_GUARD_iEditor_h__