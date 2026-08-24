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
		friend class MaterialShader;

	private: // Members:
		uint32_t renderModeIndex;
		emberCommon::VertexMemoryLayout vertexMemoryLayout;


	private: // Methods:
		// Constructor:
		PipelineKey(uint32_t renderModeIndex, emberCommon::VertexMemoryLayout vertexMemoryLayout);

	public: // Mehtods:
		// Constructors:
		template<RenderStage stage>
		requires HasRenderPipelineAndMode<stage>
		static PipelineKey Create(typename RenderStageTraits<stage>::RenderMode renderMode, emberCommon::VertexMemoryLayout vertexMemoryLayout)
		{
			return PipelineKey(RenderStageTraits<stage>::RenderModeIndex(renderMode), vertexMemoryLayout);
		}
		template<RenderStage stage>
		requires HasRenderPipelineAndNotMode<stage>
		static PipelineKey Create(emberCommon::VertexMemoryLayout vertexMemoryLayout)
		{
			return PipelineKey(RenderStageTraits<stage>::RenderModeIndex(), vertexMemoryLayout);
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