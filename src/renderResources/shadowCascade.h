#ifndef __INCLUDE_GUARD_shadowCascade_h__
#define __INCLUDE_GUARD_shadowCascade_h__
#include "mathf.h"



class Camera;



class ShadowCascade
{
private: // Members:
	static Float4 s_frustumCorners_Clip[8];
	Camera* m_pCamera;
	Float3 m_position_World;
	Float3 m_direction_World;
	Float4x4 m_lightLocalToWorldMatrix;
	float m_width;
	float m_height;
	float m_nearClip = 1.0f;
	float m_farClip;
	Float3 m_subFrustumCorners_World[8];
	Float3 m_subFrustumCorners_LightLocal[8];

public: // Methods:
	ShadowCascade(Camera* const pCamera, const Float3& direction_World, float nearDepth, float farDepth, float worldHeight);
	Float3 GetPosition() const;
	Float3 GetDirection() const;
	Float4x4 GetLightLocalToWorldMatrix() const;
	Float4x4 GetProjectionMatrix() const;


private: // Methods:
	void ComputeSubFrustumCorners_World(float nearDepth, float farDepth);
	Float3 SubFrustumCenter_World() const;
	Float3 ComputeLightPosition_World(float worldHeight) const;
	void ComputeSubFrustumCorners_LightLocal(const Float3& position_World, const Float3& direction_World);
};



#endif // __INCLUDE_GUARD_shadowCascade_h__