#ifndef __INCLUDE_GUARD_emberEngine_h__
#define __INCLUDE_GUARD_emberEngine_h__



// This is the main header file for the emberEngine.
// It includes all headers that are necessary to use the engine.
// It must be included in every Component scripts header fie.



// Enums: (move somewher else?)
namespace emberEngine
{
	enum class ShadowType { hard, soft };
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
#include "mathf.h"
#include "meshGenerator.h"
#include "timer.h"

// Rendering/Compute backend:
#include "computeShader.h"
#include "compute.h"
#include "graphics.h"
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
//#include "renderPassManager.h"	// only needed in backend, not on engine level.
#include "samplerManager.h"
#include "textureManager.h"

// GameObject system:
#include "component.h"
#include "gameObject.h"
#include "scene.h"

// GameObject Components:
#include "camera.h"
#include "cameraController.h"
#include "computeUnit.h"
#include "directionalLight.h"
#include "drawMeshData.h"
#include "instancedRenderer.h"
#include "meshRenderer.h"
#include "meshTester.h"
#include "pointLight.h"
#include "spinGlobal.h"
#include "spinLocal.h"
#include "spotLight.h"
#include "testInstancedRendering.h"
#include "transform.h"



#endif // __INCLUDE_GUARD_emberEngine_h__