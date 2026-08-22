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
#include "vulkanShaderHandle.h"
#include <cstddef>
#include <cstdint>
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
		std::string m_debugName;
		emberCommon::MaterialType m_materialType;
		ShaderHandle m_materialShaderHandle;
		std::unique_ptr<DescriptorSetBinding> m_pShaderDescriptorSetBinding;
		std::unique_ptr<emberCommon::ForwardRenderState> m_pForwardRenderState;
		std::unique_ptr<emberCommon::GizmoRenderState> m_pGizmoRenderState;
		std::unique_ptr<emberCommon::OutlineRenderState> m_pOutlineRenderState;
		std::unique_ptr<emberCommon::ShadowRenderState> m_pShadowRenderState;
		std::unique_ptr<emberCommon::PresentRenderState> m_pPresentRenderState;

	public: // Methods:
		// Factories/Destructor:
        static Material CreateOutline(MaterialShader* pMaterialShader, const std::string& debugName);
		static Material CreateForward(MaterialShader* pMaterialShader, emberCommon::ForwardRenderMode renderMode, const std::string& debugName);
		static Material CreateGizmo(MaterialShader* pMaterialShader, emberCommon::GizmoRenderMode renderMode, const std::string& debugName);
		static Material CreateShadow(MaterialShader* pMaterialShader, const std::string& debugName);
		static Material CreatePresent(MaterialShader* pMaterialShader, const std::string& debugName);
		static Material CloneForward(const Material& sourceMaterial, const std::string& debugName);
		static Material CloneGizmo(const Material& sourceMaterial, const std::string& debugName);
		static Material CloneShadow(const Material& sourceMaterial, const std::string& debugName);
		~Material();

		// Non-copyable:
		Material(const Material& other) = delete;
		Material& operator=(const Material& other) = delete;

		// Movable:
		Material(Material&& other) noexcept;
		Material& operator=(Material&& other) noexcept;

		// Setters:
		void SetRenderQueue(int32_t renderQueue) override;
		void SetCullMode(emberCommon::CullMode cullMode) override;
		void SetForwardRenderMode(emberCommon::ForwardRenderMode renderMode) override;
		void SetGizmoRenderMode(emberCommon::GizmoRenderMode renderMode) override;

		// Getters:
		emberCommon::MaterialType GetMaterialType() const override;
		emberBackendInterface::IDescriptorSetBinding* GetShaderDescriptorSetBinding() const override;
		DescriptorSetBinding* GetDescriptorSetBinding() const;
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
		const std::string& GetDebugName() const;
		template<RenderStage stage>
		requires HasRenderPipelineAndMode<stage>
		const Pipeline* GetPipeline(const Mesh* pMesh) const
		{
			if constexpr (stage == RenderStage::forward)
				return GetMaterialShader()->GetPipeline<stage>(pMesh, GetForwardRenderMode());
			else if constexpr (stage == RenderStage::gizmo)
				return GetMaterialShader()->GetPipeline<stage>(pMesh, GetGizmoRenderMode());
			else
				static_assert(stage == RenderStage::forward || stage == RenderStage::gizmo, "Material::GetPipeline(...) must be updated for this render stage.");
		}
		template<RenderStage stage>
		requires HasRenderPipelineAndNotMode<stage>
		const Pipeline* GetPipeline(const Mesh* pMesh) const
		{
			return GetMaterialShader()->GetPipeline<stage>(pMesh);
		}

		// Debugging:
		void Print() const override;

	private: // Methods:
		// Constructor:
		Material(emberCommon::MaterialType materialType, MaterialShader* pMaterialShader, const std::string& debugName);
		Material(emberCommon::MaterialType materialType, MaterialShader* pMaterialShader, std::unique_ptr<DescriptorSetBinding> pShaderDescriptorSetBinding, const std::string& debugName);
	};
}