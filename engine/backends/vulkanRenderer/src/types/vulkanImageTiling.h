#pragma once
#include <cstdint>
#include <string>



namespace vulkanRendererBackend
{
	using ImageTiling = uint32_t;
	struct ImageTilings
	{
		static ImageTiling optimal;
		static ImageTiling linear;
		static ImageTiling drm_format_modifier_ext;
		static ImageTiling max_enum;
		static std::string ToString(ImageTiling imageTiling);
	};
}