#pragma once
#ifndef __INCLUDE_GUARD_meshRenderer_h__
#define __INCLUDE_GUARD_meshRenderer_h__
#include "component.h"
#include "mesh.h"
#include "materialProperties.h"
#include "directionalLight.h"
#include "spotLight.h"
#include "pointLight.h"
#include "camera.h"
#include "macros.h"



class MeshRenderer : public Component
{
public: // Members:
	Mesh* mesh;
	std::unique_ptr<MaterialProperties> shadingMaterialProperties;
	std::unique_ptr<MaterialProperties> shadowMaterialProperties;
	bool castShadows = true;
	bool receiveShadows = true;

private: // Members:
	Material* shadingMaterial;
	static Material* shadowMaterial;
	static ShadowRenderPass* shadowRenderPass;

public: // Methods:
	MeshRenderer();
	~MeshRenderer();

	// Setter:
	void SetShadingMaterial(Material* shadingMaterial);
	void SetShadingRenderMatrizes(Camera* camera);
	void SetShadowRenderMatrizes(std::array<DirectionalLight*, MAX_D_LIGHTS> directionalLights);
	void SetShadowRenderMatrizes(std::array<SpotLight*, MAX_S_LIGHTS> spotLights);
	void SetShadowRenderMatrizes(std::array<PointLight*, MAX_P_LIGHTS> pointLights);
	void SetShadingLightData(const std::array<DirectionalLight*, MAX_D_LIGHTS>& directionalLights);
	void SetShadingLightData(const std::array<SpotLight*, MAX_S_LIGHTS>& spotLights);
	void SetShadingLightData(const std::array<PointLight*, MAX_P_LIGHTS>& pointLights);

	// Shading render pass getters:
	Material* GetShadingMaterial();
	VkDescriptorSet* GetShadingDescriptorSets(uint32_t frameIndex);
	VkPipeline& GetShadingPipeline();
	VkPipelineLayout& GetShadingPipelineLayout();

	// Shadow render pass getters:
	static Material* GetShadowMaterial();
	VkDescriptorSet* GetShadowDescriptorSets(uint32_t frameIndex);
	static VkPipeline& GetShadowPipeline();
	static VkPipelineLayout& GetShadowPipelineLayout();
	// Overrides:
	std::string ToString() const override;

private: // Methods:
};



#endif // __INCLUDE_GUARD_meshRenderer_h__