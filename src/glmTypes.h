#ifndef __INCLUDE_GUARD_glmTypes_h__
#define __INCLUDE_GUARD_glmTypes_h__



// glm library:
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/matrix_access.hpp>

// using statements, to_string conversion, and ostream overloading for easier access to glm types:
#include "floatTypes.h"
#include "doubleTypes.h"
#include "intTypes.h"
#include "uintTypes.h"
using Quaternion = glm::quat;



#endif // __INCLUDE_GUARD_glmTypes_h__