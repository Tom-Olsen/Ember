#ifndef __INCLUDE_GUARD_camera_h__
#define __INCLUDE_GUARD_camera_h__
#include "emberEngine.h"



namespace emberEngine
{
	/// <summary>
	/// View direction is down = (0,0,-1).
	/// </summary>
	class Camera : public Component
	{
	private: // Members:
		float m_fov;
		float m_aspectRatio;
		float m_nearClip;
		float m_farClip;
		float m_orthographicSize;
		ProjectionType m_projectionType;
		Float4x4 m_projectionMatrix;
		bool m_updateProjectionMatrix;
		bool m_drawFrustum;

	public: // Methods:
		Camera();
		~Camera();

		// Setters:
		void SetFov(float fov);
		void SetAspectRatio(float aspectRatio);
		void SetNearClip(float nearClip);
		void SetFarClip(float farClip);
		void SetOrthographicSize(float orthographicSize);
		void SetProjectionType(ProjectionType projectionType);
		void SetDrawFrustum(bool drawFrustum);

		// Getters:
		float GetFov() const;
		float GetAspectRatio() const;
		float GetNearClip() const;
		float GetFarClip() const;
		float GetOrthographicSize() const;
		Float3 GetViewDirection() const;
		ProjectionType GetProjectionType() const;
		Float4x4 GetViewMatrix() const;
		Float4x4 GetProjectionMatrix();

		// Overrides:
		void LateUpdate() override;
		const std::string ToString() const override;

	private: // Methods:
		void UpdateProjectionMatrix();
	};
}



#endif // __INCLUDE_GUARD_camera_h__