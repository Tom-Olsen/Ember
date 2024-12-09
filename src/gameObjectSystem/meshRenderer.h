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
	std::unique_ptr<MaterialProperties> materialProperties;
	bool castShadows = true;
	bool receiveShadows = true;

private: // Members:
	Material* material;
	static Material* shadowMaterial;
	static std::unique_ptr<MaterialProperties> shadowMaterialProperties;

public: // Methods:
	MeshRenderer();
	~MeshRenderer();

	// Setter:
	void SetMaterial(Material* material);
	void SetRenderMatrizes(Camera* camera);
	void SetLightData(const std::array<DirectionalLight*, MAX_D_LIGHTS>& directionalLights);
	void SetLightData(const std::array<SpotLight*, MAX_S_LIGHTS>& spotLights);
	void SetLightData(const std::array<PointLight*, MAX_P_LIGHTS>& pointLights);

	// Shading render pass getters:
	Material* GetMaterial();
	const VkDescriptorSet* const GetShadingDescriptorSets(uint32_t frameIndex) const;
	const VkPipeline& GetShadingPipeline() const;
	const VkPipelineLayout& GetShadingPipelineLayout() const;

	// Shadow render pass getters:
	static const VkDescriptorSet* const GetShadowDescriptorSets(uint32_t frameIndex);
	static const VkPipeline& GetShadowPipeline();
	static const VkPipelineLayout& GetShadowPipelineLayout();

	// Overrides:
	std::string ToString() const override;

private: // Methods:
};



#endif // __INCLUDE_GUARD_meshRenderer_h__