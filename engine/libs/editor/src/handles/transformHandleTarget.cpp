#include "transformHandleTarget.h"
#include "transform.h"



namespace emberEditor
{
	// Public methods:
	// Constructors:
	TransformHandleTarget::TransformHandleTarget()
	{

	}
	TransformHandleTarget::TransformHandleTarget(emberEcs::Transform* pTransform)
	{
		m_pTransform = pTransform;
	}



	// Setters:
	void TransformHandleTarget::SetTransform(emberEcs::Transform* pTransform)
	{
		m_pTransform = pTransform;
	}



	// Capabilities:
	IHandleTarget::HandleCapabilities TransformHandleTarget::GetCapabilities() const
	{
		if (m_pTransform == nullptr)
			return IHandleTarget::HandleCapabilities::none;
		return IHandleTarget::HandleCapabilities::translateRotateScale;
	}



	// Translation:
	Float3 TransformHandleTarget::GetPosition() const
	{
		if (m_pTransform == nullptr)
			return Float3::zero;
		return m_pTransform->GetPosition();
	}
	void TransformHandleTarget::SetPosition(const Float3& position)
	{
		if (m_pTransform == nullptr)
			return;
		m_pTransform->SetPosition(position);
	}



	// Rotation:
	Float3x3 TransformHandleTarget::GetRotation3x3() const
	{
		if (m_pTransform == nullptr)
			return Float3x3::identity;
		return m_pTransform->GetRotation3x3();
	}
	void TransformHandleTarget::SetRotation(const Float3x3& rotation)
	{
		if (m_pTransform == nullptr)
			return;
		m_pTransform->SetRotationMatrix(rotation);
	}



	// Scale:
	Float3 TransformHandleTarget::GetScale() const
	{
		if (m_pTransform == nullptr)
			return Float3::one;
		return m_pTransform->GetScale();
	}
	void TransformHandleTarget::SetScale(const Float3& scale)
	{
		if (m_pTransform == nullptr)
			return;
		m_pTransform->SetScale(scale);
	}
}