#include "shadowCascade.h"
#include "camera.h"
#include "emberEngine.h"



// static members:
Float4 ShadowCascade::s_frustumCorners_Clip[8] =
{
	Float4(-1, -1, 0, 1),
	Float4(-1, -1, 1, 1),
	Float4(-1,  1, 0, 1),
	Float4(-1,  1, 1, 1),
	Float4( 1, -1, 0, 1),
	Float4( 1, -1, 1, 1),
	Float4( 1,  1, 0, 1),
	Float4( 1,  1, 1, 1)
};



// Constructor:
ShadowCascade::ShadowCascade()
{
	m_size = 0.0f;
	m_position = Float3::zero;
	m_viewMatrix = Float4x4::identity;
	m_projectionMatrix = Float4x4::identity;
}
void ShadowCascade::Update(Camera* const pCamera, const Float3& direction_World, float nearDepth, float farDepth, float sceneHeight)
{
	m_direction = direction_World;
	ComputeCascadePositionAndSize(pCamera, nearDepth, farDepth, sceneHeight);

	// Construct view matrix:
	Float4x4 posMatrix = Float4x4::Translate(m_position);
	Float4x4 rotMatrix = Float4x4::RotateThreeLeg(Float3::down, m_direction, Float3::right, Float3::right);
	Float4x4 lightLocalToWorldMatrix = posMatrix * rotMatrix;
	m_viewMatrix = lightLocalToWorldMatrix.Inverse(); // world to light local.


	// Construct orthographic projection matrix:
	float left = -0.5f * m_size;
	float right = 0.5f * m_size;
	float bottom = -0.5f * m_size;
	float top = 0.5f * m_size;
	float nearClip = 0.0f;
	float farClip = ComputeFarClip(pCamera, lightLocalToWorldMatrix);
	m_projectionMatrix = Float4x4::Orthographic(left, right, bottom, top, nearClip, farClip);
}


// Public methods:
Float3 ShadowCascade::GetPosition() const
{
	return m_position;
}
Float3 ShadowCascade::GetDirection() const
{
	return m_direction;
}
Float4x4 ShadowCascade::GetViewMatrix() const
{
	return m_viewMatrix;
}
Float4x4 ShadowCascade::GetProjectionMatrix() const
{
	return m_projectionMatrix;
}


