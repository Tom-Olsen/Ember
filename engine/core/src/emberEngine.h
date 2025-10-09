#pragma once



// This is the main header file for the emberEngine.
// It includes all headers that are necessary to use the engine.
// It must be included in every Component scripts header fie.



// Ember::ToDo: move this somewhere else!
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

// Common (core <-> backend) communication: (Not all included on purpose for now)
#include "commonAccessMask.h"
#include "commonInput.h"
#include "commonLighting.h"

// Common Aliases:
namespace emberEngine
{
    // Input aliasing:
    using Input = emberCommon::Input;
    using MouseButton = emberCommon::Input::MouseButton;
    using ControllerButton = emberCommon::Input::ControllerButton;
    using Key = emberCommon::Input::Key;

    // Compute shader aliasing:
    using AccessMasks = emberCommon::AccessMasks;
    using BufferUsage = emberCommon::BufferUsage;

    using ShadowType = emberCommon::ShadowType;
}