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
	m_direction_World = direction_World;

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

	Graphics::DrawBounds(Float4x4::identity, bounds);
	Graphics::DrawSphere(Float3::zero, 0.5f, Float4::green);
	for (int i = 0; i < 8; i++)
		Graphics::DrawSphere(m_subFrustumCorners_LightLocal[i], 0.5f, Float4::red);
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

	for (uint32_t i = 0; i < 8; i++)
		Graphics::DrawSphere(m_subFrustumCorners_World[i], 0.5f, Float4::blue);
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
	// This is wrong!
	// Correct way: (needs Float2x3/Float3x2 (?) implementation)
	// -Project all 8 sub frumstum corners to a plane that is perpendicular to the light direction.
	// -Compute xy-Coordinates of all 8 projected points in coordinate system of the plane.
	//  right = (1,0) is given by m_pCamera->GetTransform()->GetRight()
	//  up = (0,1) is given by the cross product of the light direction and the right vector.
	// -Compute min and max of the xy points.
	// -Compute center of the min-max rectangle.
	// -Transform this center point back to 3d space.
	// -Compute LinePlaneIntersection from this point to the plane with normal (0,0,1) and support (0,0,worldHeight).
	// -This is the light position.
	Float3 centerProjection = Float3::zero;
	for (uint32_t i = 0; i < 8; i++)
		centerProjection += geometry3d::PointToPlaneProjection(m_subFrustumCorners_World[i], Float3::zero, -m_direction_World);
	centerProjection /= 8.0f;

	Float3 planeNormal = Float3::up;
	Float3 planeSupport = worldHeight * Float3::up;
	return geometry3d::LinePlaneIntersection(centerProjection, m_direction_World, planeSupport, planeNormal);
}
void ShadowCascade::ComputeSubFrustumCorners_LightLocal(const Float3& position_World, const Float3& direction_World)
{
	// Transform sub frustum corners to local space of shadow cascade light source:
	Float3 cameraRight = m_pCamera->GetTransform()->GetRight();
	Float4x4 posMatrix = Float4x4::Translate(position_World);
	Float4x4 rotMatrix = Float4x4::RotateThreeLeg(Float3::forward, -direction_World, Float3::right, cameraRight);
	m_lightLocalToWorldMatrix = posMatrix * rotMatrix;
	Float4x4 worldToLightLocalMatrix = m_lightLocalToWorldMatrix.Inverse();
	for (uint32_t i = 0; i < 8; i++)
		m_subFrustumCorners_LightLocal[i] = Float3(worldToLightLocalMatrix * Float4(m_subFrustumCorners_World[i], 1.0f));
}