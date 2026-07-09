#pragma once
#include "emberMath.h"
#include <cstdint>



namespace emberEditor
{
    class IHandleTarget
    {
    protected:  // Capability checks:
        enum HandleCapabilities : uint8_t
        {
            none      = 0b00000000,
            translate = 0b00000001,
            rotate    = 0b00000010,
            scale     = 0b00000100,
            translateRotate      = translate | rotate,
            translateScale       = translate | scale,
            rotateScale          = rotate | scale,
            translateRotateScale = translate | rotate | scale
        };
    	virtual HandleCapabilities GetCapabilities() const = 0;

    public: // Methods:
        // Virtual destructor for v-table:
        virtual ~IHandleTarget() = default;

        // Capability checks:
        bool CanTranslate() const { return (GetCapabilities() & translate) != none; }
        bool CanRotate() const { return (GetCapabilities() & rotate) != none; }
        bool CanScale() const { return (GetCapabilities() & scale) != none; }

        // Translation:
    	virtual Float3 GetPosition() const = 0; // must be implemented as otherwise handle position not defined.
    	virtual void SetPosition(const Float3& position) {}

        // Rotation:
    	virtual Float3x3 GetRotation3x3() const { return Float3x3::identity; }
    	virtual Float4x4 GetRotation4x4() const { return Float4x4(GetRotation3x3()); }
    	virtual void SetRotation(const Float3x3& rotation) {}

        // Scale:
    	virtual Float3 GetScale() const { return Float3::one; }
    	virtual void SetScale(const Float3& scale) {}
    };
}