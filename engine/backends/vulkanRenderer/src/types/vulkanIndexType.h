#pragma once
#include <cstdint>
#include <string>



namespace vulkanRendererBackend
{
	using IndexType = uint32_t;
	struct IndexTypes
	{
		static IndexType uint16;
		static IndexType uint32;
		static IndexType uint8;
		static IndexType none_khr;
		static IndexType none_nv;
		static IndexType uint8_ext;
		static IndexType uint8_khr;
		static IndexType max_enum;
		static std::string ToString(IndexType indexType);
	};
}