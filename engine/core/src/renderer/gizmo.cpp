#include "gizmo.h"
#include "material.h"
#include "materialManager.h"
#include "mesh.h"
#include "meshManager.h"
#include "renderer.h"
#include "shaderProperties.h"



namespace emberCore
{
	// Static members:
	GizmoMaterial Gizmo::s_material;
	GizmoMaterial Gizmo::s_opaqueMaterial;
	GizmoMaterial Gizmo::s_transparentMaterial;
	bool Gizmo::s_isLit = true;
	emberCommon::CullMode Gizmo::s_cullMode = emberCommon::CullMode::back;
	Float4 Gizmo::s_color = Float4::white;



	// Private methods:
	void Gizmo::Init()
	{
		s_material = MaterialManager::TryGetGizmoMaterial("gizmoMaterial");
		s_opaqueMaterial = s_material;
		s_transparentMaterial = s_opaqueMaterial.CloneWithDefaultBindings("gizmoMaterialTransparent", emberCommon::GizmoRenderMode::transparent);
	}
	void Gizmo::Clear()
	{
		s_material = GizmoMaterial();
		s_opaqueMaterial = GizmoMaterial();
		s_transparentMaterial = GizmoMaterial();
	}



	// Public methods:
	void Gizmo::DrawMesh(const Mesh& mesh, const Float4x4& localToWorldMatrix)
	{
		if (s_cullMode == emberCommon::CullMode::count)
			s_cullMode = s_material.GetCullMode();
		ShaderProperties shaderProperties = Renderer::DrawGizmo(localToWorldMatrix, mesh, s_material, s_cullMode);
		shaderProperties.SetValue("SurfaceProperties", "surface_diffuseColor", s_color);
		shaderProperties.SetValue("SurfaceProperties", "surface_isLit", s_isLit);
	}
	void Gizmo::DrawQuad(const Float4x4& localToWorldMatrix)
	{
		static Mesh& quadMesh = MeshManager::GetMesh("quad");
		DrawMesh(quadMesh, localToWorldMatrix);
	}
	void Gizmo::DrawCube(const Float4x4& localToWorldMatrix)
	{
		static Mesh& cubeMesh = MeshManager::GetMesh("cube");
		DrawMesh(cubeMesh, localToWorldMatrix);
	}
	void Gizmo::DrawSphere(const Float4x4& localToWorldMatrix)
	{
		static Mesh& cubeSphereMesh = MeshManager::GetMesh("cubeSphere");
		DrawMesh(cubeSphereMesh, localToWorldMatrix);
	}
	void Gizmo::DrawConeSmooth(const Float4x4& localToWorldMatrix)
    {
		static Mesh& coneSmoothMesh = MeshManager::GetMesh("coneSmooth");
		DrawMesh(coneSmoothMesh, localToWorldMatrix);
    }
	void Gizmo::DrawConeFlat(const Float4x4& localToWorldMatrix)
    {
		static Mesh& coneFlatMesh = MeshManager::GetMesh("coneFlat");
		DrawMesh(coneFlatMesh, localToWorldMatrix);
    }
	void Gizmo::DrawLineSegment(const Float3& start, const Float3& end, float width)
	{
		static Mesh& zylinderFlatMesh = MeshManager::GetMesh("zylinderFlat");

		Float3 direction = end - start;
		if (direction.IsEpsilonZero())
			return;

		float length = direction.Length();
		Float3 position = start + 0.5f * direction;
		Float3x3 rotationMatrix = Float3x3::RotateFromTo(Float3::up, direction);
		Float4x4 localToWorldMatrix = Float4x4::TRS(position, rotationMatrix, Float3(width, width, length));
		DrawMesh(zylinderFlatMesh, localToWorldMatrix);
	}
	void Gizmo::DrawArrow(const Float3& position, const Float3& direction, float size)
	{
		static Mesh& arrowFlatMesh = MeshManager::GetMesh("arrowFlat");;

		if (direction.IsEpsilonZero())
			return;

		Float3x3 rotationMatrix = Float3x3::RotateFromTo(Float3::forward, direction);
		Float4x4 localToWorldMatrix = Float4x4::TRS(position, rotationMatrix, Float3(size));
		
		DrawMesh(arrowFlatMesh, localToWorldMatrix);
	}
	void Gizmo::DrawCapsule(const Capsule& capsule)
	{
		static Mesh& halfCubeSphereMesh = MeshManager::GetMesh("halfCubeSphere");

		if (capsule.radius <= 0.0f)
			return;

		float diameter = 2.0f * capsule.radius;
		Float3 scale(diameter);
		if (capsule.point0.IsEpsilonEqual(capsule.point1))
		{
			DrawSphere(Float4x4::TRS(capsule.point0, Float3x3::identity, scale));
			return;
		}

		Float3 direction = capsule.point1 - capsule.point0;
		Float3x3 point1Rotation = Float3x3::RotateFromTo(Float3::forward, direction);
		Float3x3 point0Rotation = point1Rotation * Float3x3::rot180x;

		DrawMesh(halfCubeSphereMesh, Float4x4::TRS(capsule.point0, point0Rotation, scale));
		DrawMesh(halfCubeSphereMesh, Float4x4::TRS(capsule.point1, point1Rotation, scale));
		DrawLineSegment(capsule.point0, capsule.point1, diameter);
	}
	void Gizmo::DrawFrustum(const Float4x4& localToWorldMatrix, const Float4x4& projectionMatrix, float width)
	{
		Float4 cornerPoints[8] =
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

		float det = projectionMatrix.Determinant();
		if (det == 0.0f)
			return;

		Float4x4 invProjectionMatrix = projectionMatrix.Inverse(det);
		for (uint32_t i = 0; i < 8; i++)
		{
			cornerPoints[i] = invProjectionMatrix * cornerPoints[i];
			cornerPoints[i] /= cornerPoints[i].w;
			cornerPoints[i] = localToWorldMatrix * cornerPoints[i];
		}

		for (uint32_t i = 0; i < 8; i++)
			DrawSphere(Float4x4::TRS(Float3(cornerPoints[i]), Float3x3::identity, 2.0f * width));

		auto line = [&](uint32_t a, uint32_t b)
		{ DrawLineSegment(Float3(cornerPoints[a]), Float3(cornerPoints[b]), width); };

		// Horizontal/Vertical/Depth:
		line(0, 4); line(1, 5); line(2, 6); line(3, 7);
		line(0, 2); line(1, 3); line(4, 6); line(5, 7);
		line(0, 1); line(2, 3); line(4, 5); line(6, 7);
	}
	void Gizmo::DrawBounds(const Float4x4& localToWorldMatrix, const Bounds2d& bounds, float width)
	{
		std::array<Float2, 4> corners = bounds.GetCorners();

		for (uint32_t i = 0; i < 4; i++)
		{
			Float3 pos = Float3(localToWorldMatrix * Float4(corners[i], 0.0f, 1.0f));
			DrawSphere(Float4x4::TRS(pos, Float3x3::identity, width));
		}

		auto line = [&](uint32_t a, uint32_t b)
		{
			Float3 pa = Float3(localToWorldMatrix * Float4(corners[a], 0.0f, 1.0f));
			Float3 pb = Float3(localToWorldMatrix * Float4(corners[b], 0.0f, 1.0f));
			DrawLineSegment(pa, pb, width);
		};

		line(0, 1); line(1, 3); line(3, 2); line(2, 0);
	}
	void Gizmo::DrawBounds(const Float4x4& localToWorldMatrix, const Bounds& bounds, float width)
	{
		std::array<Float3, 8> corners = bounds.GetCorners();

		for (uint32_t i = 0; i < 8; i++)
		{
			Float3 pos = Float3(localToWorldMatrix * Float4(corners[i], 1.0f));
			DrawSphere(Float4x4::TRS(pos, Float3x3::identity, width));
		}

		auto line = [&](uint32_t a, uint32_t b)
		{
			Float3 pa = Float3(localToWorldMatrix * Float4(corners[a], 1.0f));
			Float3 pb = Float3(localToWorldMatrix * Float4(corners[b], 1.0f));
			DrawLineSegment(pa, pb, width);
		};

		// Horizontal/Vertical/Depth:
		line(0, 4); line(1, 5); line(2, 6); line(3, 7);
		line(0, 2); line(1, 3); line(4, 6); line(5, 7);
		line(0, 1); line(2, 3); line(4, 5); line(6, 7);
	}
	void Gizmo::DrawRotatedBounds(const Float4x4& localToWorldMatrix, const RotatedBounds& bounds, float width)
	{
		std::array<Float3, 8> corners = bounds.GetCorners();

		for (uint32_t i = 0; i < 8; i++)
		{
			Float3 pos = Float3(localToWorldMatrix * Float4(corners[i], 1.0f));
			DrawSphere(Float4x4::TRS(pos, Float3x3::identity, width));
		}

		auto line = [&](uint32_t a, uint32_t b)
		{
			Float3 pa = Float3(localToWorldMatrix * Float4(corners[a], 1.0f));
			Float3 pb = Float3(localToWorldMatrix * Float4(corners[b], 1.0f));
			DrawLineSegment(pa, pb, width);
		};

		// Horizontal/Vertical/Depth:
		line(0, 4); line(1, 5); line(2, 6); line(3, 7);
		line(0, 2); line(1, 3); line(4, 6); line(5, 7);
		line(0, 1); line(2, 3); line(4, 5); line(6, 7);
	}



