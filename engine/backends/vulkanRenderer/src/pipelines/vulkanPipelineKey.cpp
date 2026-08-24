#include "vulkanPipelineKey.h"



namespace vulkanRendererBackend
{
	// Private methods:
	// Constructor:
	PipelineKey::PipelineKey(uint32_t renderModeIndex, emberCommon::VertexMemoryLayout vertexMemoryLayout)
		: renderModeIndex(renderModeIndex)
		, vertexMemoryLayout(vertexMemoryLayout)
	{

	}


	
	// Public methods:
	// Comparison:
	bool PipelineKey::operator==(const PipelineKey& other) const
	{
		return renderModeIndex == other.renderModeIndex
			&& vertexMemoryLayout == other.vertexMemoryLayout;
	}

	// Hashing:
	size_t PipelineKey::Hasher::operator()(const PipelineKey& pipelineKey) const
	{
		size_t renderModeIndex = static_cast<size_t>(pipelineKey.renderModeIndex);
		size_t vertexMemoryLayout = static_cast<size_t>(pipelineKey.vertexMemoryLayout);
		return renderModeIndex ^ (vertexMemoryLayout << 8);
	}
}