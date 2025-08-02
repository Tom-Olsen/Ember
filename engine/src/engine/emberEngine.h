#ifndef __INCLUDE_GUARD_emberEngine_h__
#define __INCLUDE_GUARD_emberEngine_h__



// This is the main header file for the emberEngine.
// It includes all headers that are necessary to use the engine.
// It must be included in every Component scripts header fie.



// Enums: (move somewhere else?)
namespace emberEngine
{
	enum class ProjectionType { perspective, orthographic};
}

// Standart library:
#include <array>
#include <memory>
#include <string>
#include <vector>

// Basic functionality:
#include "eventSystem.h"
#include "logger.h"
#include "emberMath.h"
#include "meshGenerator.h"
#include "emberTime.h"

// Compute backend:
#include "compute.h"
#include "computeShader.h"

// Graphics backend:
#include "graphics.h"
#include "lighting.h"
#include "mesh.h"
#include "material.h"
#include "shaderProperties.h"
#include "sampler.h"
#include "texture2d.h"
#include "depthTexture2dArray.h"
#include "renderTexture2d.h"
#include "sampleTexture2d.h"
#include "storageSampleTexture2d.h"
#include "storageTexture2d.h"
#include "storageBuffer.h"
#include "textureCube.h"

// Managers:
#include "bufferManager.h"
#include "computeShaderManager.h"
#include "materialManager.h"
#include "meshManager.h"
#include "samplerManager.h"
#include "textureManager.h"

// GameObject system:
#include "component.h"
#include "editor.h"
#include "gameObject.h"
#include "scene.h"



#endif // __INCLUDE_GUARD_emberEngine_h__