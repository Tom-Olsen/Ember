#pragma once
#include "emberMath.h"



namespace vulkanRendererBackend
{
	// Forward declarations:
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