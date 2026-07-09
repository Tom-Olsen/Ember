#include "rotatedBoundsHandleTarget.h"
#include "emberMath.h"



namespace emberEditor
{
	// Public methods:
	// Constructors:
	RotatedBoundsHandleTarget::RotatedBoundsHandleTarget()
	{

	}
	RotatedBoundsHandleTarget::RotatedBoundsHandleTarget(emberMath::RotatedBounds* pBounds)
	{
		m_pBounds = pBounds;
	}



	// Setters:
	void RotatedBoundsHandleTarget::SetBounds(emberMath::RotatedBounds* pBounds)
	{
		m_pBounds = pBounds;
	}



	// Capabilities:
	IHandleTarget::HandleCapabilities RotatedBoundsHandleTarget::GetCapabilities() const
	{
		if (m_pBounds == nullptr)
			return IHandleTarget::HandleCapabilities::none;
		return IHandleTarget::HandleCapabilities::translateRotateScale;
	}



	// Translation:
	Float3 RotatedBoundsHandleTarget::GetPosition() const
	{
		if (m_pBounds == nullptr)
			return Float3::zero;
		return m_pBounds->localBounds.center;
	}
	void RotatedBoundsHandleTarget::SetPosition(const Float3& position)
	{
		if (m_pBounds == nullptr)
			return;
		m_pBounds->localBounds.center = position;
	}



	// Rotation:
	Float3x3 RotatedBoundsHandleTarget::GetRotation3x3() const
	{
		if (m_pBounds == nullptr)
			return Float3x3::identity;
		return m_pBounds->GetRotation3x3();
	}
	void RotatedBoundsHandleTarget::SetRotation(const Float3x3& rotation)
	{
		if (m_pBounds == nullptr)
			return;
		m_pBounds->SetRotationMatrix(rotation);
	}



	// Scale:
	Float3 RotatedBoundsHandleTarget::GetScale() const
	{
		if (m_pBounds == nullptr)
			return Float3::one;
		return m_pBounds->localBounds.GetSize();
	}
	void RotatedBoundsHandleTarget::SetScale(const Float3& scale)
	{
		if (m_pBounds == nullptr)
			return;
		m_pBounds->localBounds.extent = 0.5f * Float3::Max(Float3::Abs(scale), Float3(1e-4f));
	}
}