#pragma once
#include <cstdint>



namespace emberCommon
{
	/// <summary>
	/// Describes how a compute shader accesses the FRAME_SET scene color textures.
	/// Derived from its EMBER_SCENE_COLOR_ACCESS declaration and used to select textures, flip output, and insert barriers.
	/// </summary>
	enum class SceneColorAccess : uint8_t
	{
		none,
		read,
		inPlace,
		outOfPlace
	};
}