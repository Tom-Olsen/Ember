#ifndef __INCLUDE_GUARD_managers_h__
#define __INCLUDE_GUARD_managers_h__
#include "bufferManager.h"
#include "computeShaderManager.h"
#include "materialManager.h"
#include "meshManager.h"
#include "poolManager.h"
#include "renderPassManager.h"
#include "samplerManager.h"
#include "textureManager.h"



namespace emberEngine
{
    class Managers
    {
    public: // Methods
        static void Init(uint32_t renderWidth, uint32_t renderHeight);
        static void Clear();

    private: // Methods
        // Delete all constructors:
        Managers() = delete;
        Managers(const Managers&) = delete;
        Managers& operator=(const Managers&) = delete;
        ~Managers() = delete;
    };
}



#endif // __INCLUDE_GUARD_managers_h__