#pragma once
#include "rotateHandle.h"
#include "scaleHandle.h"
#include "translateHandle.h"



// Forward declarations:
namespace emberEcs
{
	class Transform;
}



namespace emberEditor
{
	class TransformHandle
	{
	public: // Enums:
		enum class Mode
		{
            translate,
            rotate,
			scale
		};

	private: // Members:
        // Static:
		static Float4 s_colorX;
		static Float4 s_colorY;
		static Float4 s_colorZ;
		static Float4 s_colorCenter;
        static Float4 s_hoverColor;
        static Float4 s_activeColor;
		static Float4x4 s_rotX;
		static Float4x4 s_rotY;
		static Float4x4 s_rotZ;
        // Per instance:
		Mode m_activeMode;
		emberEcs::Transform* m_pTransform;
		TranslateHandle m_translateHandle;
		RotateHandle m_rotateHandle;
		ScaleHandle m_scaleHandle;

	public: // Methods:
		// Constructor/Destructor:
		TransformHandle();
		~TransformHandle();

		// Target:
		void SetTarget(emberEcs::Transform* pTransform);
		void ClearTarget();
		bool HasTarget() const;

		// Mode:
		void SetMode(Mode mode);
		Mode GetMode() const;
		void ConsumeModeHotkeys();

		// Handle:
		bool GetIsDragging() const;

		// Update/Draw:
		void Update();
		void Draw();

        // Colors:
        static void SetColorX(const Float4& color);
        static void SetColorY(const Float4& color);
        static void SetColorZ(const Float4& color);
        static void SetColorCenter(const Float4& color);
        static void SetHoverColor(const Float4& color);
        static void SetActiveColor(const Float4& color);
        static void ResetColors();
        static Float4 GetColorX();
        static Float4 GetColorY();
        static Float4 GetColorZ();
        static Float4 GetColorCenter();
        static Float4 GetHoverColor();
        static Float4 GetActiveColor();

        // Rotations:
        static Float4x4 GetRotationX();
        static Float4x4 GetRotationY();    
        static Float4x4 GetRotationZ();

	private: // Methods:
		void SyncActiveHandleTarget();
	};
}