#pragma once
#include <cstdint>
#include <string>



namespace vulkanRendererBackend
{
	using BufferCreateFlag = uint32_t;
	struct BufferCreateFlags
	{
        static BufferCreateFlag sparse_binding_bit;
        static BufferCreateFlag sparse_residency_bit;
        static BufferCreateFlag sparse_aliased_bit;
        static BufferCreateFlag protected_bit;
        static BufferCreateFlag device_address_capture_replay_bit;
        static BufferCreateFlag descriptor_buffer_capture_replay_bit_ext;
        static BufferCreateFlag video_profile_independent_bit_khr;
        static BufferCreateFlag device_address_capture_replay_bit_ext;
        static BufferCreateFlag device_address_capture_replay_bit_khr;
        static BufferCreateFlag flag_bits_max_enum;
        static std::string ToString(BufferCreateFlag bufferCreateFlag);
	};
}