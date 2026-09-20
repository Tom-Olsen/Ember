#pragma once
#include "commonMaterialPass.h"
#include "commonVertexMemoryLayout.h"
#include "vulkanPipelineKey.h"
#include "vulkanShader.h"
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Forward declarations:
	class MaterialShaderManager;
	class Mesh;
	class Pipeline;



	class VULKAN_RENDERER_API MaterialShader : public Shader
	{
		// Friends:
		friend class MaterialShaderManager;

	private: // Members:
		emberCommon::MaterialPass m_materialPass;
		std::unordered_map<PipelineKey, std::unique_ptr<Pipeline>, PipelineKey::Hasher> m_pipelines;

	public: // Methods:
		// Destructor:
		~MaterialShader();

		// Non-copyable:
		MaterialShader(const MaterialShader& other) = delete;
		MaterialShader& operator=(const MaterialShader& other) = delete;

		// Movable:
		MaterialShader(MaterialShader&& other) noexcept;
		MaterialShader& operator=(MaterialShader&& other) noexcept;

		// Getters:
		emberCommon::MaterialPass GetMaterialPass() const;
		template<RenderStage stage>
		requires HasRenderPipelineAndMode<stage>
		const Pipeline* GetPipeline(const Mesh* pMesh, typename RenderStageTraits<stage>::RenderMode renderMode) const
		{
			if (m_materialPass != RenderStageTraits<stage>::materialPass)
				throw std::runtime_error("MaterialShader::GetPipeline(...) failed. Requested material pass is not supported by this material shader.");
			return GetPipeline(pMesh, RenderStageTraits<stage>::RenderModeIndex(renderMode));
		}
		template<RenderStage stage>
		requires HasMeshRenderPipelineAndNotMode<stage>
		const Pipeline* GetPipeline(const Mesh* pMesh) const
		{
			if (m_materialPass != RenderStageTraits<stage>::materialPass)
				throw std::runtime_error("MaterialShader::GetPipeline(...) failed. Requested material pass is not supported by this material shader.");
			return GetPipeline(pMesh, RenderStageTraits<stage>::RenderModeIndex());
		}
		template<RenderStage stage>
		requires HasFullscreenPipeline<stage>
		const Pipeline* GetFullscreenPipeline() const
		{
			if (m_materialPass != RenderStageTraits<stage>::materialPass)
				throw std::runtime_error("MaterialShader::GetFullscreenPipeline() failed. Requested material pass is not supported by this material shader.");
			return GetFullscreenPipeline(RenderStageTraits<stage>::RenderModeIndex());
		}

	private: // Methods:
		// Constructor:
		MaterialShader(emberCommon::MaterialPass materialPass, const std::string& debugName);

		// Factories:
		static MaterialShader CreateGizmoMaterialShader(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& debugName);
		static MaterialShader CreateOutlineMaterialShader(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& debugName);
		static MaterialShader CreateShadowMaterialShader(uint32_t shadowMapResolution, const std::filesystem::path& vertexSpv, const std::string& debugName);
		static MaterialShader CreateDeferredGeometryMaterialShader(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& debugName);
		static MaterialShader CreateDeferredLightingMaterialShader(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& debugName);
		static MaterialShader CreateForwardMaterialShader(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& debugName);
		static MaterialShader CreatePresentMaterialShader(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& debugName);

		// Pipeline lookup:
		const Pipeline* GetPipeline(const Mesh* pMesh, uint32_t renderModeIndex) const;
		const Pipeline* GetFullscreenPipeline(uint32_t renderModeIndex) const;
	};
}