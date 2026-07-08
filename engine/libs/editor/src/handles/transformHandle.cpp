#include "transformHandle.h"
#include "eventSystem.h"
#include "transform.h"



namespace emberEditor
{
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