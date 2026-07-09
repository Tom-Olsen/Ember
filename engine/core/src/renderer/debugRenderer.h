#pragma once
#include "emberCoreExport.h"
#include "emberMath.h"



namespace emberCore
{
	// Forward declarations:
	class Core;
	class Material;
	class Mesh;
	class ShaderProperties;



	class EMBER_CORE_API DebugRenderer
    {
		// Friends:
		friend class Core;  // for Init()/Clear().

    private: // Members:
        static inline bool s_receiveShadows = true;
        static inline bool s_castShadows = true;
        static inline Float4 s_color = Float4::white;
        static Material s_material;

    private: // Methods:
        static void Init();
        static void Clear();

    public: // Methods:
		static ShaderProperties DrawMesh(const Mesh& mesh, const Float4x4& localToWorldMatrix);
		static ShaderProperties DrawQuad(const Float4x4& localToWorldMatrix);
		static ShaderProperties DrawCube(const Float4x4& localToWorldMatrix);
		static ShaderProperties DrawSphere(const Float4x4& localToWorldMatrix);
		static ShaderProperties DrawConeSmooth(const Float4x4& localToWorldMatrix);
		static ShaderProperties DrawConeFlat(const Float4x4& localToWorldMatrix);
		static ShaderProperties DrawLineSegment(const Float3& start, const Float3& end, float width = 0.1f);
		static ShaderProperties DrawArrow(const Float3& position, const Float3& direction, float size = 1.0f);
		static void DrawCapsule(const Capsule& capsule);
		static void DrawFrustum(const Float4x4& localToWorldMatrix, const Float4x4& projectionMatrix, float width = 0.1f);
		static void DrawBounds(const Float4x4& localToWorldMatrix, const Bounds2d& bounds, float width = 0.1f);
		static void DrawBounds(const Float4x4& localToWorldMatrix, const Bounds& bounds, float width = 0.1f);
		static void DrawRotatedBounds(const Float4x4& localToWorldMatrix, const RotatedBounds& bounds, float width = 0.1f);

        // Setters:
        static void SetReceiveShadows(bool value);
        static void SetCastShadows(bool value);
        static void SetColor(const Float4& color);
        static void SetMaterial(const Material& material);

        // Getters:
        static bool GetReceiveShadows();
        static bool GetCastShadows();
        static Float4 GetColor();
        static Material GetMaterial();
    };
}