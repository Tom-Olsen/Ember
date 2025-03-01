#ifndef __INCLUDE_GUARD_drawCall_h__
#define __INCLUDE_GUARD_drawCall_h__
#include "macros.h"
#include "mathf.h"
#include <array>



namespace emberEngine
{
	// Forward declarations:
	class Camera;
	class DirectionalLight;
	class Material;
	class PointLight;
	class ShaderProperties;
	class SpotLight;
	class StorageBuffer;
	class Mesh;

	struct DrawCall
	{
		Float4x4 localToWorldMatrix;
		bool receiveShadows;
		bool castShadows;
		Material* pMaterial;
		ShaderProperties* pShaderProperties;
		Mesh* pMesh;
		uint32_t instanceCount;	// 0 implies no instanced rendering.
		StorageBuffer* pInstanceBuffer;	// TODO: remove this, or use it for shadow descriptorSet?

		void SetRenderMatrizes(Camera* const pCamera);
		void SetLightData(const std::array<DirectionalLight*, MAX_D_LIGHTS>& directionalLights);
		void SetLightData(const std::array<SpotLight*, MAX_S_LIGHTS>& spotLights);
		void SetLightData(const std::array<PointLight*, MAX_P_LIGHTS>& pointLights);
	};
}



#endif // __INCLUDE_GUARD_drawCall_h__