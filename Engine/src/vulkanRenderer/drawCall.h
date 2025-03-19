#ifndef __INCLUDE_GUARD_drawCall_h__
#define __INCLUDE_GUARD_drawCall_h__
#include "emberMath.h"



namespace emberEngine
{
	// Forward declarations:
	class Camera;
	class Material;
	class ShaderProperties;
	class Mesh;

	struct DrawCall
	{
		Float4x4 localToWorldMatrix;
		bool receiveShadows;
		bool castShadows;
		Material* pMaterial;
		ShaderProperties* pShaderProperties;
		ShaderProperties* pShadowShaderProperties;
		Mesh* pMesh;
		uint32_t instanceCount;	// 0 implies no instanced rendering.

		void SetRenderMatrizes(const Float4x4& viewMatrix, const Float4x4& projectionMatrix);
		void SetLightData();

	private:
		void SetDirectionalLightData();
		void SetPositionalLightData();
	};
}



#endif // __INCLUDE_GUARD_drawCall_h__