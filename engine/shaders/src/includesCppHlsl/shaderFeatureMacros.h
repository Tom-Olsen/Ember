#ifndef __INCLUDE_GUARD_shaderFeatureMacros_h__
#define __INCLUDE_GUARD_shaderFeatureMacros_h__



// Compute shader feature declarations:
// Define EMBER_SCENE_COLOR_ACCESS before including computeShaderCommon.hlsli.
#define EMBER_SCENE_COLOR_ACCESS_NONE 0			// shader does not use scene color.
#define EMBER_SCENE_COLOR_ACCESS_READ 1			// shader reads scene color but does not write it.
#define EMBER_SCENE_COLOR_ACCESS_IN_PLACE 2		// shader reads and writes current scene color texture.
#define EMBER_SCENE_COLOR_ACCESS_OUT_OF_PLACE 3	// shader reads current scene color and writes alternate texture.

#ifndef EMBER_SCENE_COLOR_ACCESS
#define EMBER_SCENE_COLOR_ACCESS EMBER_SCENE_COLOR_ACCESS_NONE
#endif



// C++ macro-name bridge:
#ifdef __cplusplus
#include <string_view>



namespace emberCommon
{
	enum class ShaderFeatureMacro
	{
		sceneColorAccess,
		sceneColorAccessNone,
		sceneColorAccessRead,
		sceneColorAccessInPlace,
		sceneColorAccessOutOfPlace
	};

	constexpr std::string_view ToString(ShaderFeatureMacro macro)
	{
		switch (macro)
		{
			case ShaderFeatureMacro::sceneColorAccess:
				return "EMBER_SCENE_COLOR_ACCESS";
			case ShaderFeatureMacro::sceneColorAccessNone:
				return "EMBER_SCENE_COLOR_ACCESS_NONE";
			case ShaderFeatureMacro::sceneColorAccessRead:
				return "EMBER_SCENE_COLOR_ACCESS_READ";
			case ShaderFeatureMacro::sceneColorAccessInPlace:
				return "EMBER_SCENE_COLOR_ACCESS_IN_PLACE";
			case ShaderFeatureMacro::sceneColorAccessOutOfPlace:
				return "EMBER_SCENE_COLOR_ACCESS_OUT_OF_PLACE";
		}
		return {};
	}
}
#endif



#endif // __INCLUDE_GUARD_shaderFeatureMacros_h__