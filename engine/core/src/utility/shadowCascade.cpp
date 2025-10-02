#include "shadowCascade.h"
#include "camera.h"
#include "emberEngine.h"



namespace emberEngine
{
	// static members:
	Float4 ShadowCascade::s_frustum_Clip[8] =
	{
		// Note: In vulkan y axes is flippes => -1 = top, 1 = bottom
		Float4(-1, -1, 0, 1),	// left top near
		Float4(-1, -1, 1, 1),	// left top far
		Float4(-1,  1, 0, 1),	// left bottom near
		Float4(-1,  1, 1, 1),	// left bottom far
		Float4( 1, -1, 0, 1),	// right top near
		Float4( 1, -1, 1, 1),	// right top far
		Float4( 1,  1, 0, 1),	// right bottom near
		Float4( 1,  1, 1, 1)	// right bottom far
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
		// Transformation matrizes:
		Float4x4 clipToCameraLocalMatrix = pCamera->GetProjectionMatrix().Inverse();
		Float4x4 cameraLocalToWorldMatrix = pCamera->GetTransform()->GetLocalToWorldMatrix();

		// Construct 2d plane orhtogonal to light direction, with coordinate system alignes with light local space:
		Float4x4 rotMatrix = Float4x4::RotateThreeLeg(Float3::down, m_direction, Float3::right, Float3::right);
		Float3 e1 = Float3(rotMatrix * Float4::right).Normalize();
		Float3 e2 = Float3::Cross(-m_direction, e1).Normalize();
		Float3x2 planeToWorld = Float3x2::Columns(e1, e2);
		Float2x3 worldToPlane = planeToWorld.LeftInverse();

		// Compute camera sub frustum in camera space and world space:
		ComputeSubFrustum(pCamera, nearDepth, farDepth, clipToCameraLocalMatrix, cameraLocalToWorldMatrix);

		// Compute orthographic projection size:
		float diagFar = (m_subFrustum_CameraLocal[7] - m_subFrustum_CameraLocal[1]).Length();
		float diagNearToFar = (m_subFrustum_CameraLocal[7] - m_subFrustum_CameraLocal[0]).Length();
		m_size = math::Max(diagNearToFar, diagFar);
		float texelSize = m_size / Renderer::GetShadowMapResolution();

		// Find center of sub frustum on orthographic projection plane:
		Bounds2d lightBounds(worldToPlane * m_subFrustum_World[0], Float2::zero);
		for (uint32_t i = 1; i < 8; i++)
		{
			Float2 projection = worldToPlane * m_subFrustum_World[i];
			lightBounds.Encapsulate(projection);
		}
		Float2 projectionCenter_Plane = lightBounds.center;

		// Optional: move projectionCenter_Plane to fit sub frustum better

		// Round projection center in plane coordinates and transform to world space:
		projectionCenter_Plane.x = math::Round(projectionCenter_Plane.x / texelSize) * texelSize;
		projectionCenter_Plane.y = math::Round(projectionCenter_Plane.y / texelSize) * texelSize;
		Float3 projectionCenter_World = planeToWorld * projectionCenter_Plane;

		// Compute light position by intersecting line from camera sub frustum center to scene ceiling plane along light direction:
		Float3 planeNormal = Float3::up;
		Float3 planeSupport = sceneHeight * Float3::up;
		m_position = geometry3d::LinePlaneIntersection(projectionCenter_World, m_direction, planeSupport, planeNormal);
	}
	void ShadowCascade::ComputeSubFrustum(Camera* const pCamera, float nearDepth, float farDepth, const Float4x4& clipToCameraLocalMatrix, const Float4x4& cameraLocalToWorldMatrix)
	{
		// Camera frustum in camera space:
		Float3 frustum_CameraLocal[8];
		for (uint32_t i = 0; i < 8; i++)
		{
			Float4 temp = clipToCameraLocalMatrix * s_frustum_Clip[i];
			frustum_CameraLocal[i] = Float3(temp) / temp.w;
		}

		// Camera sub frustum in camera space:
		// nearDepth, farDepth € [0,1] are the percentile splits of the camera frustum.
		m_subFrustum_CameraLocal[0] = frustum_CameraLocal[0] + nearDepth * (frustum_CameraLocal[1] - frustum_CameraLocal[0]);
		m_subFrustum_CameraLocal[1] = frustum_CameraLocal[0] + farDepth * (frustum_CameraLocal[1] - frustum_CameraLocal[0]);
		m_subFrustum_CameraLocal[2] = frustum_CameraLocal[2] + nearDepth * (frustum_CameraLocal[3] - frustum_CameraLocal[2]);
		m_subFrustum_CameraLocal[3] = frustum_CameraLocal[2] + farDepth * (frustum_CameraLocal[3] - frustum_CameraLocal[2]);
		m_subFrustum_CameraLocal[4] = frustum_CameraLocal[4] + nearDepth * (frustum_CameraLocal[5] - frustum_CameraLocal[4]);
		m_subFrustum_CameraLocal[5] = frustum_CameraLocal[4] + farDepth * (frustum_CameraLocal[5] - frustum_CameraLocal[4]);
		m_subFrustum_CameraLocal[6] = frustum_CameraLocal[6] + nearDepth * (frustum_CameraLocal[7] - frustum_CameraLocal[6]);
		m_subFrustum_CameraLocal[7] = frustum_CameraLocal[6] + farDepth * (frustum_CameraLocal[7] - frustum_CameraLocal[6]);

		// Camera sub frustum in world space:
		for (uint32_t i = 0; i < 8; i++)
			m_subFrustum_World[i] = Float3(cameraLocalToWorldMatrix * Float4(m_subFrustum_CameraLocal[i], 1.0f));
	}
	float ShadowCascade::ComputeFarClip(Camera* const pCamera, const Float4x4& lightLocalToWorldMatrix)
	{
		// Construct bounding box that encapsulates the sub frustum and the light source in light space:
		Bounds bounds = Bounds(Float3::zero, Float3::zero);	// lights source located at zero.
		Float4x4 cameraLocalToLightLocalMatrix = m_viewMatrix * pCamera->GetTransform()->GetLocalToWorldMatrix();
		Float3 subFrustum_LightLocal[8];
		for (uint32_t i = 0; i < 8; i++)
		{
			subFrustum_LightLocal[i] = Float3(cameraLocalToLightLocalMatrix * Float4(m_subFrustum_CameraLocal[i], 1.0f));
			bounds.Encapsulate(subFrustum_LightLocal[i]);
		}
		// Add a little extra (4 * frustum height) to prevent light clipping at some camera angles. Could need some improvement. 
		return bounds.GetSize().z + 4.0f * math::Abs(m_subFrustum_CameraLocal[5].y - m_subFrustum_CameraLocal[7].y);
	}
}