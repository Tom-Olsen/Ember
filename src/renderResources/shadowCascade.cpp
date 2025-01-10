#include "shadowCascade.h"
#include "camera.h"
#include "emberEngine.h"



// static members:
Float4 ShadowCascade::s_frustumCorners_Clip[8] =
{
	Float4(-1, -1, -1, 1),
	Float4(-1, -1,  1, 1),
	Float4(-1,  1, -1, 1),
	Float4(-1,  1,  1, 1),
	Float4(1, -1, -1, 1),
	Float4(1, -1,  1, 1),
	Float4(1,  1, -1, 1),
	Float4(1,  1,  1, 1)
};



// Constructor:
ShadowCascade::ShadowCascade(Camera* const pCamera, const Float3& direction_World, float nearDepth, float farDepth, float worldHeight)
{
	m_pCamera = pCamera;
	m_direction_World = direction_World; // should already be normalized.

	ComputeSubFrustumCorners_World(nearDepth, farDepth);
	Float3 subFrustumCenter_World = SubFrustumCenter_World();
	// Add 1 to world height to compensate the near clip plane of 1.
	m_position_World = ComputeLightPosition_World(worldHeight + 1.0f);
	ComputeSubFrustumCorners_LightLocal(m_position_World, direction_World);

	// Construct bounding box that encapsulates the sub frustum corners and the light source in light local space:
	Bounds bounds = Bounds(m_subFrustumCorners_LightLocal);
	bounds.Encapsulate(Float3::zero);	// light source itself.
	m_width = bounds.GetSize().x;
	m_height = bounds.GetSize().y;
	m_farClip = bounds.GetSize().z;
}


// Public methods:
Float3 ShadowCascade::GetPosition() const
{
	return m_position_World;
}
Float3 ShadowCascade::GetDirection() const
{
	return m_direction_World;
}
Float4x4 ShadowCascade::GetLightLocalToWorldMatrix() const
{
	return m_lightLocalToWorldMatrix;
}
Float4x4 ShadowCascade::GetProjectionMatrix() const
{
	float left = -m_width / 2.0f;
	float right = m_width / 2.0f;
	float bottom = -m_height / 2.0f;
	float top = m_height / 2.0f;
	return Float4x4::Orthographic(left, right, bottom, top, m_nearClip, m_farClip);
}


// Private methods:
void ShadowCascade::ComputeSubFrustumCorners_World(float nearDepth, float farDepth)
{
	Float4x4 clipToCameraLocalMatrix = m_pCamera->GetProjectionMatrix().Inverse();
	Float4x4 cameraLocalToWorldMatrix = m_pCamera->GetTransform()->GetLocalToWorldMatrix();

	Float3 frustumCorners_World[8];
	for (uint32_t i = 0; i < 8; i++)
	{
		Float4 frustumCorners_CameraLocal = clipToCameraLocalMatrix * s_frustumCorners_Clip[i];
		frustumCorners_CameraLocal /= frustumCorners_CameraLocal.w;
		frustumCorners_World[i] = Float3(cameraLocalToWorldMatrix * frustumCorners_CameraLocal);
	}

	// Sub frustum corners in camera space:
	// nearDepth, farDepth € [0,1] are the percentile splits of the camera frustum.
	m_subFrustumCorners_World[0] = frustumCorners_World[0] + nearDepth * (frustumCorners_World[1] - frustumCorners_World[0]);
	m_subFrustumCorners_World[1] = frustumCorners_World[0] + farDepth  * (frustumCorners_World[1] - frustumCorners_World[0]);
	m_subFrustumCorners_World[2] = frustumCorners_World[2] + nearDepth * (frustumCorners_World[3] - frustumCorners_World[2]);
	m_subFrustumCorners_World[3] = frustumCorners_World[2] + farDepth  * (frustumCorners_World[3] - frustumCorners_World[2]);
	m_subFrustumCorners_World[4] = frustumCorners_World[4] + nearDepth * (frustumCorners_World[5] - frustumCorners_World[4]);
	m_subFrustumCorners_World[5] = frustumCorners_World[4] + farDepth  * (frustumCorners_World[5] - frustumCorners_World[4]);
	m_subFrustumCorners_World[6] = frustumCorners_World[6] + nearDepth * (frustumCorners_World[7] - frustumCorners_World[6]);
	m_subFrustumCorners_World[7] = frustumCorners_World[6] + farDepth  * (frustumCorners_World[7] - frustumCorners_World[6]);
}
Float3 ShadowCascade::SubFrustumCenter_World() const
{
	Float3 subFrustumCenter_World = Float3::zero;
	for (uint32_t i = 0; i < 8; i++)
		subFrustumCenter_World += m_subFrustumCorners_World[i];
	return subFrustumCenter_World / 8.0f;
}
Float3 ShadowCascade::ComputeLightPosition_World(float worldHeight) const
{
	// Plane direction vectors e1=(1,0) and e2=(0,1) in world space:
	Float3 e1 = geometry3d::PointToPlaneProjection(m_pCamera->GetTransform()->GetRight(), Float3::zero, m_direction_World).Normalize();
	Float3 e2 = Float3::Cross(m_direction_World, e1).Normalize();

	// Projection matrices:
	Float3x2 planeToWorld = Float3x2::Columns(e1, e2);
	Float2x3 worldToPlane = planeToWorld.LeftInverse();

	// Project sub frustum corners to plane and determine 2d bounding box on the plane (min/max):
	Float2 centerPlane = Float2::zero;
	Float2 min = Float2::max;
	Float2 max = Float2::min;
	for (uint32_t i = 0; i < 8; i++)
	{
		Float2 subFrustumCorner_Projection = worldToPlane * m_subFrustumCorners_World[i];
		min = Float2::Min(min, subFrustumCorner_Projection);
		max = Float2::Max(max, subFrustumCorner_Projection);
	}
	// Center of bounding box in world space is the point at which the light source must be aimed.
	Float3 centerWorld = planeToWorld * (min + max) / 2.0f;

	Float3 planeNormal = Float3::up;
	Float3 planeSupport = worldHeight * Float3::up;
	return geometry3d::LinePlaneIntersection(centerWorld, m_direction_World, planeSupport, planeNormal);
}
void ShadowCascade::ComputeSubFrustumCorners_LightLocal(const Float3& position_World, const Float3& direction_World)
{
	// Transform sub frustum corners to local space of shadow cascade light source:
	Float3 cameraRight = m_pCamera->GetTransform()->GetRight();
	Float4x4 posMatrix = Float4x4::Translate(position_World);
	Float4x4 rotMatrix = Float4x4::RotateThreeLeg(Float3::up, -direction_World, Float3::right, cameraRight);
	m_lightLocalToWorldMatrix = posMatrix * rotMatrix;
	Float4x4 worldToLightLocalMatrix = m_lightLocalToWorldMatrix.Inverse();
	for (uint32_t i = 0; i < 8; i++)
		m_subFrustumCorners_LightLocal[i] = Float3(worldToLightLocalMatrix * Float4(m_subFrustumCorners_World[i], 1.0f));
}