// Private methods:
void ShadowCascade::ComputeCascadePositionAndSize(Camera* const pCamera, float nearDepth, float farDepth, float sceneHeight)
{
	Float4x4 clipToCameraLocalMatrix = pCamera->GetProjectionMatrix().Inverse();
	Float4x4 cameraLocalToWorldMatrix = pCamera->GetTransform()->GetLocalToWorldMatrix();

	Float3 frustumCorners_CameraLocal[8];
	for (uint32_t i = 0; i < 8; i++)
	{
		Float4 temp = clipToCameraLocalMatrix * s_frustumCorners_Clip[i];
		frustumCorners_CameraLocal[i] = Float3(temp) / temp.w;
	}

	// Sub frustum corners in camera space:
	// nearDepth, farDepth € [0,1] are the percentile splits of the camera frustum.
	m_subFrustumCorners_CameraLocal[0] = frustumCorners_CameraLocal[0] + nearDepth * (frustumCorners_CameraLocal[1] - frustumCorners_CameraLocal[0]);
	m_subFrustumCorners_CameraLocal[1] = frustumCorners_CameraLocal[0] + farDepth  * (frustumCorners_CameraLocal[1] - frustumCorners_CameraLocal[0]);
	m_subFrustumCorners_CameraLocal[2] = frustumCorners_CameraLocal[2] + nearDepth * (frustumCorners_CameraLocal[3] - frustumCorners_CameraLocal[2]);
	m_subFrustumCorners_CameraLocal[3] = frustumCorners_CameraLocal[2] + farDepth  * (frustumCorners_CameraLocal[3] - frustumCorners_CameraLocal[2]);
	m_subFrustumCorners_CameraLocal[4] = frustumCorners_CameraLocal[4] + nearDepth * (frustumCorners_CameraLocal[5] - frustumCorners_CameraLocal[4]);
	m_subFrustumCorners_CameraLocal[5] = frustumCorners_CameraLocal[4] + farDepth  * (frustumCorners_CameraLocal[5] - frustumCorners_CameraLocal[4]);
	m_subFrustumCorners_CameraLocal[6] = frustumCorners_CameraLocal[6] + nearDepth * (frustumCorners_CameraLocal[7] - frustumCorners_CameraLocal[6]);
	m_subFrustumCorners_CameraLocal[7] = frustumCorners_CameraLocal[6] + farDepth  * (frustumCorners_CameraLocal[7] - frustumCorners_CameraLocal[6]);


	for (uint32_t i = 0; i < 8; i++)
	{
		Float3 subFrustumCorner_World = Float3(cameraLocalToWorldMatrix * Float4(m_subFrustumCorners_CameraLocal[i], 1.0f));
		Graphics::DrawSphere(subFrustumCorner_World, 0.2f, Float4::black);
	}

	// Compute orthographic projection size so that the sub frustum always fits:
	Bounds bounds = Bounds(m_subFrustumCorners_CameraLocal);
	m_size = bounds.GetDiagonal();

	// Compute sub frustum center:
	float texelSize = m_size / SHADOW_MAP_WIDTH;
	Float3 center_CameraLocal = bounds.center;	// works because in camera space the frustum is axis aligned.
	Float3 center_World = Float3(cameraLocalToWorldMatrix * Float4(center_CameraLocal, 1.0f));

	// Construct 2d plane orhtogonal to light direction, with coordinate system alignes with light local space:
	Float4x4 rotMatrix = Float4x4::RotateThreeLeg(Float3::down, m_direction, Float3::right, Float3::right);
	Float3 e1 = Float3(rotMatrix * Float4::right);
	Float3 e2 = Float3::Cross(m_direction, e1).Normalize();
	Float3x2 planeToWorld = Float3x2::Columns(e1, e2);
	Float2x3 worldToPlane = planeToWorld.LeftInverse();
	Float2 center_Plane = worldToPlane * center_World;
	center_Plane.x = mathf::Round(center_Plane.x / texelSize) * texelSize;
	center_Plane.y = mathf::Round(center_Plane.y / texelSize) * texelSize;
	center_World = planeToWorld * center_Plane;

	// Compute light position:
	Float3 planeNormal = Float3::up;
	Float3 planeSupport = sceneHeight * Float3::up;
	m_position = geometry3d::LinePlaneIntersection(center_World, m_direction, planeSupport, planeNormal);
}
float ShadowCascade::ComputeFarClip(Camera* const pCamera, const Float4x4& lightLocalToWorldMatrix)
{
	// Construct bounding box that encapsulates the sub frustum corners and the light source in light space:
	Bounds bounds = Bounds(Float3::zero, Float3::zero);	// bounds only contains light source
	Float4x4 cameraLocalToLightLocalMatrix = m_viewMatrix * pCamera->GetTransform()->GetLocalToWorldMatrix();
	for (uint32_t i = 0; i < 8; i++)
	{
		Float3 corner = Float3(cameraLocalToLightLocalMatrix * Float4(m_subFrustumCorners_CameraLocal[i], 1.0f));
		bounds.Encapsulate(corner);
	}
	return bounds.GetSize().z;

	// TODO:
	// - find 3 lowest points of camera frustum
	// - construct plane from these points (as below)
	// - find intersection of all four bounding light lines with this plane (as below)
	// - find furthest intersection point as seen from support point (sure this is correct? the four points may have vastly different start heights) (as below?)
	// - ignore points that are behind the camera
	//Float4x4 clipToCameraLocalMatrix = pCamera->GetProjectionMatrix().Inverse();
	//Float4x4 cameraLocalToWorldMatrix = pCamera->GetTransform()->GetLocalToWorldMatrix();
	//Float3 corners[3] = { Float3(-1, -1, 0), Float3(1, -1, 0), Float3(1, 1, 1)};
	//for (uint32_t i = 0; i < 3; i++)
	//{
	//	Float4 temp = clipToCameraLocalMatrix * Float4(corners[i], 1);
	//	corners[i] = Float3(temp) / temp.w;
	//	corners[i] = Float3(cameraLocalToWorldMatrix * Float4(corners[i], 1));
	//}
	//Float3& planeSupport = corners[0];
	//Float3 planeNormal = Float3::Cross(corners[2] - corners[0], corners[1] - corners[0]).Normalize();
	//
	//
	//Float3 lineSupports[4] =
	//{
	//	Float3(lightLocalToWorldMatrix * Float4(-0.5f * m_size, -0.5f * m_size, 0, 1)),
	//	Float3(lightLocalToWorldMatrix * Float4(-0.5f * m_size,  0.5f * m_size, 0, 1)),
	//	Float3(lightLocalToWorldMatrix * Float4( 0.5f * m_size, -0.5f * m_size, 0, 1)),
	//	Float3(lightLocalToWorldMatrix * Float4( 0.5f * m_size,  0.5f * m_size, 0, 1))
	//};
	//
	//float maxDistance = mathf::min;
	//for (uint32_t i = 0; i < 4; i++)
	//{
	//	Float3 lineSupport = lineSupports[i];
	//	Float3 lineDirection = m_direction;
	//	Float3 intersection = geometry3d::LinePlaneIntersection(lineSupport, lineDirection, planeSupport, planeNormal);
	//	float distance = (intersection - lineSupport).Length();
	//	maxDistance = mathf::Max(maxDistance, distance);
	//	Graphics::DrawSphere(intersection, 0.3f, Float4::red);
	//	Graphics::DrawSphere(lineSupport, 0.3f, Float4::blue);
	//}
	//return maxDistance;
}