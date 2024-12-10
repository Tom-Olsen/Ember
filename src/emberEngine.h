#ifndef __INCLUDE_GUARD_emberEngine_h__
#define __INCLUDE_GUARD_emberEngine_h__



// This is the main header file for the emberEngine.
// It includes all headers that are necessary to use the engine.
// It must be included in every Component scripts header fie.

// Basic functionality:
#include "eventSystem.h"
#include "logger.h"
#include "mathf.h"
#include "timer.h"

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
#include "pointLight.h"
#include "spinGlobal.h"
#include "spotLight.h"
#include "spinLocal.h"
#include "transform.h"



#endif // __INCLUDE_GUARD_emberEngine_h__