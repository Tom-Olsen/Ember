#pragma once
#include <cstdint>
#include <string_view>



namespace emberCommon
{
	enum class AccessMaskFlag : uint64_t
	{
		none,
		transferRead,
		transferWrite,
		uniformRead,
		shaderRead,
		shaderWrite,
		sampledRead,
		storageRead,
		storageWrite,
		memoryRead,
		memoryWrite,
		accelerationStructureRead,
		descriptorBufferRead,
		vertexAttributeRead,
		indexRead,
		depthStencilAttachmentRead,
		depthStencilAttachmentWrite,
		colorAttachmentRead,
		colorAttachmentWrite
	};
	inline constexpr std::string_view AccessMaskFlagNames[] =
	{
		"none",
		"transferRead",
		"transferWrite",
		"uniformRead",
		"shaderRead",
		"shaderWrite",
		"sampledRead",
		"storageRead",
		"storageWrite",
		"memoryRead",
		"memoryWrite",
		"accelerationStructureRead",
		"descriptorBufferRead",
		"vertexAttributeRead",
		"indexRead",
		"depthStencilAttachmentRead",
		"depthStencilAttachmentWrite",
		"colorAttachmentRead",
		"colorAttachmentWrite"
	};



	struct AccessMask { AccessMaskFlag flag; };
	struct TopOfPipeAccessMask : public AccessMask {};
	struct TransferAccessMask : public AccessMask {};
	struct ComputeShaderAccessMask : public AccessMask {};
	struct VertexAttributeInputAccessMask : public AccessMask {};
	struct VertexInputAccessMask : public AccessMask {};
	struct VertexShaderAccessMask : public AccessMask {};
	struct EarlyFragmentTestAccessMask : public AccessMask {};
	struct FragmentShaderAccessMask : public AccessMask {};
	struct ColorAttachmentOutputAccessMask : public AccessMask {};
	struct CopyAccessMask : public AccessMask {};
	struct NoneAccessMask : public AccessMask {};
	struct BottomOfPipeAccessMask : public AccessMask {};



	struct AccessMasks
	{
		inline static TopOfPipeAccessMask topOfPipe_none{ AccessMaskFlag::none };

		inline static TransferAccessMask transfer_transferRead{ AccessMaskFlag::transferRead };
		inline static TransferAccessMask transfer_transferWrite{ AccessMaskFlag::transferWrite };
		inline static TransferAccessMask transfer_none{ AccessMaskFlag::none };

		inline static ComputeShaderAccessMask computeShader_uniformRead{ AccessMaskFlag::uniformRead };
		inline static ComputeShaderAccessMask computeShader_shaderRead{ AccessMaskFlag::shaderRead };
		inline static ComputeShaderAccessMask computeShader_shaderWrite{ AccessMaskFlag::shaderWrite };
		inline static ComputeShaderAccessMask computeShader_sampledRead{ AccessMaskFlag::sampledRead };
		inline static ComputeShaderAccessMask computeShader_storageRea{ AccessMaskFlag::storageRead };
		inline static ComputeShaderAccessMask computeShader_storageWrit{ AccessMaskFlag::storageWrite };
		inline static ComputeShaderAccessMask computeShader_memoryRea{ AccessMaskFlag::memoryRead };
		inline static ComputeShaderAccessMask computeShader_memoryWrit{ AccessMaskFlag::memoryWrite };
		inline static ComputeShaderAccessMask computeShader_accelerationStructureRea{ AccessMaskFlag::accelerationStructureRead };
		inline static ComputeShaderAccessMask computeShader_descriptorBufferRea{ AccessMaskFlag::descriptorBufferRead };
		inline static ComputeShaderAccessMask computeShader_none{ AccessMaskFlag::none };

		inline static VertexAttributeInputAccessMask vertexAttributeInput_vertexAttributeRead{ AccessMaskFlag::vertexAttributeRead };
		inline static VertexAttributeInputAccessMask vertexAttributeInput_none{ AccessMaskFlag::none };

