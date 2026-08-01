#pragma once
#include "commonPipelineState.h"
#include <cstddef>
#include <cstdint>



namespace vulkanRendererBackend
{
	// Forward declarations:
	class Mesh;


	enum class PipelineType : uint8_t
	{
		gizmo,
		outline,
		shadow,
		forward,
		present
	};


	
	struct PipelineKey
	{
	public: // Members:
		PipelineType pipelineType;
		emberCommon::RenderMode renderMode;
		emberCommon::VertexMemoryLayout vertexMemoryLayout;

	public: // Mehtods:
		// Constructors:
		PipelineKey();
		PipelineKey(PipelineType pipelineType, emberCommon::RenderMode renderMode, emberCommon::VertexMemoryLayout vertexMemoryLayout);
		PipelineKey(PipelineType pipelineType, emberCommon::RenderMode renderMode, const Mesh* pMesh);

		// Comparison:
		bool operator==(const PipelineKey& other) const;
		
		// Hashing:
		struct Hasher
		{
			size_t operator()(const PipelineKey& pipelineKey) const;
		};

	private: // Methods:
		emberCommon::RenderMode ResolveRenderMode(PipelineType pipelineType, emberCommon::RenderMode renderMode);
	};
}