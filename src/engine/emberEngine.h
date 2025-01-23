#ifndef __INCLUDE_GUARD_emberEngine_h__
#define __INCLUDE_GUARD_emberEngine_h__



// This is the main header file for the emberEngine.
// It includes all headers that are necessary to use the engine.
// It must be included in every Component scripts header fie.



// Enums: (move somewher else?)
enum class ShadowType { hard, soft };
enum class ProjectionType { perspective, orthographic};

// Basic functionality:
#include "eventSystem.h"
#include "logger.h"
#include "mathf.h"
#include "meshGenerator.h"
#include "timer.h"

// Rendering backend:
#include "graphics.h"
#include "mesh.h"
#include "material.h"
#include "materialProperties.h"
#include "sampler.h"
#include "texture2d.h"
#include "textureCube.h"

// Managers:
#include "materialManager.h"
#include "meshManager.h"
#include "samplerManager.h"
#include "textureManager.h"

// GameObject system:
#include "component.h"
#include "gameObject.h"
#include "scene.h"

// GameObject Components:
#include "camera.h"
#include "cameraController.h"
#include "directionalLight.h"
#include "drawMeshData.h"
#include "meshRenderer.h"
#include "meshTester.h"
#include "pointLight.h"
#include "rotationController.h"
#include "spinGlobal.h"
#include "spotLight.h"
#include "spinLocal.h"
#include "transform.h"



#endif // __INCLUDE_GUARD_emberEngine_h__