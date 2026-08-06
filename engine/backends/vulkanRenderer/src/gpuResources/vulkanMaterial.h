#pragma once
#include "iMaterial.h"
#include "commonCullMode.h"
#include "commonForwardRenderState.h"
#include "commonGizmoRenderState.h"
#include "commonMaterialType.h"
#include "commonOutlineRenderState.h"
#include "commonPresentRenderState.h"
#include "commonShadowRenderState.h"
#include "vulkanMaterialShader.h"
#include "vulkanRendererExport.h"
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Forward declarations:
	class DescriptorSetBinding;
	class Mesh;
	class Pipeline;



	/// <summary>
	/// Material creation is expensive.
	/// It is recommended to create all Materials at the start of the application.
	/// Create a Material pointer and store it in the static MaterialManager class (core), making it globally accessible.
	/// Each Material must be used with a ShaderProperties which is customized for the Material.
	/// </summary>
	class VULKAN_RENDERER_API Material : public emberBackendInterface::IMaterial
	{
	private: // Members:
		std::string m_name;
		emberCommon::MaterialType m_materialType;
		std::shared_ptr<MaterialShader> m_pMaterialShader;
		std::unique_ptr<DescriptorSetBinding> m_pShaderDescriptorSetBinding;
		std::unique_ptr<emberCommon::ForwardRenderState> m_pForwardRenderState;
		std::unique_ptr<emberCommon::GizmoRenderState> m_pGizmoRenderState;
		std::unique_ptr<emberCommon::OutlineRenderState> m_pOutlineRenderState;
		std::unique_ptr<emberCommon::ShadowRenderState> m_pShadowRenderState;
		std::unique_ptr<emberCommon::PresentRenderState> m_pPresentRenderState;
		Material* m_pShadowMaterial;    // the shadow material used alongside this material.

	public: // Methods:
		// Factories/Destructor:
        static Material CreateOutline(const std::string& name, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv);
		static Material CreateForward(const std::string& name, emberCommon::ForwardRenderMode renderMode, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv);
		static Material CreateGizmo(const std::string& name, emberCommon::GizmoRenderMode renderMode, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv);
		static Material CreateShadow(const std::string& name, uint32_t shadowMapResolution, const std::filesystem::path& vertexSpv);
		static Material CreatePresent(const std::string& name, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv);
		static Material CloneForward(const std::string& name, const Material& sourceMaterial);
		static Material CloneForward(const std::string& name, const Material& sourceMaterial, emberCommon::ForwardRenderMode renderMode);
		static Material CloneGizmo(const std::string& name, const Material& sourceMaterial);
		static Material CloneGizmo(const std::string& name, const Material& sourceMaterial, emberCommon::GizmoRenderMode renderMode);
		static Material CloneShadow(const std::string& name, const Material& sourceMaterial);
		~Material();

		// Non-copyable:
		Material(const Material& other) = delete;
		Material& operator=(const Material& other) = delete;

		// Movable:
		Material(Material&& other) noexcept;
		Material& operator=(Material&& other) noexcept;

		// Setters:
		void SetShadowMaterial(emberBackendInterface::IMaterial* pShadowMaterial) override;
		void SetRenderQueue(int32_t renderQueue) override;
		void SetCullMode(emberCommon::CullMode cullMode) override;
		void SetForwardRenderMode(emberCommon::ForwardRenderMode renderMode) override;
		void SetGizmoRenderMode(emberCommon::GizmoRenderMode renderMode) override;

		// Getters:
		const std::string& GetName() const override;
		emberCommon::MaterialType GetMaterialType() const override;
		Material* GetShadowMaterial() const override;
		emberBackendInterface::IDescriptorSetBinding* GetShaderDescriptorSetBinding() const override;
		int32_t GetRenderQueue() const override;
		emberCommon::CullMode GetCullMode() const override;
		bool IsTransparent() const override;
		emberCommon::ForwardRenderMode GetForwardRenderMode() const override;
		emberCommon::GizmoRenderMode GetGizmoRenderMode() const override;
		const emberCommon::ForwardRenderState& GetForwardRenderState() const override;
		const emberCommon::GizmoRenderState& GetGizmoRenderState() const override;
		Shader* GetShader() const;
		MaterialShader* GetMaterialShader() const;
		const VkPipelineLayout& GetVkPipelineLayout() const;
		DescriptorSetBinding* GetDescriptorSetBinding() const;
		template<RenderStage stage>
		requires HasRenderPipelineAndMode<stage>
		const Pipeline* GetPipeline(const Mesh* pMesh) const
		{
			if constexpr (stage == RenderStage::forward)
				return m_pMaterialShader->GetPipeline<stage>(pMesh, GetForwardRenderMode());
			else if constexpr (stage == RenderStage::gizmo)
				return m_pMaterialShader->GetPipeline<stage>(pMesh, GetGizmoRenderMode());
			else
				static_assert(stage == RenderStage::forward || stage == RenderStage::gizmo, "Material::GetPipeline(...) must be updated for this render stage.");
		}
		template<RenderStage stage>
		requires HasRenderPipelineAndNotMode<stage>
		const Pipeline* GetPipeline(const Mesh* pMesh) const
		{
			return m_pMaterialShader->GetPipeline<stage>(pMesh);
		}

		// Debugging:
		void Print() const override;

	private: // Methods:
		// Constructor:
		Material(const std::string& name, emberCommon::MaterialType materialType, std::shared_ptr<MaterialShader> pMaterialShader);
	};
}