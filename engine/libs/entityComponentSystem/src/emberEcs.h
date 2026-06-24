#pragma once

// ECS classes (include order important):
#include "scene.h"
#include "entity.h"
#include "component.h"
#include "entity.inl"
#include "component.inl"

// Basic components:
#include "camera.h"
#include "cameraController.h"
#include "directionalLight.h"
#include "entityState.h"
#include "instancedMeshRenderer.h"
#include "meshRenderer.h"
#include "pointLight.h"
#include "postRenderEffects.h"
#include "spotLight.h"
#include "transform.h"