#ifndef __INCLUDE_GUARD_vulkanRendererBackend_graphics_h__
#define __INCLUDE_GUARD_vulkanRendererBackend_graphics_h__
#include "drawCall.h"		// needed, as we don't save pointers.
#include "iMath.h"
#include <memory>
#include <unordered_map>
#include <vector>



namespace vulkanRendererBackend
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
			iMath::Float3 position;
			iMath::Float4x4 viewMatrix;
			iMath::Float4x4 projectionMatrix;
		};

	private: // Members
		static bool s_isInitialized;
		static Camera s_activeCamera;
		static std::vector<DrawCall> s_staticDrawCalls;
		static std::vector<DrawCall> s_dynamicDrawCalls;
		static std::vector<DrawCall*> s_sortedDrawCallPointers;
        static std::unique_ptr<Mesh> s_pFullScreenRenderQuad;
		static std::unique_ptr<Material> s_pShadowMaterial;
		static std::unique_ptr<Material> s_pDefaultMaterial;
		static std::unique_ptr<Material> s_errorMaterial;
		static float s_depthBiasConstantFactor;
		static float s_depthBiasClamp;
		static float s_depthBiasSlopeFactor;

	public: // Methods:
		static void Init();
		static void Clear();

		// Draw mesh:
		static void DrawMesh(Mesh* pMesh, Material* pMaterial, ShaderProperties* pShaderProperties, const iMath::Float4x4& localToWorldMatrix, bool receiveShadows = true, bool castShadows = true);
		static ShaderProperties* DrawMesh(Mesh* pMesh, Material* pMaterial, const iMath::Float4x4& localToWorldMatrix, bool receiveShadows = true, bool castShadows = true);
		
		// Draw instanced:
		static void DrawInstanced(uint32_t instanceCount, StorageBuffer* pInstanceBuffer, Mesh* pMesh, Material* pMaterial, ShaderProperties* pShaderProperties, const iMath::Float4x4& localToWorldMatrix, bool receiveShadows = true, bool castShadows = true);
		static ShaderProperties* DrawInstanced(uint32_t instanceCount, StorageBuffer* pInstanceBuffer, Mesh* pMesh, Material* pMaterial, const iMath::Float4x4& localToWorldMatrix, bool receiveShadows = true, bool castShadows = true);

		// Setters:
		static void SetActiveCamera(const iMath::Float3& position, const iMath::Float4x4& viewMatrix, const iMath::Float4x4& projectionMatrix);
		static void SetDeptBiasConstantFactor(float depthBiasConstantFactor);
		static void SetDeptBiasClamp(float depthBiasClamp);
		static void SetDeptBiasSlopeFactor(float depthBiasSlopeFactor);

		// Getters:
        static Mesh* GetFullScreenRenderQuad();
        static Material* GetShadowMaterial();
		static const Camera& GetActiveCamera();
		static std::vector<DrawCall*>* GetSortedDrawCallPointers();
		static float GetDeptBiasConstantFactor();
		static float GetDeptBiasClamp();
		static float GetDeptBiasSlopeFactor();

		// Management:
		static void ResetDrawCalls();

	private: // Methods:
	    static Mesh* CreateFullScreenRenderQuad();
		// Delete all constructors:
		Graphics() = delete;
		Graphics(const Graphics&) = delete;
		Graphics& operator=(const Graphics&) = delete;
		~Graphics() = delete;
	};
}



#endif // __INCLUDE_GUARD_vulkanRendererBackend_graphics_h__