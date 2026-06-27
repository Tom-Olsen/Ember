#include "primitives.h"
#include "material.h"
#include "mesh.h"
#include "meshManager.h"
#include "renderer.h"
#include "shaderProperties.h"
#include <array>



namespace emberCore
{
	ShaderProperties Primitives::DrawMesh(const Mesh& mesh, const Material& material, const Float4x4& localToWorldMatrix, bool receiveShadows, bool castShadows, bool asGizmo)
	{
		if (asGizmo)
			return Renderer::DrawGizmo(mesh, material, localToWorldMatrix);
		else
			return Renderer::DrawMesh(mesh, material, localToWorldMatrix, receiveShadows, castShadows);
	}

    ShaderProperties Primitives::DrawQuad(const Float4x4& localToWorldMatrix, const Material& material, bool receiveShadows, bool castShadows, bool asGizmo)
    {
        return DrawMesh(MeshManager::GetMesh("quad"), material, localToWorldMatrix, receiveShadows, castShadows, asGizmo);
    }

	ShaderProperties Primitives::DrawCube(const Float4x4& localToWorldMatrix, const Material& material, bool receiveShadows, bool castShadows, bool asGizmo)
	{
		return DrawMesh(MeshManager::GetMesh("cube"), material, localToWorldMatrix, receiveShadows, castShadows, asGizmo);
	}

	ShaderProperties Primitives::DrawSphere(const Float4x4& localToWorldMatrix, const Material& material, bool receiveShadows, bool castShadows, bool asGizmo)
	{
		return DrawMesh(MeshManager::GetMesh("cubeSphere"), material, localToWorldMatrix, receiveShadows, castShadows, asGizmo);
	}

	ShaderProperties Primitives::DrawLineSegment(const Float3& start, const Float3& end, float width, const Material& material, bool receiveShadows, bool castShadows, bool asGizmo)
	{
		Float3 direction = end - start;
		if (direction.IsEpsilonZero())
			return ShaderProperties();

		float length = direction.Length();
		Float3 position = start + 0.5f * direction;
		Float3x3 rotationMatrix = Float3x3::RotateFromTo(Float3::up, direction);
		Float4x4 localToWorldMatrix = Float4x4::TRS(position, rotationMatrix, Float3(width, width, length));
		return DrawMesh(MeshManager::GetMesh("zylinderFlat"), material, localToWorldMatrix, receiveShadows, castShadows, asGizmo);
	}

	ShaderProperties Primitives::DrawArrow(const Float3& position, const Float3& direction, float size, const Material& material, bool receiveShadows, bool castShadows, bool asGizmo)
	{
		if (direction.IsEpsilonZero())
			return ShaderProperties();

		Float3x3 rotationMatrix = Float3x3::RotateFromTo(Float3::forward, direction);
		Float4x4 localToWorldMatrix = Float4x4::TRS(position, rotationMatrix, Float3(size));
		return DrawMesh(MeshManager::GetMesh("arrowFlat"), material, localToWorldMatrix, receiveShadows, castShadows, asGizmo);
	}

	void Primitives::DrawFrustum(const Float4x4& localToWorldMatrix, const Float4x4& projectionMatrix, float width, const Material& material, const Float4& color, bool receiveShadows, bool castShadows, bool asGizmo)
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
		{
			Float4x4 ltw = Float4x4::TRS(Float3(cornerPoints[i]), Float3x3::identity, 2.0f * width);
			ShaderProperties shaderProperties = DrawSphere(ltw, material, receiveShadows, castShadows, asGizmo);
			shaderProperties.SetValue("SurfaceProperties", "diffuseColor", Float4::gray);
		}

		auto line = [&](uint32_t a, uint32_t b)
		{
			ShaderProperties shaderProperties = DrawLineSegment(Float3(cornerPoints[a]), Float3(cornerPoints[b]), width, material, receiveShadows, castShadows, asGizmo);
			shaderProperties.SetValue("SurfaceProperties", "diffuseColor", color);
		};

		// Horizontal:
		line(0, 4); line(1, 5); line(2, 6); line(3, 7);
		// Vertical:
		line(0, 2); line(1, 3); line(4, 6); line(5, 7);
		// Depth:
		line(0, 1); line(2, 3); line(4, 5); line(6, 7);
	}

	void Primitives::DrawBounds(const Float4x4& localToWorldMatrix, const Bounds2d& bounds, float width, const Material& material, const Float4& color, bool receiveShadows, bool castShadows, bool asGizmo)
	{
		std::array<Float2, 4> corners = bounds.GetCorners();

		for (uint32_t i = 0; i < 4; i++)
		{
			Float3 pos = Float3(localToWorldMatrix * Float4(corners[i], 0.0f, 1.0f));
			ShaderProperties shaderProperties = DrawSphere(Float4x4::TRS(pos, Float3x3::identity, width), material, receiveShadows, castShadows, asGizmo);
			shaderProperties.SetValue("SurfaceProperties", "diffuseColor", color);
		}

		auto line = [&](uint32_t a, uint32_t b)
		{
			Float3 pa = Float3(localToWorldMatrix * Float4(corners[a], 0.0f, 1.0f));
			Float3 pb = Float3(localToWorldMatrix * Float4(corners[b], 0.0f, 1.0f));
			ShaderProperties shaderProperties = DrawLineSegment(pa, pb, width, material, receiveShadows, castShadows, asGizmo);
			shaderProperties.SetValue("SurfaceProperties", "diffuseColor", color);
		};

		line(0, 1); line(1, 3); line(3, 2); line(2, 0);
	}

	void Primitives::DrawBounds(const Float4x4& localToWorldMatrix, const Bounds& bounds, float width, const Material& material, const Float4& color, bool receiveShadows, bool castShadows, bool asGizmo)
	{
		std::array<Float3, 8> corners = bounds.GetCorners();

		for (uint32_t i = 0; i < 8; i++)
		{
			Float3 pos = Float3(localToWorldMatrix * Float4(corners[i], 1.0f));
			ShaderProperties shaderProperties = DrawSphere(Float4x4::TRS(pos, Float3x3::identity, width), material, receiveShadows, castShadows, asGizmo);
			shaderProperties.SetValue("SurfaceProperties", "diffuseColor", color);
		}

		auto line = [&](uint32_t a, uint32_t b)
		{
			Float3 pa = Float3(localToWorldMatrix * Float4(corners[a], 1.0f));
			Float3 pb = Float3(localToWorldMatrix * Float4(corners[b], 1.0f));
			ShaderProperties shaderProperties = DrawLineSegment(pa, pb, width, material, receiveShadows, castShadows, asGizmo);
			shaderProperties.SetValue("SurfaceProperties", "diffuseColor", color);
		};

		// Horizontal:
		line(0, 4); line(1, 5); line(2, 6); line(3, 7);
		// Vertical:
		line(0, 2); line(1, 3); line(4, 6); line(5, 7);
		// Depth:
		line(0, 1); line(2, 3); line(4, 5); line(6, 7);
	}
}
