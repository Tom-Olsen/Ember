#include "vulkanPipelineKey.h"



namespace vulkanRendererBackend
{
	// Private methods:
	// Constructor:
	PipelineKey::PipelineKey(PipelineType pipelineType, uint32_t renderModeIndex, emberCommon::VertexMemoryLayout vertexMemoryLayout)
		: pipelineType(pipelineType)
		, renderModeIndex(renderModeIndex)
		, vertexMemoryLayout(vertexMemoryLayout)
	{

	}


	
	// Public methods:
	// Comparison:
	bool PipelineKey::operator==(const PipelineKey& other) const
	{
		return pipelineType == other.pipelineType
			&& renderModeIndex == other.renderModeIndex
			&& vertexMemoryLayout == other.vertexMemoryLayout;
	}

	// Hashing:
	size_t PipelineKey::Hasher::operator()(const PipelineKey& pipelineKey) const
	{
		size_t pipelineType = static_cast<size_t>(pipelineKey.pipelineType);
		size_t renderModeIndex = static_cast<size_t>(pipelineKey.renderModeIndex);
		size_t vertexMemoryLayout = static_cast<size_t>(pipelineKey.vertexMemoryLayout);
		return pipelineType ^ (renderModeIndex << 8) ^ (vertexMemoryLayout << 16);
	}
}
