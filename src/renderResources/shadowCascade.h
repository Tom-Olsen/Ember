#ifndef __INCLUDE_GUARD_shadowCascade_h__
#define __INCLUDE_GUARD_shadowCascade_h__
#include "mathf.h"



namespace emberEngine
{
	// Forward declarations:
	class Camera;



	class ShadowCascade
	{
	private: // Members:
		static Float4 s_frustum_Clip[8];
		Float3 m_subFrustum_CameraLocal[8];
		Float3 m_subFrustum_World[8];
		Float3 m_position;	// in world space
		Float3 m_direction;	// in world space
		Float4x4 m_viewMatrix;
		Float4x4 m_projectionMatrix;
		float m_size;

	public: // Methods:
		ShadowCascade();
		void Update(Camera* const pCamera, const Float3& direction_World, float nearDepth, float farDepth, float sceneHeight);
		Float3 GetPosition() const;
		Float3 GetDirection() const;
		Float4x4 GetViewMatrix() const;
		Float4x4 GetProjectionMatrix() const;

	private: // Methods:
		void ComputeCascadePositionAndSize(Camera* const pCamera, float nearDepth, float farDepth, float sceneHeight);
		void ComputeSubFrustum(Camera* const pCamera, float nearDepth, float farDepth, const Float4x4& clipToCameraLocalMatrix, const Float4x4& cameraLocalToWorldMatrix);
		float ComputeFarClip(Camera* const pCamera, const Float4x4& lightLocalToWorldMatrix);
	};
}



#endif // __INCLUDE_GUARD_shadowCascade_h__