#include "boundsHandleTarget.h"
#include "emberMath.h"



namespace emberEditor
{
	// Public methods:
	// Constructors:
	BoundsHandleTarget::BoundsHandleTarget()
	{

	}
	BoundsHandleTarget::BoundsHandleTarget(emberMath::Bounds* pBounds)
	{
		m_pBounds = pBounds;
	}



	// Setters:
	void BoundsHandleTarget::SetBounds(emberMath::Bounds* pBounds)
	{
		m_pBounds = pBounds;
	}



	// Capabilities:
	IHandleTarget::HandleCapabilities BoundsHandleTarget::GetCapabilities() const
	{
		if (m_pBounds == nullptr)
			return IHandleTarget::HandleCapabilities::none;
		return IHandleTarget::HandleCapabilities::translateScale;
	}



	// Translation:
	Float3 BoundsHandleTarget::GetPosition() const
	{
		if (m_pBounds == nullptr)
			return Float3::zero;
		return m_pBounds->center;
	}
	void BoundsHandleTarget::SetPosition(const Float3& position)
	{
		if (m_pBounds == nullptr)
			return;
		Float3 extent = m_pBounds->extent;
        *m_pBounds = Bounds(position, extent);
	}



	// Scale:
	Float3 BoundsHandleTarget::GetScale() const
	{
		if (m_pBounds == nullptr)
			return Float3::one;
		return m_pBounds->GetSize();
	}
	void BoundsHandleTarget::SetScale(const Float3& scale)
	{
		if (m_pBounds == nullptr)
			return;
		m_pBounds->extent = 0.5f * Float3::Max(Float3::Abs(scale), Float3(1e-4f));
	}
}