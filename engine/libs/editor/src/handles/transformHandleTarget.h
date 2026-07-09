#pragma once
#include "iHandleTarget.h"



// Forward declarations:
namespace emberEcs
{
	class Transform;
}



namespace emberEditor
{
	class TransformHandleTarget : public IHandleTarget
	{
	private: // Members:
		emberEcs::Transform* m_pTransform = nullptr;

	public: // Methods:
		// Constructors:
		TransformHandleTarget();
		explicit TransformHandleTarget(emberEcs::Transform* pTransform);

		// Setters:
		void SetTransform(emberEcs::Transform* pTransform);

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