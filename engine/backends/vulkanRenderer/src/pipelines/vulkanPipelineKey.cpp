#include "vulkanPipelineKey.h"
#include "vulkanMesh.h"
#include <stdexcept>



namespace vulkanRendererBackend
{
	// Public methods:
	// Constructors:
	PipelineKey::PipelineKey()
		: pipelineType(PipelineType::forward)
		, renderMode(emberCommon::RenderMode::opaque)
		, vertexMemoryLayout(emberCommon::VertexMemoryLayout::interleaved)
	{

	}
	PipelineKey::PipelineKey(PipelineType pipelineType, emberCommon::RenderMode renderMode, emberCommon::VertexMemoryLayout vertexMemoryLayout)
		: pipelineType(pipelineType)
		, renderMode(ResolveRenderMode(pipelineType, renderMode))
		, vertexMemoryLayout(vertexMemoryLayout)
	{

	}
	PipelineKey::PipelineKey(PipelineType pipelineType, emberCommon::RenderMode renderMode, const Mesh* pMesh)
		: pipelineType(pipelineType)
		, renderMode(ResolveRenderMode(pipelineType, renderMode))
		, vertexMemoryLayout(pMesh->GetVertexMemoryLayout())
	{

	}

	// Comparison:
	bool PipelineKey::operator==(const PipelineKey& other) const
	{
		return pipelineType == other.pipelineType
			&& renderMode == other.renderMode
			&& vertexMemoryLayout == other.vertexMemoryLayout;
	}

	// Hashing:
	size_t PipelineKey::Hasher::operator()(const PipelineKey& pipelineKey) const
	{
		size_t pipelineType = static_cast<size_t>(pipelineKey.pipelineType);
		size_t renderMode = static_cast<size_t>(pipelineKey.renderMode);
		size_t vertexMemoryLayout = static_cast<size_t>(pipelineKey.vertexMemoryLayout);
		return pipelineType ^ (renderMode << 8) ^ (vertexMemoryLayout << 16);
	}



	// Private methods:
	emberCommon::RenderMode PipelineKey::ResolveRenderMode(PipelineType pipelineType, emberCommon::RenderMode renderMode)
	{
		// gizmo/forward can be in differend render modes, e.g. opaque or transparent.
		// outline/shadow/present always have to use opaque render mode, as others make no sense for them.
		switch (pipelineType)
		{
			case PipelineType::gizmo:
			case PipelineType::forward:
				return renderMode;
			case PipelineType::outline:
			case PipelineType::shadow:
			case PipelineType::present:
				return emberCommon::RenderMode::opaque;
			default:
				throw std::runtime_error("PipelineKey::ResolveRenderMode(...) failed. Unsupported pipeline type.");
		}
	}
}