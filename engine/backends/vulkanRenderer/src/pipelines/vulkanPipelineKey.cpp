#include "vulkanPipelineKey.h"



namespace vulkanRendererBackend
{
	// Private methods:
	// Constructor:
	PipelineKey::PipelineKey(PipelineType pipelineType, uint32_t pipelineVariantIndex, emberCommon::VertexMemoryLayout vertexMemoryLayout)
		: pipelineType(pipelineType)
		, pipelineVariantIndex(pipelineVariantIndex)
		, vertexMemoryLayout(vertexMemoryLayout)
	{

	}


	
	// Public methods:
	// Comparison:
	bool PipelineKey::operator==(const PipelineKey& other) const
	{
		return pipelineType == other.pipelineType
			&& pipelineVariantIndex == other.pipelineVariantIndex
			&& vertexMemoryLayout == other.vertexMemoryLayout;
	}

	// Hashing:
	size_t PipelineKey::Hasher::operator()(const PipelineKey& pipelineKey) const
	{
		size_t pipelineType = static_cast<size_t>(pipelineKey.pipelineType);
		size_t pipelineVariantIndex = static_cast<size_t>(pipelineKey.pipelineVariantIndex);
		size_t vertexMemoryLayout = static_cast<size_t>(pipelineKey.vertexMemoryLayout);
		return pipelineType ^ (pipelineVariantIndex << 8) ^ (vertexMemoryLayout << 16);
	}
}
