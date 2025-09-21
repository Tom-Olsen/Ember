#pragma once
#include <cstdint>
#include <string>



namespace vulkanRendererBackend
{
	using ColorSpace = uint32_t;
	struct ColorSpaces
	{
		static ColorSpace srgb_nonlinear;
		static ColorSpace display_p3_nonlinear;
		static ColorSpace extended_srgb_linear;
		static ColorSpace display_p3_linear;
		static ColorSpace dci_p3_nonlinear;
		static ColorSpace bt709_linear;
		static ColorSpace bt709_nonlinear;
		static ColorSpace bt2020_linear;
		static ColorSpace hdr10_st2084;
		static ColorSpace dolbyvision;
		static ColorSpace hdr10_hlg;
		static ColorSpace adobergb_linear;
		static ColorSpace adobergb_nonlinear;
		static ColorSpace pass_through;
		static ColorSpace extended_srgb_nonlinear;
		static ColorSpace display_native;
		static ColorSpace max_enum;
		static std::string ToString(ColorSpace colorSpace);
	};
}