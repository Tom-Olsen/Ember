#pragma once
#ifndef __INCLUDE_GUARD_meshRenderer_h__
#define __INCLUDE_GUARD_meshRenderer_h__
#include "component.h"
#include "mesh.h"
#include "materialProperties.h"
#include "directionalLight.h"
#include "camera.h"
#include "macros.h"



class MeshRenderer : public Component
{
public: // Members:
	Mesh* mesh;
	std::unique_ptr<MaterialProperties> forwardMaterialProperties;
	std::unique_ptr<MaterialProperties> shadowMaterialProperties;
	bool castShadows = true;
	bool receiveShadows = true;

private: // Members:
	Material* forwardMaterial;
	static Material* shadowMaterial;
	static ShadowRenderPass* shadowRenderPass;

public: // Methods:
	MeshRenderer();
	~MeshRenderer();

	// Setter:
	void SetForwardMaterial(Material* forwardMaterial);
	void SetForwardRenderMatrizes(Camera* camera);
	void SetShadowRenderMatrizes(std::array<DirectionalLight*, MAX_D_LIGHTS> directionalLights);
	void SetForwardLightData(const std::array<DirectionalLight*, MAX_D_LIGHTS>& directionalLights);

	// Forward render pass getters:
	Material* GetForwardMaterial();
	VkDescriptorSet* GetForwardDescriptorSets(uint32_t frameIndex);
	VkPipeline& GetForwardPipeline();
	VkPipelineLayout& GetForwardPipelineLayout();

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