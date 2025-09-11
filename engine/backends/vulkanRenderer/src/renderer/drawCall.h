#ifndef __INCLUDE_GUARD_vulkanRendererBackend_drawCall_h__
#define __INCLUDE_GUARD_vulkanRendererBackend_drawCall_h__
#include "iMath.h"



namespace vulkanRendererBackend
{
	// Forward declarations:
	class Material;
	class ShaderProperties;
	class Mesh;

	struct DrawCall
	{
		iMath::Float4x4 localToWorldMatrix;
		bool receiveShadows;
		bool castShadows;
		Material* pMaterial;
		ShaderProperties* pShaderProperties;
		ShaderProperties* pShadowShaderProperties;
		Mesh* pMesh;
		uint32_t instanceCount;	// 0 implies no instanced rendering.

		void SetRenderMatrizes(const iMath::Float4x4& viewMatrix, const iMath::Float4x4& projectionMatrix);
		void SetLightData();

	private:
		void SetDirectionalLightData();
		void SetPositionalLightData();
	};
}



#endif // __INCLUDE_GUARD_vulkanRendererBackend_drawCall_h__