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
#include "emberMath.h"
#include "emberTime.h"
#include "logger.h"
#include "meshGenerator.h"

// Render/Compute backends:
#include "renderer.h"
#include "compute.h"

// Gpu resources:
#include "buffer.h"
#include "mesh.h"
#include "material.h"
#include "computeShader.h"
#include "shaderProperties.h"

// Managers:
#include "bufferManager.h"
#include "computeShaderManager.h"
#include "materialManager.h"
#include "meshManager.h"
#include "textureManager.h"

// Game Object System (ECS):
#include "component.h"
#include "gameObject.h"
#include "scene.h"



#endif // __INCLUDE_GUARD_emberEngine_h__