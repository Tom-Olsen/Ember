#include "vulkanPipelineKey.h"
#include <stdexcept>



namespace vulkanRendererBackend
{
	// Private methods:
	// Constructor:
	PipelineKey::PipelineKey(uint32_t renderModeIndex)
		: renderModeIndex(renderModeIndex)
		, vertexMemoryLayout(emberCommon::VertexMemoryLayout::none)
	{

	}
	PipelineKey::PipelineKey(uint32_t renderModeIndex, emberCommon::VertexMemoryLayout vertexMemoryLayout)
		: renderModeIndex(renderModeIndex)
		, vertexMemoryLayout(vertexMemoryLayout)
	{
		if (vertexMemoryLayout == emberCommon::VertexMemoryLayout::vertexMemoryLayoutCount
		 || vertexMemoryLayout == emberCommon::VertexMemoryLayout::none
		 || vertexMemoryLayout == emberCommon::VertexMemoryLayout::count)
			throw std::invalid_argument("PipelineKey::PipelineKey(...) failed. Mesh pipeline requires a vertex memory layout.");
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