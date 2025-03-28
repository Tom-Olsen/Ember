#include "camera.h"
#include "fpsEditorWindow.h"



namespace emberEngine
{
	// Constructor/Destructor:
	Camera::Camera()
	{
		m_fov = math::deg2rad * 60.0f;
		m_aspectRatio = 16.0f / 9.0f;	// 1920x1080
		m_nearClip = 0.1f;
		m_farClip = 1000.0f;
		m_orthographicSize = 10.0f;
		m_projectionType = ProjectionType::perspective;
		m_updateProjectionMatrix = true;
		m_drawFrustum = false;
		fpsEditorWindow = std::make_unique<FpsEditorWindow>();
	}
	Camera::~Camera()
	{

	}



	// Setters:
	void Camera::SetFov(float fov)
	{
		if (m_fov != fov)
		{
			m_fov = fov;
			m_updateProjectionMatrix = true;
		}
	}
	void Camera::SetAspectRatio(float aspectRatio)
	{
		if (m_aspectRatio != aspectRatio)
		{
			m_aspectRatio = aspectRatio;
			m_updateProjectionMatrix = true;
		}
	}
	void Camera::SetNearClip(float nearClip)
	{
		if (m_nearClip != nearClip)
		{
			m_nearClip = nearClip;
			m_updateProjectionMatrix = true;
		}
	}
	void Camera::SetFarClip(float farClip)
	{
		if (m_farClip != farClip)
		{
			m_farClip = farClip;
			m_updateProjectionMatrix = true;
		}
	}
	void Camera::SetOrthographicSize(float orthographicSize)
	{
		if (m_orthographicSize != orthographicSize)
		{
			m_orthographicSize = orthographicSize;
			m_updateProjectionMatrix = true;
		}
	}
	void Camera::SetProjectionType(ProjectionType projectionType)
	{
		if (m_projectionType != projectionType)
		{
			m_projectionType = projectionType;
			m_updateProjectionMatrix = true;
		}
	}
	void Camera::SetDrawFrustum(bool drawFrustum)
	{
		m_drawFrustum = drawFrustum;
	}



	// Getters:
	float Camera::GetFov() const
	{
		return m_fov;
	}
	float Camera::GetAspectRatio() const
	{
		return m_aspectRatio;
	}
	float Camera::GetNearClip() const
	{
		return m_nearClip;
	}
	float Camera::GetFarClip() const
	{
		return m_farClip;
	}
	float Camera::GetOrthographicSize() const
	{
		return m_orthographicSize;
	}
	Float3 Camera::GetViewDirection() const
	{
		return m_pTransform->GetDown();
	}
	ProjectionType Camera::GetProjectionType() const
	{
		return m_projectionType;
	}
	Float4x4 Camera::GetViewMatrix() const
	{
		return GetTransform()->GetWorldToLocalMatrix();
	}
	Float4x4 Camera::GetProjectionMatrix()
	{
		if (m_updateProjectionMatrix && isActive && GetGameObject()->isActive)
			UpdateProjectionMatrix();
		return m_projectionMatrix;
	}



	// Private methods:
	void Camera::UpdateProjectionMatrix()
	{
		m_updateProjectionMatrix = false;
		if (m_projectionType == ProjectionType::perspective)
			m_projectionMatrix = Float4x4::Perspective(m_fov, m_aspectRatio, m_nearClip, m_farClip);
		else if (m_projectionType == ProjectionType::orthographic)
			m_projectionMatrix = Float4x4::Orthographic(-0.5f * m_orthographicSize * m_aspectRatio, 0.5f * m_orthographicSize * m_aspectRatio, -0.5f * m_orthographicSize, 0.5f * m_orthographicSize, m_nearClip, m_farClip);
	}



	// Overrides:
	void Camera::LateUpdate()
	{
		Graphics::SetActiveCamera(GetTransform()->GetPosition(), GetViewMatrix(), GetProjectionMatrix());
		if (m_drawFrustum)
			Graphics::DrawFrustum(m_pTransform->GetLocalToWorldMatrix(), GetProjectionMatrix());
	}
	const std::string Camera::ToString() const
	{
		return "Camera";
	}
}