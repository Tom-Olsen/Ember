#include "transformHandle.h"
#include "eventSystem.h"
#include "transform.h"



namespace emberEditor
{
	// Static members:
    // Handle colors:
	Float4 TransformHandle::s_colorX = Float4(1.0f, 0.38039, 0.24314, 1.0f);
	Float4 TransformHandle::s_colorY = Float4(0.78039f, 1.0f, 0.41569, 1.0f);
	Float4 TransformHandle::s_colorZ = Float4(0.31765f, 0.66667f, 1.0f, 1.0f);
	Float4 TransformHandle::s_colorCenter = Float4::white;
	Float4 TransformHandle::s_hoverColor = Float4(0.9f, 0.9f, 0.0f, 1.0f);
	Float4 TransformHandle::s_activeColor = Float4::yellow;
    // Rotation matrizes:
	Float4x4 TransformHandle::s_rotX = Float4x4::RotateFromTo(Float3::up, Float3::right) * Float4x4::RotateZ(math::pi2);
	Float4x4 TransformHandle::s_rotY = Float4x4::RotateFromTo(Float3::up, Float3::forward) * Float4x4::RotateZ(-math::pi2);
	Float4x4 TransformHandle::s_rotZ = Float4x4::identity;

	// Public methods:
	// Constructor/Destructor:
	TransformHandle::TransformHandle()
	{
		m_activeMode = Mode::translate;
		m_pTransform = nullptr;
	}
	TransformHandle::~TransformHandle()
	{

	}



	// Target:
	void TransformHandle::SetTarget(emberEcs::Transform* pTransform)
	{
		if (m_pTransform == pTransform)
			return;
		m_pTransform = pTransform;
		SyncActiveHandleTarget();
	}
	void TransformHandle::ClearTarget()
	{
		m_pTransform = nullptr;
		m_translateHandle.ClearTarget();
		m_rotateHandle.ClearTarget();
		m_scaleHandle.ClearTarget();
	}
	bool TransformHandle::HasTarget() const
	{
		return m_pTransform != nullptr;
	}



	// Mode:
	void TransformHandle::SetMode(Mode mode)
	{
		if (m_activeMode == mode || GetIsDragging())
			return;
		m_activeMode = mode;
		SyncActiveHandleTarget();
	}
	TransformHandle::Mode TransformHandle::GetMode() const
	{
		return m_activeMode;
	}
	void TransformHandle::ConsumeModeHotkeys()
	{
		if (GetIsDragging())
			return;

		if (emberCore::EventSystem::KeyDown(emberCommon::Input::Key::W))
		{
			SetMode(Mode::translate);
			emberCore::EventSystem::ConsumeKey(emberCommon::Input::Key::W);
		}
		if (emberCore::EventSystem::KeyDown(emberCommon::Input::Key::E))
		{
			SetMode(Mode::rotate);
			emberCore::EventSystem::ConsumeKey(emberCommon::Input::Key::E);
		}
		if (emberCore::EventSystem::KeyDown(emberCommon::Input::Key::R))
		{
			SetMode(Mode::scale);
			emberCore::EventSystem::ConsumeKey(emberCommon::Input::Key::R);
		}
	}



	// Handle:
	bool TransformHandle::GetIsDragging() const
	{
		return m_translateHandle.GetIsDragging() || m_scaleHandle.GetIsDragging() || m_rotateHandle.GetIsDragging();
	}



	// Update/Draw:
	void TransformHandle::Update()
	{
		if (!HasTarget())
			return;
		if (m_activeMode == Mode::translate)
			m_translateHandle.Update();
		else if (m_activeMode == Mode::rotate)
            m_rotateHandle.Update();
		else if (m_activeMode == Mode::scale)
			m_scaleHandle.Update();
	}
	void TransformHandle::Draw()
	{
		if (m_activeMode == Mode::translate)
			m_translateHandle.Draw();
		else if (m_activeMode == Mode::rotate)
            m_rotateHandle.Draw();
		else if (m_activeMode == Mode::scale)
			m_scaleHandle.Draw();
	}


    // Colors:
    void TransformHandle::SetColorX(const Float4& color)
    {
        s_colorX = color;
    }
    void TransformHandle::SetColorY(const Float4& color)
    {
        s_colorY = color;
    }
    void TransformHandle::SetColorZ(const Float4& color)
    {
        s_colorZ = color;
    }
    void TransformHandle::SetColorCenter(const Float4& color)
    {
        s_colorCenter = color;
    }
    void TransformHandle::SetHoverColor(const Float4& color)
    {
        s_hoverColor = color;
    }
    void TransformHandle::SetActiveColor(const Float4& color)
    {
        s_activeColor = color;
    }
    void TransformHandle::ResetColors()
    {
	    s_colorX = Float4(1.0f, 0.38039, 0.24314, 1.0f);
	    s_colorY = Float4(0.78039f, 1.0f, 0.41569, 1.0f);
	    s_colorZ = Float4(0.31765f, 0.66667f, 1.0f, 1.0f);
	    s_colorCenter = Float4::white;
	    s_hoverColor = Float4(0.9f, 0.9f, 0.0f, 1.0f);
	    s_activeColor = Float4::yellow;
    }
    Float4 TransformHandle::GetColorX()
    {
        return s_colorX;
    }
    Float4 TransformHandle::GetColorY()
    {
        return s_colorY;
    }
    Float4 TransformHandle::GetColorZ()
    {
        return s_colorZ;
    }
    Float4 TransformHandle::GetColorCenter()
    {
        return s_colorCenter;
    }
    Float4 TransformHandle::GetHoverColor()
    {
        return s_hoverColor;
    }
    Float4 TransformHandle::GetActiveColor()
    {
        return s_activeColor;
    }



    // Rotations:
    Float4x4 TransformHandle::GetRotationX()
    {
        return s_rotX;
    }
    Float4x4 TransformHandle::GetRotationY()
    {
        return s_rotY;
    }
    Float4x4 TransformHandle::GetRotationZ()
    {
        return s_rotZ;
    }



	// Private methods:
	void TransformHandle::SyncActiveHandleTarget()
	{
		if (m_activeMode == Mode::translate)
		{
			m_translateHandle.SetTarget(m_pTransform);
			m_rotateHandle.ClearTarget();
			m_scaleHandle.ClearTarget();
		}
        else if (m_activeMode == Mode::rotate)
        {
            m_translateHandle.ClearTarget();
			m_rotateHandle.SetTarget(m_pTransform);
			m_scaleHandle.ClearTarget();
        }
		else if (m_activeMode == Mode::scale)
		{
            m_translateHandle.ClearTarget();
			m_rotateHandle.ClearTarget();
			m_scaleHandle.SetTarget(m_pTransform);
		}
	}
}