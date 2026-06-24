#pragma once



// Main header for EmberCore.
// Exposes all rendering, GPU resource, manager, and event functionality.



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

// Renderer/Compute:
#include "renderer.h"
#include "debugRenderer.h"
#include "gizmo.h"
#include "compute.h"

// Gpu resources:
#include "buffer.h"
#include "bufferView.h"
#include "bufferTyped.h"
#include "tripleBuffer.h"
#include "physicsTripleBufferState.h"
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

// Other:
#include "gpuSort.h"

// Common (core <-> backend) communication: (Not all included on purpose for now)
#include "commonInput.h"
#include "commonLighting.h"

// Common Aliases:
namespace emberCore
{
    // Input aliasing:
    using Input = emberCommon::Input;
    using MouseButton = emberCommon::Input::MouseButton;
    using ControllerButton = emberCommon::Input::ControllerButton;
    using Key = emberCommon::Input::Key;

    using BufferUsage = emberCommon::BufferUsage;
    using ShadowType = emberCommon::ShadowType;
}