#include "debugRenderer.h"
#include "material.h"
#include "materialManager.h"
#include "mesh.h"
#include "primitives.h"
#include "shaderProperties.h"



namespace emberCore
{
	// Static members:
	Material DebugRenderer::s_material;



	// Private methods:
	void DebugRenderer::Init()
	{
		s_material = MaterialManager::GetMaterial("simpleUnlitMaterial");
	}
	void DebugRenderer::Clear()
	{
		s_material = Material();
	}



	// Public methods:
	ShaderProperties DebugRenderer::DrawMesh(const Mesh& mesh, const Float4x4& localToWorldMatrix)
	{
		ShaderProperties shaderProperties = Primitives::DrawMesh(mesh, s_material, localToWorldMatrix, s_receiveShadows, s_castShadows, false);
		shaderProperties.SetValue("SurfaceProperties", "diffuseColor", s_color);
		return shaderProperties;
	}
	ShaderProperties DebugRenderer::DrawLineSegment(const Float3& start, const Float3& end, float width)
	{
		ShaderProperties shaderProperties = Primitives::DrawLineSegment(start, end, width, s_material, s_receiveShadows, s_castShadows, false);
		shaderProperties.SetValue("SurfaceProperties", "diffuseColor", s_color);
		return shaderProperties;
	}
	ShaderProperties DebugRenderer::DrawArrow(const Float3& position, const Float3& direction, float size)
	{
		ShaderProperties shaderProperties = Primitives::DrawArrow(position, direction, size, s_material, s_receiveShadows, s_castShadows, false);
		shaderProperties.SetValue("SurfaceProperties", "diffuseColor", s_color);
		return shaderProperties;
	}
	ShaderProperties DebugRenderer::DrawCube(const Float4x4& localToWorldMatrix)
	{
		ShaderProperties shaderProperties = Primitives::DrawCube(localToWorldMatrix, s_material, s_receiveShadows, s_castShadows, false);
		shaderProperties.SetValue("SurfaceProperties", "diffuseColor", s_color);
		return shaderProperties;
	}
	ShaderProperties DebugRenderer::DrawSphere(const Float4x4& localToWorldMatrix)
	{
		ShaderProperties shaderProperties = Primitives::DrawSphere(localToWorldMatrix, s_material, s_receiveShadows, s_castShadows, false);
		shaderProperties.SetValue("SurfaceProperties", "diffuseColor", s_color);
		return shaderProperties;
	}
	void DebugRenderer::DrawFrustum(const Float4x4& localToWorldMatrix, const Float4x4& projectionMatrix, float width)
	{
		Primitives::DrawFrustum(localToWorldMatrix, projectionMatrix, width, s_material, s_color, s_receiveShadows, s_castShadows, false);
	}
	void DebugRenderer::DrawBounds(const Float4x4& localToWorldMatrix, const Bounds2d& bounds, float width)
	{
		Primitives::DrawBounds(localToWorldMatrix, bounds, width, s_material, s_color, s_receiveShadows, s_castShadows, false);
	}
	void DebugRenderer::DrawBounds(const Float4x4& localToWorldMatrix, const Bounds& bounds, float width)
	{
		Primitives::DrawBounds(localToWorldMatrix, bounds, width, s_material, s_color, s_receiveShadows, s_castShadows, false);
	}



	// Setters:
	void DebugRenderer::SetReceiveShadows(bool value)
	{
		s_receiveShadows = value;
	}
	void DebugRenderer::SetCastShadows(bool value)
	{
		s_castShadows = value;
	}
	void DebugRenderer::SetColor(const Float4& color)
	{
		s_color = color;
	}
	void DebugRenderer::SetMaterial(const Material& material)
	{
		s_material = material;
	}



	// Getters:
	bool DebugRenderer::GetReceiveShadows()
	{
		return s_receiveShadows;
	}
	bool DebugRenderer::GetCastShadows()
	{
		return s_castShadows;
	}
	Float4 DebugRenderer::GetColor()
	{
		return s_color;
	}
	Material DebugRenderer::GetMaterial()
	{
		return s_material;
	}
}