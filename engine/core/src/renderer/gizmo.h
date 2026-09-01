#pragma once
#include "commonCullMode.h"
#include "emberCoreExport.h"
#include "emberMath.h"



namespace emberCore
{
	// Forward declarations:
	class Core;
	class Mesh;
	class GizmoMaterial;
	class ShaderProperties;



	class EMBER_CORE_API Gizmo
	{
		// Friends:
		friend class Core;  // for Init()/Clear().

	private: // Members:
		static GizmoMaterial s_material;
		static GizmoMaterial s_opaqueMaterial;
		static GizmoMaterial s_transparentMaterial;
		static bool s_isLit;
		static emberCommon::CullMode s_cullMode;
		static Float4 s_color;

	private: // Methods:
		static void Init();
		static void Clear();

	public: // Methods:
		static void DrawMesh(const Mesh& mesh, const Float4x4& localToWorldMatrix);
		static void DrawQuad(const Float4x4& localToWorldMatrix);
		static void DrawCube(const Float4x4& localToWorldMatrix);
		static void DrawSphere(const Float4x4& localToWorldMatrix);
		static void DrawConeSmooth(const Float4x4& localToWorldMatrix);
		static void DrawConeFlat(const Float4x4& localToWorldMatrix);
		static void DrawLineSegment(const Float3& start, const Float3& end, float width = 0.1f);
		static void DrawArrow(const Float3& position, const Float3& direction, float size = 1.0f);
		static void DrawCapsule(const Capsule& capsule);
		static void DrawFrustum(const Float4x4& localToWorldMatrix, const Float4x4& projectionMatrix, float width = 0.1f);
		static void DrawBounds(const Float4x4& localToWorldMatrix, const Bounds2d& bounds, float width = 0.1f);
		static void DrawBounds(const Float4x4& localToWorldMatrix, const Bounds& bounds, float width = 0.1f);
		static void DrawRotatedBounds(const Float4x4& localToWorldMatrix, const RotatedBounds& bounds, float width = 0.1f);

		// Setters:
		static void SetIsOpaque(bool isOpaque);
		static void SetIsTransparent(bool isTransparent);
		static void SetIsLit(bool isLit);
		static void SetCullMode(emberCommon::CullMode cullMode);
		static void SetColor(const Float4& color);

		// Getters:
		static bool GetIsOpaque();
		static bool GetIsTransparent();
		static bool GetIsLit();
		static emberCommon::CullMode GetCullMode();
		static Float4 GetColor();
	};
}