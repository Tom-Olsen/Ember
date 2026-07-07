#pragma once
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
			scale
		};

	private: // Members:
		Mode m_activeMode;
		emberEcs::Transform* m_pTransform;
		TranslateHandle m_translateHandle;
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

	private: // Methods:
		void SyncActiveHandleTarget();
	};
}