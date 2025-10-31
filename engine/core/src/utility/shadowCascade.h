#pragma once
#include "emberCoreExport.h"
#include "emberMath.h"



namespace emberEngine
{
	class EMBER_CORE_API ShadowCascade
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
		void Update(const Float4x4& cameraLocalToWorldMatrix, const Float4x4& cameraProjectionMatrix, const Float3& direction_World, float nearDepth, float farDepth, float sceneHeight);
		Float3 GetPosition() const;
		Float3 GetDirection() const;
		Float4x4 GetViewMatrix() const;
		Float4x4 GetProjectionMatrix() const;

	private: // Methods:
		void ComputeCascadePositionAndSize(const Float4x4& cameraLocalToWorldMatrix, const Float4x4& cameraProjectionMatrix, float nearDepth, float farDepth, float sceneHeight);
		void ComputeSubFrustum(float nearDepth, float farDepth, const Float4x4& clipToCameraLocalMatrix, const Float4x4& cameraLocalToWorldMatrix);
		float ComputeFarClip(const Float4x4& cameraLocalToWorldMatrix, const Float4x4& lightLocalToWorldMatrix);
	};
}