	// Setters:
	void Gizmo::SetIsOpaque(bool isOpaque)
	{
		if (isOpaque)
			s_material = s_opaqueMaterial;
		else
			s_material = s_transparentMaterial;
	}
	void Gizmo::SetIsTransparent(bool isTransparent)
	{
		if (isTransparent)
			s_material = s_transparentMaterial;
		else
			s_material = s_opaqueMaterial;
	}
	void Gizmo::SetIsLit(bool isLit)
	{
		s_isLit = isLit;
	}
	void Gizmo::SetCullMode(emberCommon::CullMode cullMode)
	{
		s_cullMode = cullMode;
	}
	void Gizmo::SetColor(const Float4& color)
	{
		s_color = color;
	}



	// Getters:
	bool Gizmo::GetIsOpaque()
	{
		return s_material.GetRenderMode() == s_opaqueMaterial.GetRenderMode();
	}
	bool Gizmo::GetIsTransparent()
	{
		return s_material.GetRenderMode() == s_transparentMaterial.GetRenderMode();
	}
	bool Gizmo::GetIsLit()
	{
		return s_isLit;
	}
	emberCommon::CullMode Gizmo::GetCullMode()
	{
		return s_cullMode;
	}
	Float4 Gizmo::GetColor()
	{
		return s_color;
	}
}