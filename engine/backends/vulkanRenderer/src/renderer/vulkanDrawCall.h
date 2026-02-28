#pragma once
#include "commonLighting.h"
#include "emberMath.h"
#include <vector>



namespace vulkanRendererBackend
{
	// Forward declarations:
	class Material;
	class DescriptorSetBinding;
	class Mesh;



	struct DrawCall
	{
		Float4x4 localToWorldMatrix;
		bool receiveShadows;
		bool castShadows;
		Material* pMaterial;
		DescriptorSetBinding* pDescriptorSetBinding;
		DescriptorSetBinding* pShadowDescriptorSetBinding;
		Mesh* pMesh;
		uint32_t instanceCount;	// 0 implies no instanced rendering.

		void SetRenderMatrizes(const Float4x4& viewMatrix, const Float4x4& projectionMatrix);
		void SetLightData(std::vector<emberCommon::DirectionalLight>& directionalLights, std::vector<emberCommon::PositionalLight>& positionalLights);

	private:
		void SetDirectionalLightData(std::vector<emberCommon::DirectionalLight>& directionalLights);
		void SetPositionalLightData(std::vector<emberCommon::PositionalLight>& positionalLights);
	};
}