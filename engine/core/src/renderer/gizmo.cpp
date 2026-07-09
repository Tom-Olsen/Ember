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
	emberCommon::CullMode Gizmo::s_cullMode = emberCommon::CullMode::back;
	Material Gizmo::s_defaultMaterial;
	Material Gizmo::s_material;



	// Private methods:
	void Gizmo::Init()
	{
		s_defaultMaterial = MaterialManager::GetMaterial("gizmoUnlitMaterial");
		s_material = s_defaultMaterial;
	}
	void Gizmo::Clear()
	{
		s_material = Material();
	}



	// Public methods:
	ShaderProperties Gizmo::DrawMesh(const Mesh& mesh, const Float4x4& localToWorldMatrix)
	{
		ShaderProperties shaderProperties = Primitives::DrawMesh(mesh, s_material, localToWorldMatrix, false, false, true, s_cullMode);
		shaderProperties.SetValue("SurfaceProperties", "diffuseColor", s_color);
        return shaderProperties;
	}
	ShaderProperties Gizmo::DrawQuad(const Float4x4& localToWorldMatrix)
	{
		ShaderProperties shaderProperties = Primitives::DrawQuad(localToWorldMatrix, s_material, false, false, true, s_cullMode);
		shaderProperties.SetValue("SurfaceProperties", "diffuseColor", s_color);
        return shaderProperties;
	}
	ShaderProperties Gizmo::DrawCube(const Float4x4& localToWorldMatrix)
	{
		ShaderProperties shaderProperties = Primitives::DrawCube(localToWorldMatrix, s_material, false, false, true, s_cullMode);
		shaderProperties.SetValue("SurfaceProperties", "diffuseColor", s_color);
        return shaderProperties;
	}
	ShaderProperties Gizmo::DrawSphere(const Float4x4& localToWorldMatrix)
	{
		ShaderProperties shaderProperties = Primitives::DrawSphere(localToWorldMatrix, s_material, false, false, true, s_cullMode);
		shaderProperties.SetValue("SurfaceProperties", "diffuseColor", s_color);
        return shaderProperties;
	}
	ShaderProperties Gizmo::DrawConeSmooth(const Float4x4& localToWorldMatrix)
    {
        ShaderProperties shaderProperties = Primitives::DrawConeSmooth(localToWorldMatrix, s_material, false, false, true, s_cullMode);
        shaderProperties.SetValue("SurfaceProperties", "diffuseColor", s_color);
        return shaderProperties;
    }
	ShaderProperties Gizmo::DrawConeFlat(const Float4x4& localToWorldMatrix)
    {
        ShaderProperties shaderProperties = Primitives::DrawConeFlat(localToWorldMatrix, s_material, false, false, true, s_cullMode);
        shaderProperties.SetValue("SurfaceProperties", "diffuseColor", s_color);
        return shaderProperties;
    }
	ShaderProperties Gizmo::DrawLineSegment(const Float3& start, const Float3& end, float width)
	{
		ShaderProperties shaderProperties = Primitives::DrawLineSegment(start, end, width, s_material, false, false, true, s_cullMode);
		shaderProperties.SetValue("SurfaceProperties", "diffuseColor", s_color);
        return shaderProperties;
	}
	ShaderProperties Gizmo::DrawArrow(const Float3& position, const Float3& direction, float size)
	{
		ShaderProperties shaderProperties = Primitives::DrawArrow(position, direction, size, s_material, false, false, true, s_cullMode);
		shaderProperties.SetValue("SurfaceProperties", "diffuseColor", s_color);
        return shaderProperties;
	}
	void Gizmo::DrawCapsule(const Capsule& capsule)
	{
		Primitives::DrawCapsule(capsule, s_material, s_color, false, false, true, s_cullMode);
	}
	void Gizmo::DrawFrustum(const Float4x4& localToWorldMatrix, const Float4x4& projectionMatrix, float width)
	{
		Primitives::DrawFrustum(localToWorldMatrix, projectionMatrix, width, s_material, s_color, false, false, true, s_cullMode);
	}
	void Gizmo::DrawBounds(const Float4x4& localToWorldMatrix, const Bounds2d& bounds, float width)
	{
		Primitives::DrawBounds(localToWorldMatrix, bounds, width, s_material, s_color, false, false, true, s_cullMode);
	}
	void Gizmo::DrawBounds(const Float4x4& localToWorldMatrix, const Bounds& bounds, float width)
	{
		Primitives::DrawBounds(localToWorldMatrix, bounds, width, s_material, s_color, false, false, true, s_cullMode);
	}
	void Gizmo::DrawRotatedBounds(const Float4x4& localToWorldMatrix, const RotatedBounds& bounds, float width)
	{
		Primitives::DrawRotatedBounds(localToWorldMatrix, bounds, width, s_material, s_color, false, false, true, s_cullMode);
	}



	// Setters:
	void Gizmo::SetColor(const Float4& color)
	{
		s_color = color;
	}
	void Gizmo::SetCullMode(emberCommon::CullMode cullMode)
	{
		s_cullMode = cullMode;
	}
    void Gizmo::SetMaterial(const Material& material)
    {
        s_material = material;
    }
    void Gizmo::ResetMaterial()
    {
        s_material = s_defaultMaterial;
    }
    void Gizmo::ResetCullMode()
    {
        s_cullMode = emberCommon::CullMode::back;
    }



	// Getters:
	Float4 Gizmo::GetColor()
	{
		return s_color;
	}
	emberCommon::CullMode Gizmo::GetCullMode()
	{
		return s_cullMode;
	}
    Material Gizmo::GetMaterial()
    {
        return s_material;
    }
    Material Gizmo::GetDefaultMaterial()
    {
        return s_defaultMaterial;
    }
}