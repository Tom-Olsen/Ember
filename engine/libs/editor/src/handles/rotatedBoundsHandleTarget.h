#pragma once
#include "iHandleTarget.h"



// Forward declarations:
namespace emberMath
{
	struct RotatedBounds;
}



namespace emberEditor
{
	class RotatedBoundsHandleTarget : public IHandleTarget
	{
	private: // Members:
		emberMath::RotatedBounds* m_pBounds = nullptr;

	public: // Methods:
		// Constructors:
		RotatedBoundsHandleTarget();
		explicit RotatedBoundsHandleTarget(emberMath::RotatedBounds* m_pBounds);

		// Setters:
		void SetBounds(emberMath::RotatedBounds* m_pBounds);

		// Capabilities:
		HandleCapabilities GetCapabilities() const override;

		// Translation:
		Float3 GetPosition() const override;
		void SetPosition(const Float3& position) override;

		// Rotation:
		Float3x3 GetRotation3x3() const override;
		void SetRotation(const Float3x3& rotation) override;

		// Scale:
		Float3 GetScale() const override;
		void SetScale(const Float3& scale) override;
	};
}