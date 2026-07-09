#pragma once
#include "iHandleTarget.h"



// Forward declarations:
namespace emberMath
{
	class Bounds;
}



namespace emberEditor
{
	class BoundsHandleTarget : public IHandleTarget
	{
	private: // Members:
		emberMath::Bounds* m_pBounds = nullptr;

	public: // Methods:
		// Constructors:
		BoundsHandleTarget();
		explicit BoundsHandleTarget(emberMath::Bounds* m_pBounds);

		// Setters:
		void SetBounds(emberMath::Bounds* m_pBounds);

		// Capabilities:
		HandleCapabilities GetCapabilities() const override;

		// Translation:
		Float3 GetPosition() const override;
		void SetPosition(const Float3& position) override;

		// Scale:
		Float3 GetScale() const override;
		void SetScale(const Float3& scale) override;
	};
}