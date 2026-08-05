#pragma once
#include "commonVertexMemoryLayout.h"
#include "vulkanRenderStage.h"
#include <cstddef>
#include <cstdint>



namespace vulkanRendererBackend
{
	// Forward declarations:
	class Material;
	class MaterialShader;


	struct PipelineKey
	{
		// Friends:
		friend class Material;
		friend class MaterialShader;

	public: // Members:
		PipelineType pipelineType;
		uint32_t pipelineVariantIndex;
		emberCommon::VertexMemoryLayout vertexMemoryLayout;


	private: // Methods:
		// Constructor:
		PipelineKey(PipelineType pipelineType, uint32_t pipelineVariantIndex, emberCommon::VertexMemoryLayout vertexMemoryLayout);

	public: // Mehtods:
		// Constructors:
		template<RenderStage stage>
		requires HasRenderPipelineAndMode<stage>
		static PipelineKey Create(typename RenderStageTraits<stage>::RenderMode renderMode, emberCommon::VertexMemoryLayout vertexMemoryLayout)
		{
			return PipelineKey(RenderStageTraits<stage>::pipelineType, RenderStageTraits<stage>::PipelineVariantIndex(renderMode), vertexMemoryLayout);
		}
		template<RenderStage stage>
		requires HasRenderPipelineAndNotMode<stage>
		static PipelineKey Create(emberCommon::VertexMemoryLayout vertexMemoryLayout)
		{
			return PipelineKey(RenderStageTraits<stage>::pipelineType, RenderStageTraits<stage>::PipelineVariantIndex(), vertexMemoryLayout);
		}

		// Comparison:
		bool operator==(const PipelineKey& other) const;
		
		// Hashing:
		struct Hasher
		{
			size_t operator()(const PipelineKey& pipelineKey) const;
		};
	};
}