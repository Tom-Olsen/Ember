#pragma once
#include <cstdint>
#include <string_view>



namespace emberCommon
{
	enum class ForwardRenderMode : uint32_t
	{
		opaque,
		transparent,
		skybox,
		wireframe,
		count
	};
	inline constexpr std::string_view ForwardRenderModeToString(ForwardRenderMode renderMode)
	{
		switch (renderMode)
		{
			case ForwardRenderMode::opaque: return "opaque";
			case ForwardRenderMode::transparent: return "transparent";
			case ForwardRenderMode::skybox: return "skybox";
			case ForwardRenderMode::wireframe: return "wireframe";
			case ForwardRenderMode::count: return "count";
			default: return "unknown";
		}
	}
}