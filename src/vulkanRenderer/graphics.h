#ifndef __INCLUDE_GUARD_graphics_h__
#define __INCLUDE_GUARD_graphics_h__
#include "drawCall.h"	// needed for s_drawCalls, as we don't save pointers.
#include "emberMath.h"
#include "resourcePool.h"
#include <unordered_map>
#include <vector>



namespace emberEngine
{
	// Forward declarations:
	class Material;
	class ShaderProperties;
	class Mesh;
	class StorageBuffer;



	class Graphics
	{
	private: // Members
		static bool s_isInitialized;
		static Material* s_pShadowMaterial;
		static std::vector<DrawCall> s_staticDrawCalls;
		static std::vector<DrawCall> s_dynamicDrawCalls;
		static std::vector<DrawCall*> s_sortedDrawCallPointers;
		static std::unordered_map<Material*, ResourcePool<ShaderProperties, 20>> s_shaderPropertiesPoolMap;
		static ResourcePool<ShaderProperties, 200> s_shadowShaderPropertiesPool;	// Seperate pool for shadow material, as it is the same for all draw calls and benefits from larger pool blockSize.
		static Mesh* s_pLineSegmentMesh;
		static Mesh* s_pSphereMesh;
		static Mesh* s_pArrowMesh;
		static Material* s_pSimpleLitMaterial;
		static Material* s_pSimpleUnlitMaterial;
		static Material* s_errorMaterial;

	public: // Methods:
		static void Init();
		static void Clear();
		static void SetLineSegmentMesh(Mesh* pMesh);
		
		// Draw mesh:
		static ShaderProperties* DrawMesh(Mesh* pMesh, Material* pMaterial, const Float3& position, const Float3x3& rotationMatrix = Float3x3::identity, float scale = 1.0f, bool receiveShadows = true, bool castShadows = true);
		static ShaderProperties* DrawMesh(Mesh* pMesh, Material* pMaterial, const Float3& position, const Float3x3& rotationMatrix = Float3x3::identity, const Float3& scale = Float3::one, bool receiveShadows = true, bool castShadows = true);
		static void DrawMesh(Mesh* pMesh, Material* pMaterial, ShaderProperties* pShaderProperties, const Float4x4& localToWorldMatrix, bool receiveShadows = true, bool castShadows = true);
		static ShaderProperties* DrawMesh(Mesh* pMesh, Material* pMaterial, const Float4x4& localToWorldMatrix, bool receiveShadows = true, bool castShadows = true);
		
		// Draw instanced:
		static void DrawInstanced(uint32_t instanceCount, StorageBuffer* pInstanceBuffer, Mesh* pMesh, Material* pMaterial, ShaderProperties* pShaderProperties, const Float4x4& localToWorldMatrix, bool receiveShadows = true, bool castShadows = true);
		static ShaderProperties* DrawInstanced(uint32_t instanceCount, StorageBuffer* pInstanceBuffer, Mesh* pMesh, Material* pMaterial, const Float4x4& localToWorldMatrix, bool receiveShadows = true, bool castShadows = true);

		// Draw line segment:
		static void DrawLineSegment(const Float3& start, const Float3& end, float width, const Float4& color = Float4::one, bool receiveShadows = true, bool castShadows = true);
		static ShaderProperties* DrawLineSegment(const Float3& start, const Float3& end, float width, Material* pMaterial, bool receiveShadows = true, bool castShadows = true);

		// Speciaized draw calls:
		static void DrawSphere(const Float3& position, float radius, const Float4& color = Float4::white, bool receiveShadows = true, bool castShadows = true);
		static void DrawArrow(const Float3& position, const Float3& direction, float size = 1.0f, const Float4& color = Float4::white, bool receiveShadows = true, bool castShadows = true);
		static void DrawFrustum(const Float4x4& localToWorldMatrix, const Float4x4& projectionMatrix, float width = 0.1f, const Float4& color = Float4::white, bool receiveShadows = false, bool castShadows = false);
		static void DrawBounds(const Float4x4& localToWorldMatrix, const Bounds& bounds, float width = 0.1f, const Float4& color = Float4::white, bool receiveShadows = false, bool castShadows = false);


		// Management:
		static std::vector<DrawCall*>* GetSortedDrawCallPointers();
		static void ResetDrawCalls();

	private: // Methods:
		// Delete all constructors:
		Graphics() = delete;
		Graphics(const Graphics&) = delete;
		Graphics& operator=(const Graphics&) = delete;
		~Graphics() = delete;
	};
}



#endif // __INCLUDE_GUARD_graphics_h__