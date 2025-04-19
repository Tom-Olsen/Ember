#ifndef __INCLUDE_GUARD_graphics_h__
#define __INCLUDE_GUARD_graphics_h__
#include "drawCall.h"		// needed, as we don't save pointers.
#include "emberMath.h"
#include "resourcePool.h"
#include <unordered_map>
#include <vector>



namespace emberEngine
{
	// Forward declarations:
	class ComputeShader;
	class Material;
	class ShaderProperties;
	class ShaderPropertiesPool;
	class Mesh;
	class Shader;
	class StorageBuffer;



	class Graphics
	{
	public: // Structs:
		struct Camera
		{
			Float3 position;
			Float4x4 viewMatrix;
			Float4x4 projectionMatrix;
		};

	private: // Members
		static bool s_isInitialized;
		static Camera s_activeCamera;
		static Material* s_pShadowMaterial;
		static std::vector<DrawCall> s_staticDrawCalls;
		static std::vector<DrawCall> s_dynamicDrawCalls;
		static std::vector<DrawCall*> s_sortedDrawCallPointers;
		static std::unordered_map<Shader*, ShaderPropertiesPool> s_shaderPropertiesPoolMap;
		static ShaderPropertiesPool s_shadowShaderPropertiesPool;	// Ember::TODO: either remove and use above pool for everything, or make this pool with block allocation.
		static Mesh* s_pLineSegmentMesh;
		static Mesh* s_pSphereMesh;
		static Mesh* s_pArrowMesh;
		static Material* s_pSimpleLitMaterial;
		static Material* s_pSimpleUnlitMaterial;
		static Material* s_errorMaterial;

	public: // Methods:
		static void Init();
		static void Clear();

		// Setters:
		static void SetActiveCamera(const Float3& position, const Float4x4& viewMatrix, const Float4x4& projectionMatrix);
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
		static void DrawBounds(const Float4x4& localToWorldMatrix, const Bounds2d& bounds, float width = 0.1f, const Float4& color = Float4::white, bool receiveShadows = false, bool castShadows = false);
		static void DrawBounds(const Float4x4& localToWorldMatrix, const Bounds& bounds, float width = 0.1f, const Float4& color = Float4::white, bool receiveShadows = false, bool castShadows = false);

		// Getters:
		static const Camera& GetActiveCamera();
		static std::vector<DrawCall*>* GetSortedDrawCallPointers();

		// Management:
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