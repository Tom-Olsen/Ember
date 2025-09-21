#pragma once
#include <cstdint>
#include <string>



namespace vulkanRendererBackend
{
	using AccessMask = uint32_t;
	struct AccessMasks
	{
		// VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT:
		struct TopOfPipe
		{
			static AccessMask none;
		};

		// VK_PIPELINE_STAGE_2_TRANSFER_BIT:
		struct Transfer
		{
			static AccessMask transferRead;
			static AccessMask transferWrite;
			static AccessMask none;
		};

		// VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT:
		struct ComputeShader
		{
			static AccessMask uniformRead;
			static AccessMask shaderRead;
			static AccessMask shaderWrite;
			static AccessMask sampledRead;
			static AccessMask storageRead;
			static AccessMask storageWrite;
			static AccessMask memoryRead;
			static AccessMask memoryWrite;
			static AccessMask accelerationStructureRead;
			static AccessMask descriptorBufferRead;
			static AccessMask none;
		};

		// VK_PIPELINE_STAGE_2_VERTEX_ATTRIBUTE_INPUT_BIT:
		struct VertexAttributeInput
		{
			static AccessMask vertexAttributeRead;
			static AccessMask none;
		};

		// VK_PIPELINE_STAGE_2_VERTEX_INPUT_BIT:
		struct VertexInput
		{
			static AccessMask indexRead;
			static AccessMask vertexAttributeRead;
			static AccessMask none;
		};

		// VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT:
		struct VertexShader
		{
			static AccessMask uniformRead;
			static AccessMask shaderRead;
			static AccessMask shaderWrite;
			static AccessMask sampledRead;
			static AccessMask storageRead;
			static AccessMask storageWrite;
			static AccessMask accelerationStructureRead;
			static AccessMask descriptorBufferRead;
			static AccessMask none;
		};

		// VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT:
		struct EarlyFragmentTest
		{
			static AccessMask depthStencilAttachmentRead;
			static AccessMask depthStencilAttachmentWrite;
			static AccessMask none;
		};

		// VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT:
		struct FragmentShader
		{
			static AccessMask uniformRead;
			static AccessMask shaderRead;
			static AccessMask shaderWrite;
			static AccessMask colorAttachmentRead;
			static AccessMask depthStencilAttachmentRead;
			static AccessMask sampledRead;
			static AccessMask storageRead;
			static AccessMask storageWrite;
			static AccessMask accelerationStructureRead;
			static AccessMask descriptorBufferRead;
			static AccessMask none;
		};

		// VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT:
		struct ColorAttachmentOutput
		{
			static AccessMask colorAttachmentRead;
			static AccessMask colorAttachmentWrite;
			static AccessMask none;
		};

		// VK_PIPELINE_STAGE_2_COPY_BIT:
		struct Copy
		{
			static AccessMask transferRead;
			static AccessMask transferWrite;
			static AccessMask none;
		};

		// VK_PIPELINE_STAGE_2_NONE:
		struct None
		{
			static AccessMask none;
		};

		// VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT:
		struct BottomOfPipe
		{
			static AccessMask none;
		};

		static std::string ToString(AccessMask accessMask);
	};
}