#ifndef __INCLUDE_GUARD_graphics_h__
#define __INCLUDE_GUARD_graphics_h__
#include "mathf.h"
#include <vector>



namespace emberEngine
{
	// Forward declarations:
	class Material;
	class MaterialProperties;
	class Mesh;
	class MeshRenderer;
	class Transform;
	struct VulkanContext;



	class Graphics
	{
	public: // Members

	private: // Members
		static uint32_t s_drawIndex;
		static bool s_isInitialized;
		static std::vector<Transform*> s_transforms;
		static std::vector<MeshRenderer*> s_meshRenderers;
		static Mesh* s_pLineSegmentMesh;
		static Mesh* s_pSphereMesh;
		static Mesh* s_pArrowMesh;
		static Material* s_pSimpleLit;
		static Material* s_pSimpleUnlit;

	public: // Methods
		static void Init();
		static void Clear();
		static void SetLineSegmentMesh(Mesh* pMesh);

		// Draw calls:
		// Draw mesh:
		static MaterialProperties* DrawMesh(Mesh* pMesh, Material* pMaterial, Float3 position, Float3x3 rotationMatrix = Float3x3::identity, float scale = 1.0f, bool receiveShadows = true, bool castShadows = true);
		static MaterialProperties* DrawMesh(Mesh* pMesh, Material* pMaterial, Float3 position, Float3x3 rotationMatrix = Float3x3::identity, Float3 scale = Float3::one, bool receiveShadows = true, bool castShadows = true);
		static MaterialProperties* DrawMesh(Mesh* pMesh, Material* pMaterial, Float4x4 localToWorldMatrix, bool receiveShadows = true, bool castShadows = true);

		// Draw line segment:
		static void DrawLineSegment(Float3 start, Float3 end, float width, Float4 color = Float4::one, bool receiveShadows = true, bool castShadows = true);
		static MaterialProperties* DrawLineSegment(Float3 start, Float3 end, float width, Material* pMaterial, bool receiveShadows = true, bool castShadows = true);


		// Speciaized draw calls:
		static void DrawSphere(Float3 position, float radius, Float4 color = Float4::white, bool receiveShadows = true, bool castShadows = true);
		static void DrawArrow(Float3 position, Float3 direction, float size = 1.0f, Float4 color = Float4::white, bool receiveShadows = true, bool castShadows = true);
		static void DrawFrustum(Float4x4 localToWorldMatrix, const Float4x4& projectionMatrix, float width = 0.1f, const Float4& color = Float4::white, bool receiveShadows = false, bool castShadows = false);
		static void DrawBounds(Float4x4 localToWorldMatrix, const Bounds& bounds, float width = 0.1f, const Float4& color = Float4::white, bool receiveShadows = false, bool castShadows = false);

		static void ResetDrawCalls();

		// Getters:
		static std::vector<MeshRenderer*>* GetSortedMeshRenderers();

	private: // Methods
		static void DoubleCapacityIfNeeded();
		static void ReduceCapacity();

		// Delete all constructors:
		Graphics() = delete;
		Graphics(const Graphics&) = delete;
		Graphics& operator=(const Graphics&) = delete;
		~Graphics() = delete;
	};
}



#endif // __INCLUDE_GUARD_graphics_h__