#pragma once
#include "iMaterialShader.h"
#include "commonMaterialType.h"
#include "commonVertexMemoryLayout.h"
#include "vulkanPipelineKey.h"
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



	class VULKAN_RENDERER_API MaterialShader : public emberBackendInterface::IMaterialShader, public Shader
	{
	private: // Members:
		emberCommon::MaterialType m_materialType;
		std::unordered_map<PipelineKey, std::unique_ptr<Pipeline>, PipelineKey::Hasher> m_pipelines;

	public: // Methods:
		// Factories/Destructor:
		static MaterialShader CreateOutline(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& debugName);
		static MaterialShader CreateForward(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& debugName);
		static MaterialShader CreateGizmo(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& debugName);
		static MaterialShader CreateShadow(uint32_t shadowMapResolution, const std::filesystem::path& vertexSpv, const std::string& debugName);
		static MaterialShader CreatePresent(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& debugName);
		~MaterialShader();

		// Non-copyable:
		MaterialShader(const MaterialShader& other) = delete;
		MaterialShader& operator=(const MaterialShader& other) = delete;

		// Movable:
		MaterialShader(MaterialShader&& other) noexcept;
		MaterialShader& operator=(MaterialShader&& other) noexcept;

		// Getters:
		emberCommon::MaterialType GetMaterialType() const override;
		bool HasPipeline(PipelineType pipelineType) const;
		template<RenderStage stage>
		requires HasRenderPipelineAndMode<stage>
		const Pipeline* GetPipeline(const Mesh* pMesh, typename RenderStageTraits<stage>::RenderMode renderMode) const
		{
			return GetPipelineByStage(RenderStageTraits<stage>::pipelineType, pMesh, RenderStageTraits<stage>::RenderModeIndex(renderMode));
		}
		template<RenderStage stage>
		requires HasRenderPipelineAndNotMode<stage>
		const Pipeline* GetPipeline(const Mesh* pMesh) const
		{
			return GetPipelineByStage(RenderStageTraits<stage>::pipelineType, pMesh, RenderStageTraits<stage>::RenderModeIndex());
		}

	private: // Methods:
		// Constructor:
		MaterialShader(emberCommon::MaterialType materialType, const std::string& debugName);

		// Pipeline lookup:
		const Pipeline* GetPipelineByStage(PipelineType pipelineType, const Mesh* pMesh, uint32_t renderModeIndex) const;
	};
}