#ifndef __INCLUDE_GUARD_meshRenderer_h__
#define __INCLUDE_GUARD_meshRenderer_h__
#include "component.h"
#include "macros.h"
#include <array>
#include <memory>
#include <string>
#include <vulkan/vulkan.h>



class Camera;
class DirectionalLight;
class Mesh;
class Material;
class MaterialProperties;
class PointLight;
class SpotLight;



class MeshRenderer : public Component
{
public: // Members:

private: // Members:
	bool m_castShadows;
	bool m_receiveShadows;
	bool m_hasErrorMaterial;
	Mesh* m_pMesh;
	Material* m_pMaterial;
	std::unique_ptr<MaterialProperties> m_pMaterialProperties;
	static Material* m_pShadowMaterial;
	static std::unique_ptr<MaterialProperties> m_pShadowMaterialProperties;

public: // Methods:
	MeshRenderer();
	~MeshRenderer();

	// Setter:
	void SetCastShadows(bool castShadows);
	void SetReceiveShadows(bool receiveShadows);
	void SetMesh(Mesh* pMesh);
	void SetMaterial(Material* pMaterial);
	void SetRenderMatrizes(Camera* const pCamera);
	void SetLightData(const std::array<DirectionalLight*, MAX_D_LIGHTS>& directionalLights);
	void SetLightData(const std::array<SpotLight*, MAX_S_LIGHTS>& spotLights);
	void SetLightData(const std::array<PointLight*, MAX_P_LIGHTS>& pointLights);

	// Shading render pass getters:
	bool GetCastShadows() const;
	bool GetReceiveShadows() const;
	Mesh* GetMesh();
	Material* GetMaterial();
	MaterialProperties* GetMaterialProperties();
	const VkDescriptorSet* const GetShadingDescriptorSets(uint32_t frameIndex) const;
	const VkPipeline& GetShadingPipeline() const;
	const VkPipelineLayout& GetShadingPipelineLayout() const;

	// Shadow render pass getters:
	static const VkDescriptorSet* const GetShadowDescriptorSets(uint32_t frameIndex);
	static const VkPipeline& GetShadowPipeline();
	static const VkPipelineLayout& GetShadowPipelineLayout();

	// Overrides:
	const std::string ToString() const override;

private: // Methods:
};



#endif // __INCLUDE_GUARD_meshRenderer_h__