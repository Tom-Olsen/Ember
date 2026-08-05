#pragma once
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



	class VULKAN_RENDERER_API MaterialShader : public Shader
	{
	private: // Members:
		emberCommon::MaterialType m_materialType;
		std::unordered_map<PipelineKey, std::unique_ptr<Pipeline>, PipelineKey::Hasher> m_pipelines;

	public: // Methods:
		// Factories/Destructor:
		static std::shared_ptr<MaterialShader> CreateOutline(const std::string& name, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv);
		static std::shared_ptr<MaterialShader> CreateForward(const std::string& name, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv);
		static std::shared_ptr<MaterialShader> CreateGizmo(const std::string& name, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv);
		static std::shared_ptr<MaterialShader> CreateShadow(const std::string& name, uint32_t shadowMapResolution, const std::filesystem::path& vertexSpv);
		static std::shared_ptr<MaterialShader> CreatePresent(const std::string& name, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv);
		~MaterialShader();

		// Non-copyable:
		MaterialShader(const MaterialShader& other) = delete;
		MaterialShader& operator=(const MaterialShader& other) = delete;

		// Movable:
		MaterialShader(MaterialShader&& other) noexcept;
		MaterialShader& operator=(MaterialShader&& other) noexcept;

		// Getters:
		emberCommon::MaterialType GetMaterialType() const;
		bool HasPipeline(PipelineType pipelineType) const;
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

	private: // Methods:
		// Constructor:
		MaterialShader(const std::string& name, emberCommon::MaterialType materialType);

		// Pipeline lookup:
		const Pipeline* GetPipelineByStage(PipelineType pipelineType, const Mesh* pMesh, uint32_t pipelineVariantIndex) const;
	};
}