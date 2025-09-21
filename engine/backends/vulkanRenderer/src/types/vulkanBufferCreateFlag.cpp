#include "vulkanBufferCreateFlag.h"
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
    BufferCreateFlag BufferCreateFlags::sparse_binding_bit = VK_BUFFER_CREATE_SPARSE_BINDING_BIT;
    BufferCreateFlag BufferCreateFlags::sparse_residency_bit = VK_BUFFER_CREATE_SPARSE_RESIDENCY_BIT;
    BufferCreateFlag BufferCreateFlags::sparse_aliased_bit = VK_BUFFER_CREATE_SPARSE_ALIASED_BIT;
    BufferCreateFlag BufferCreateFlags::protected_bit = VK_BUFFER_CREATE_PROTECTED_BIT;
    BufferCreateFlag BufferCreateFlags::device_address_capture_replay_bit = VK_BUFFER_CREATE_DEVICE_ADDRESS_CAPTURE_REPLAY_BIT;
    BufferCreateFlag BufferCreateFlags::descriptor_buffer_capture_replay_bit_ext = VK_BUFFER_CREATE_DESCRIPTOR_BUFFER_CAPTURE_REPLAY_BIT_EXT;
    BufferCreateFlag BufferCreateFlags::video_profile_independent_bit_khr = VK_BUFFER_CREATE_VIDEO_PROFILE_INDEPENDENT_BIT_KHR;
    BufferCreateFlag BufferCreateFlags::device_address_capture_replay_bit_ext = VK_BUFFER_CREATE_DEVICE_ADDRESS_CAPTURE_REPLAY_BIT_EXT;
    BufferCreateFlag BufferCreateFlags::device_address_capture_replay_bit_khr = VK_BUFFER_CREATE_DEVICE_ADDRESS_CAPTURE_REPLAY_BIT_KHR;
    BufferCreateFlag BufferCreateFlags::flag_bits_max_enum = VK_BUFFER_CREATE_FLAG_BITS_MAX_ENUM;

	std::string BufferCreateFlags::ToString(BufferCreateFlag bufferCreateFlag)
	{
		if (bufferCreateFlag == 0)
			return "none";

		std::string result;

		auto append = [&](BufferCreateFlag bit, std::string name)
		{
			if (bufferCreateFlag & bit)
			{
				if (!result.empty()) result += " | ";
				result += name;
			}
		};

		append(BufferCreateFlags::sparse_binding_bit, "sparse_binding_bit");
		append(BufferCreateFlags::sparse_residency_bit, "sparse_residency_bit");
		append(BufferCreateFlags::sparse_aliased_bit, "sparse_aliased_bit");
		append(BufferCreateFlags::protected_bit, "protected_bit");
		append(BufferCreateFlags::device_address_capture_replay_bit, "device_address_capture_replay_bit");
		append(BufferCreateFlags::descriptor_buffer_capture_replay_bit_ext, "descriptor_buffer_capture_replay_bit_ext");
		append(BufferCreateFlags::video_profile_independent_bit_khr, "video_profile_independent_bit_khr");
		append(BufferCreateFlags::device_address_capture_replay_bit_ext, "device_address_capture_replay_bit_ext");
		append(BufferCreateFlags::device_address_capture_replay_bit_khr, "device_address_capture_replay_bit_khr");

		return result.empty() ? "unknown" : result;
	}
}