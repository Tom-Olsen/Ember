#pragma once
#include "iMaterial.h"
#include "commonCullMode.h"
#include "commonForwardRenderState.h"
#include "commonGizmoRenderState.h"
#include "commonMaterialType.h"
#include "commonOutlineRenderState.h"
#include "commonPresentRenderState.h"
#include "commonShadowRenderState.h"
#include "vulkanPipelineKey.h"
#include "vulkanRendererExport.h"
#include "vulkanShader.h"
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Forward declarations:
	class Mesh;
	class Pipeline;



	/// <summary>
	/// Material creation is expensive.
	/// It is recommended to create all Materials at the start of the application.
	/// Create a Material pointer and store it in the static MaterialManager class (core), making it globally accessible.
	/// Each Material must be used with a ShaderProperties which is customized for the Material.
	/// </summary>
	class VULKAN_RENDERER_API Material : public Shader, public emberBackendInterface::IMaterial
	{
	private: // Members:
		emberCommon::MaterialType m_materialType;
		std::unique_ptr<emberCommon::ForwardRenderState> m_pForwardRenderState;
		std::unique_ptr<emberCommon::GizmoRenderState> m_pGizmoRenderState;
		std::unique_ptr<emberCommon::OutlineRenderState> m_pOutlineRenderState;
		std::unique_ptr<emberCommon::ShadowRenderState> m_pShadowRenderState;
		std::unique_ptr<emberCommon::PresentRenderState> m_pPresentRenderState;
		Material* m_pShadowMaterial;    // the shadow material used alongside this material.
		std::unordered_map<PipelineKey, std::unique_ptr<Pipeline>, PipelineKey::Hasher> m_pipelines;

	public: // Methods:
		// Factories/Destructor:
        static Material CreateOutline(const std::string& name, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv);
		static Material CreateForward(const std::string& name, emberCommon::ForwardRenderMode renderMode, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv);
		static Material CreateGizmo(const std::string& name, emberCommon::GizmoRenderMode renderMode, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv);
		static Material CreateShadow(const std::string& name, uint32_t shadowMapResolution, const std::filesystem::path& vertexSpv);
		static Material CreatePresent(const std::string& name, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv);
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
		uint32_t GetPipelineVariantIndex() const;
		emberCommon::ForwardRenderMode GetForwardRenderMode() const override;
		emberCommon::GizmoRenderMode GetGizmoRenderMode() const override;
		const emberCommon::ForwardRenderState& GetForwardRenderState() const override;
		const emberCommon::GizmoRenderState& GetGizmoRenderState() const override;
		template<RenderStage stage>
		requires HasRenderPipelineAndMode<stage>
		const Pipeline* GetPipeline(const Mesh* pMesh, uint32_t pipelineVariantIndex) const
		{
			return GetPipelineByStage(RenderStageTraits<stage>::pipelineType, pMesh, pipelineVariantIndex);
		}
		template<RenderStage stage>
		requires HasRenderPipelineAndMode<stage>
		const Pipeline* GetPipeline(const Mesh* pMesh, typename RenderStageTraits<stage>::RenderMode renderMode) const
		{
			return GetPipelineByStage(RenderStageTraits<stage>::pipelineType, pMesh, RenderStageTraits<stage>::PipelineVariantIndex(renderMode));
		}
		template<RenderStage stage>
		requires HasRenderPipelineAndNotMode<stage>
		const Pipeline* GetPipeline(const Mesh* pMesh) const
		{
			return GetPipelineByStage(RenderStageTraits<stage>::pipelineType, pMesh, RenderStageTraits<stage>::PipelineVariantIndex());
		}

		// Debugging:
		void Print() const override;

	private: // Methods:
		// Constructor:
		Material(const std::string& name);

		// Pipeline lookup:
		bool HasPipeline(PipelineType pipelineType) const;
		const Pipeline* GetPipelineByStage(PipelineType pipelineType, const Mesh* pMesh, uint32_t pipelineVariantIndex) const;
	};
}