#ifndef __INCLUDE_GUARD_shadowCascade_h__
#define __INCLUDE_GUARD_shadowCascade_h__
#include "mathf.h"



class Camera;



class ShadowCascade
{
private: // Members:
	static Float4 s_frustumCorners_Clip[8];
	Float3 m_subFrustumCorners_CameraLocal[8];
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
	float ComputeFarClip(Camera* const pCamera, const Float4x4& lightLocalToWorldMatrix);
};



#endif // __INCLUDE_GUARD_shadowCascade_h__