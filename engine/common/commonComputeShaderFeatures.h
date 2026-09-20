#pragma once
#include "commonSceneColorAccess.h"



namespace emberCommon
{
	struct ComputeShaderFeatures
	{
	public: // Members:
		SceneColorAccess sceneColorAccess = SceneColorAccess::none;

	public: // Methods:
		inline bool ReadsSceneColor() const
		{
			return sceneColorAccess == SceneColorAccess::read || sceneColorAccess == SceneColorAccess::inPlace || sceneColorAccess == SceneColorAccess::outOfPlace;
		}
		inline bool WritesSceneColor() const
		{
			return sceneColorAccess == SceneColorAccess::inPlace || sceneColorAccess == SceneColorAccess::outOfPlace;
		}
		inline bool FlipsSceneColor() const
		{
			return sceneColorAccess == SceneColorAccess::outOfPlace;
		}
	};
}