#pragma once
#include "materialType.h"



namespace emberBackendInterface
{
    class IMaterial
    {
    public: // Methods:
        // Virtual destructor for v-table:
        virtual ~IMaterial() = default;

        // Getters:
        virtual emberEngine::MaterialType GetType() const = 0;
        virtual uint32_t GetRenderQueue() const = 0;
    };
}