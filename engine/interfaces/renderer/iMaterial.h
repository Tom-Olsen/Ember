#pragma once
#include "commonMaterialType.h"
#include <string>



namespace emberBackendInterface
{
    class IMaterial
    {
    public: // Methods:
        // Virtual destructor for v-table:
        virtual ~IMaterial() = default;

        // Getters:
        virtual const std::string& GetName() const = 0;
        virtual emberCommon::MaterialType GetType() const = 0;
        virtual uint32_t GetRenderQueue() const = 0;
    };
}