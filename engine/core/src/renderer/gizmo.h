#pragma once
#include "emberCoreExport.h"
#include "emberMath.h"



namespace emberEngine
{
	// Forward declarations:
	class Core;
	class Material;
	class Mesh;
	class ShaderProperties;



	class EMBER_CORE_API Gizmo
    {
		// Friends:
		friend class Core;  // for Init()/Clear().

    private: // Members:
        static Float4 s_color;
        static Material s_material;

    private: // Methods:
        static void Init();
        static void Clear();

    public: // Methods:
		static void DrawMesh(const Mesh& mesh, const Float4x4& localToWorldMatrix);
		static void DrawLineSegment(const Float3& start, const Float3& end, float width = 0.1f);
		static void DrawArrow(const Float3& position, const Float3& direction, float size = 1.0f);
		static void DrawCube(const Float4x4& localToWorldMatrix);
		static void DrawSphere(const Float4x4& localToWorldMatrix);
		static void DrawFrustum(const Float4x4& localToWorldMatrix, const Float4x4& projectionMatrix, float width = 0.1f);
		static void DrawBounds(const Float4x4& localToWorldMatrix, const Bounds2d& bounds, float width = 0.1f);
		static void DrawBounds(const Float4x4& localToWorldMatrix, const Bounds& bounds, float width = 0.1f);

        // Setters:
        static void SetColor(const Float4& color);

        // Getters:
        static Float4 GetColor();
    };
}