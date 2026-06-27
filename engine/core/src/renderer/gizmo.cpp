#include "gizmo.h"
#include "material.h"
#include "materialManager.h"
#include "mesh.h"
#include "primitives.h"
#include "shaderProperties.h"



namespace emberCore
{
	// Static members:
	Float4 Gizmo::s_color = Float4::white;
	Material Gizmo::s_material;



	// Private methods:
	void Gizmo::Init()
	{
		s_material = MaterialManager::GetMaterial("gizmoMaterial");
	}
	void Gizmo::Clear()
	{
		s_material = Material();
	}



	// Public methods:
	void Gizmo::DrawMesh(const Mesh& mesh, const Float4x4& localToWorldMatrix)
	{
		ShaderProperties shaderProperties = Primitives::DrawMesh(mesh, s_material, localToWorldMatrix, false, false, true);
		shaderProperties.SetValue("SurfaceProperties", "diffuseColor", s_color);
	}
	void Gizmo::DrawQuad(const Float4x4& localToWorldMatrix)
	{
		ShaderProperties shaderProperties = Primitives::DrawQuad(localToWorldMatrix, s_material, false, false, true);
		shaderProperties.SetValue("SurfaceProperties", "diffuseColor", s_color);
	}
	void Gizmo::DrawCube(const Float4x4& localToWorldMatrix)
	{
		ShaderProperties shaderProperties = Primitives::DrawCube(localToWorldMatrix, s_material, false, false, true);
		shaderProperties.SetValue("SurfaceProperties", "diffuseColor", s_color);
	}
	void Gizmo::DrawSphere(const Float4x4& localToWorldMatrix)
	{
		ShaderProperties shaderProperties = Primitives::DrawSphere(localToWorldMatrix, s_material, false, false, true);
		shaderProperties.SetValue("SurfaceProperties", "diffuseColor", s_color);
	}
	void Gizmo::DrawLineSegment(const Float3& start, const Float3& end, float width)
	{
		ShaderProperties shaderProperties = Primitives::DrawLineSegment(start, end, width, s_material, false, false, true);
		shaderProperties.SetValue("SurfaceProperties", "diffuseColor", s_color);
	}
	void Gizmo::DrawArrow(const Float3& position, const Float3& direction, float size)
	{
		ShaderProperties shaderProperties = Primitives::DrawArrow(position, direction, size, s_material, false, false, true);
		shaderProperties.SetValue("SurfaceProperties", "diffuseColor", s_color);
	}
	void Gizmo::DrawFrustum(const Float4x4& localToWorldMatrix, const Float4x4& projectionMatrix, float width)
	{
		Primitives::DrawFrustum(localToWorldMatrix, projectionMatrix, width, s_material, s_color, false, false, true);
	}
	void Gizmo::DrawBounds(const Float4x4& localToWorldMatrix, const Bounds2d& bounds, float width)
	{
		Primitives::DrawBounds(localToWorldMatrix, bounds, width, s_material, s_color, false, false, true);
	}
	void Gizmo::DrawBounds(const Float4x4& localToWorldMatrix, const Bounds& bounds, float width)
	{
		Primitives::DrawBounds(localToWorldMatrix, bounds, width, s_material, s_color, false, false, true);
	}



	// Setters:
	void Gizmo::SetColor(const Float4& color)
	{
		s_color = color;
	}



	// Getters:
	Float4 Gizmo::GetColor()
	{
		return s_color;
	}
}