		inline static VertexInputAccessMask vertexInput_indexRead{ AccessMaskFlag::indexRead };
		inline static VertexInputAccessMask vertexInput_vertexAttributeRead{ AccessMaskFlag::vertexAttributeRead };
		inline static VertexInputAccessMask vertexInput_none{ AccessMaskFlag::none };

		inline static VertexShaderAccessMask vertexShader_uniformRead{ AccessMaskFlag::uniformRead };
		inline static VertexShaderAccessMask vertexShader_shaderRead{ AccessMaskFlag::shaderRead };
		inline static VertexShaderAccessMask vertexShader_shaderWrite{ AccessMaskFlag::shaderWrite };
		inline static VertexShaderAccessMask vertexShader_sampledRead{ AccessMaskFlag::sampledRead };
		inline static VertexShaderAccessMask vertexShader_storageRead{ AccessMaskFlag::storageRead };
		inline static VertexShaderAccessMask vertexShader_storageWrite{ AccessMaskFlag::storageWrite };
		inline static VertexShaderAccessMask vertexShader_accelerationStructureRead{ AccessMaskFlag::accelerationStructureRead };
		inline static VertexShaderAccessMask vertexShader_descriptorBufferRead{ AccessMaskFlag::descriptorBufferRead };
		inline static VertexShaderAccessMask vertexShader_none{ AccessMaskFlag::none };

		inline static EarlyFragmentTestAccessMask earlyFragmentTest_depthStencilAttachmentRead{ AccessMaskFlag::depthStencilAttachmentRead };
		inline static EarlyFragmentTestAccessMask earlyFragmentTest_depthStencilAttachmentWrite{ AccessMaskFlag::depthStencilAttachmentWrite };
		inline static EarlyFragmentTestAccessMask earlyFragmentTest_none{ AccessMaskFlag::none };

		inline static FragmentShaderAccessMask fragmentShader_uniformRead{ AccessMaskFlag::uniformRead };
		inline static FragmentShaderAccessMask fragmentShader_shaderRead{ AccessMaskFlag::shaderRead };
		inline static FragmentShaderAccessMask fragmentShader_shaderWrite{ AccessMaskFlag::shaderWrite };
		inline static FragmentShaderAccessMask fragmentShader_colorAttachmentRead{ AccessMaskFlag::colorAttachmentRead };
		inline static FragmentShaderAccessMask fragmentShader_depthStencilAttachmentRead{ AccessMaskFlag::depthStencilAttachmentRead };
		inline static FragmentShaderAccessMask fragmentShader_sampledRead{ AccessMaskFlag::sampledRead };
		inline static FragmentShaderAccessMask fragmentShader_storageRead{ AccessMaskFlag::storageRead };
		inline static FragmentShaderAccessMask fragmentShader_storageWrite{ AccessMaskFlag::storageWrite };
		inline static FragmentShaderAccessMask fragmentShader_accelerationStructureRead{ AccessMaskFlag::accelerationStructureRead };
		inline static FragmentShaderAccessMask fragmentShader_descriptorBufferRead{ AccessMaskFlag::descriptorBufferRead };
		inline static FragmentShaderAccessMask fragmentShader_none{ AccessMaskFlag::none };

		inline static ColorAttachmentOutputAccessMask colorAttachmentOutput_colorAttachmentRead{ AccessMaskFlag::colorAttachmentRead };
		inline static ColorAttachmentOutputAccessMask colorAttachmentOutput_colorAttachmentWrite{ AccessMaskFlag::colorAttachmentWrite };
		inline static ColorAttachmentOutputAccessMask colorAttachmentOutput_none{ AccessMaskFlag::none };

		inline static CopyAccessMask copy_transferRead{ AccessMaskFlag::transferRead };
		inline static CopyAccessMask copy_transferWrite{ AccessMaskFlag::transferWrite };
		inline static CopyAccessMask copy_none{ AccessMaskFlag::none };

		inline static NoneAccessMask none_none{ AccessMaskFlag::none };

		inline static BottomOfPipeAccessMask bottomOfPipe_none{ AccessMaskFlag::none };
	};
}