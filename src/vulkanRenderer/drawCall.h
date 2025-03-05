#ifndef __INCLUDE_GUARD_drawCall_h__
#define __INCLUDE_GUARD_drawCall_h__
#include "emberMath.h"
#include <array>



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

		void SetRenderMatrizes(Camera* const pCamera);
		void SetLightData();

	private:
		void SetDirectionalLightData();
		void SetPositionalLightData();
	};
}



#endif // __INCLUDE_GUARD_drawCall_h__