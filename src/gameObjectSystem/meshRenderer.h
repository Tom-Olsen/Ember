#ifndef __INCLUDE_GUARD_meshRenderer_h__
#define __INCLUDE_GUARD_meshRenderer_h__
#include "component.h"
#include "macros.h"
#include <array>
#include <memory>
#include <string>
#include <vulkan/vulkan.h>



namespace emberEngine
{
	// Forward declarations:
	class Camera;
	class DirectionalLight;
	class Mesh;
	class Material;
	class ShaderProperties;
	class PointLight;
	class SpotLight;



	class MeshRenderer : public Component
	{
	private: // Members:
		bool m_castShadows;
		bool m_receiveShadows;
		bool m_hasErrorMaterial;
		Mesh* m_pMesh;
		Material* m_pMaterial;
		std::unique_ptr<ShaderProperties> m_pShaderProperties;
		static Material* s_pShadowMaterial;
		static std::unique_ptr<ShaderProperties> m_pShadowShaderProperties;

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

		// Forward render pass getters:
		bool GetCastShadows() const;
		bool GetReceiveShadows() const;
		Mesh* GetMesh();
		Material* GetMaterial();
		ShaderProperties* GetShaderProperties();
		const VkDescriptorSet* const GetForwardDescriptorSets(uint32_t frameIndex) const;
		const VkPipeline& GetForwardPipeline() const;
		const VkPipelineLayout& GetForwardPipelineLayout() const;

		// Shadow render pass getters:
		static const VkDescriptorSet* const GetShadowDescriptorSets(uint32_t frameIndex);
		static const VkPipeline& GetShadowPipeline();
		static const VkPipelineLayout& GetShadowPipelineLayout();

		// Overrides:
		const std::string ToString() const override;

	private: // Methods:
	};
}



#endif // __INCLUDE_GUARD_meshRenderer_h__