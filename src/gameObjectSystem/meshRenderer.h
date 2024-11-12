#pragma once
#ifndef __INCLUDE_GUARD_meshRenderer_h__
#define __INCLUDE_GUARD_meshRenderer_h__
#include "component.h"
#include "mesh.h"
#include "materialProperties.h"
#include "directionalLight.h"



class MeshRenderer : public Component
{
public: // Members:
	Mesh* mesh;
	std::unique_ptr<MaterialProperties> forwardMaterialProperties;
	std::unique_ptr<MaterialProperties> shadowMaterialProperties;

private: // Members:
	Material* forwardMaterial;
	Material* shadowMaterial;

public: // Methods:
	MeshRenderer();
	~MeshRenderer();

	// Setter:
	void SetForwardMaterial(Material* forwardMaterial);
	void SetShadowMaterial(Material* shadowMaterial);
	void SetForwardRenderMatrizes(const Float4x4& cameraViewMatrix, const Float4x4& cameraProjMatrix);
	void SetShadowRenderMatrizes(const Float4x4& lightViewMatrix, const Float4x4& lightProjMatrix);
	void SetForwardLightData(const std::array<DirectionalLight*, 2>& directionalLights);

	// Getters:
	Material* GetForwardMaterial();
	Material* GetShadowMaterial();
	VkDescriptorSet* GetForwardDescriptorSets(uint32_t frameIndex);
	VkDescriptorSet* GetShadowDescriptorSets(uint32_t frameIndex);
	VkPipeline& GetForwardPipeline();
	VkPipeline& GetShadowPipeline();
	VkPipelineLayout& GetForwardPipelineLayout();
	VkPipelineLayout& GetShadowPipelineLayout();

	// Overrides:
	std::string ToString() const override;

private: // Methods:
};



#endif // __INCLUDE_GUARD_meshRenderer_h__