#pragma once
#include "iMaterialShader.h"
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
	class Mesh;
	class Pipeline;



	class VULKAN_RENDERER_API MaterialShader : public emberBackendInterface::IMaterialShader, public Shader
	{
	private: // Members:
		emberCommon::MaterialPass m_materialPass;
		std::unordered_map<PipelineKey, std::unique_ptr<Pipeline>, PipelineKey::Hasher> m_pipelines;

	public: // Methods:
		// Factories/Destructor:
		static MaterialShader CreateGizmo(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& debugName);
		static MaterialShader CreateOutline(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& debugName);
		static MaterialShader CreateShadow(uint32_t shadowMapResolution, const std::filesystem::path& vertexSpv, const std::string& debugName);
		static MaterialShader CreateDeferredGeometry(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& debugName);
		static MaterialShader CreateDeferredLighting(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& debugName);
		static MaterialShader CreateForward(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& debugName);
		static MaterialShader CreatePresent(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& debugName);
		~MaterialShader();

		// Non-copyable:
		MaterialShader(const MaterialShader& other) = delete;
		MaterialShader& operator=(const MaterialShader& other) = delete;

		// Movable:
		MaterialShader(MaterialShader&& other) noexcept;
		MaterialShader& operator=(MaterialShader&& other) noexcept;

		// Getters:
		emberCommon::MaterialPass GetMaterialPass() const override;
		template<RenderStage stage>
		requires HasRenderPipelineAndMode<stage>
		const Pipeline* GetPipeline(const Mesh* pMesh, typename RenderStageTraits<stage>::RenderMode renderMode) const
		{
			if (m_materialPass != RenderStageTraits<stage>::materialPass)
				throw std::runtime_error("MaterialShader::GetPipeline(...) failed. Requested material pass is not supported by this material shader.");
			return GetPipeline(pMesh, RenderStageTraits<stage>::RenderModeIndex(renderMode));
		}
		template<RenderStage stage>
		requires HasRenderPipelineAndNotMode<stage>
		const Pipeline* GetPipeline(const Mesh* pMesh) const
		{
			if (m_materialPass != RenderStageTraits<stage>::materialPass)
				throw std::runtime_error("MaterialShader::GetPipeline(...) failed. Requested material pass is not supported by this material shader.");
			return GetPipeline(pMesh, RenderStageTraits<stage>::RenderModeIndex());
		}

	private: // Methods:
		// Constructor:
		MaterialShader(emberCommon::MaterialPass materialPass, const std::string& debugName);

		// Pipeline lookup:
		const Pipeline* GetPipeline(const Mesh* pMesh, uint32_t renderModeIndex) const;
	};
}