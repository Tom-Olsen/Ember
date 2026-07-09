#pragma once
#include "commonCullMode.h"
#include "emberMath.h"



namespace emberCore
{
	// Forward declarations:
	class Material;
	class Mesh;
	class ShaderProperties;



	// Internal helper. Routes primitive draw calls to Renderer::DrawMesh or Renderer::DrawGizmo.
	class Primitives
	{
	public: // Methods:
		static ShaderProperties DrawMesh(const Mesh& mesh, const Material& material, const Float4x4& localToWorldMatrix, bool receiveShadows, bool castShadows, bool asGizmo, emberCommon::CullMode cullMode = emberCommon::CullMode::pipelineDefault);
		static ShaderProperties DrawQuad(const Float4x4& localToWorldMatrix, const Material& material, bool receiveShadows, bool castShadows, bool asGizmo, emberCommon::CullMode cullMode = emberCommon::CullMode::pipelineDefault);
		static ShaderProperties DrawCube(const Float4x4& localToWorldMatrix, const Material& material, bool receiveShadows, bool castShadows, bool asGizmo, emberCommon::CullMode cullMode = emberCommon::CullMode::pipelineDefault);
		static ShaderProperties DrawSphere(const Float4x4& localToWorldMatrix, const Material& material, bool receiveShadows, bool castShadows, bool asGizmo, emberCommon::CullMode cullMode = emberCommon::CullMode::pipelineDefault);
        static ShaderProperties DrawConeSmooth(const Float4x4& localToWorldMatrix, const Material& material, bool receiveShadows, bool castShadows, bool asGizmo, emberCommon::CullMode cullMode = emberCommon::CullMode::pipelineDefault);
        static ShaderProperties DrawConeFlat(const Float4x4& localToWorldMatrix, const Material& material, bool receiveShadows, bool castShadows, bool asGizmo, emberCommon::CullMode cullMode = emberCommon::CullMode::pipelineDefault);
		static ShaderProperties DrawLineSegment(const Float3& start, const Float3& end, float width, const Material& material, bool receiveShadows, bool castShadows, bool asGizmo, emberCommon::CullMode cullMode = emberCommon::CullMode::pipelineDefault);
		static ShaderProperties DrawArrow(const Float3& position, const Float3& direction, float size, const Material& material, bool receiveShadows, bool castShadows, bool asGizmo, emberCommon::CullMode cullMode = emberCommon::CullMode::pipelineDefault);
		static void DrawCapsule(const Capsule& capsule, const Material& material, const Float4& color, bool receiveShadows, bool castShadows, bool asGizmo, emberCommon::CullMode cullMode = emberCommon::CullMode::pipelineDefault);
		static void DrawFrustum(const Float4x4& localToWorldMatrix, const Float4x4& projectionMatrix, float width, const Material& material, const Float4& color, bool receiveShadows, bool castShadows, bool asGizmo, emberCommon::CullMode cullMode = emberCommon::CullMode::pipelineDefault);
		static void DrawBounds(const Float4x4& localToWorldMatrix, const Bounds2d& bounds, float width, const Material& material, const Float4& color, bool receiveShadows, bool castShadows, bool asGizmo, emberCommon::CullMode cullMode = emberCommon::CullMode::pipelineDefault);
		static void DrawBounds(const Float4x4& localToWorldMatrix, const Bounds& bounds, float width, const Material& material, const Float4& color, bool receiveShadows, bool castShadows, bool asGizmo, emberCommon::CullMode cullMode = emberCommon::CullMode::pipelineDefault);
		static void DrawRotatedBounds(const Float4x4& localToWorldMatrix, const RotatedBounds& bounds, float width, const Material& material, const Float4& color, bool receiveShadows, bool castShadows, bool asGizmo, emberCommon::CullMode cullMode = emberCommon::CullMode::pipelineDefault);
	};
}