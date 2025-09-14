#pragma once
#include "emberMath.h"
#include <string>



// Forward declerations:
namespace vulkanRendererBackend
{
    class Sampler;
    class Texture2d;
    class StorageBuffer;
}



namespace emberBackendInterface
{
    class IShaderProperties
    {
    public: // Virtual destructor for v-table:
        virtual ~IShaderProperties() = default;

    public: // Methods:
        //virtual void SetSampler(const std::string& name, vulkanRendererBackend::Sampler* pSampler) = 0;
        virtual void SetTexture2d(const std::string& name, vulkanRendererBackend::Texture2d* pTexture2d) = 0;
        virtual void SetStorageBuffer(const std::string& name, vulkanRendererBackend::StorageBuffer* pStorageBuffer) = 0;

        virtual void SetValue(const std::string& bufferName, const std::string& memberName, int value) = 0;
        virtual void SetValue(const std::string& bufferName, const std::string& memberName, bool value) = 0;
        virtual void SetValue(const std::string& bufferName, const std::string& memberName, float value) = 0;
        virtual void SetValue(const std::string& bufferName, const std::string& memberName, const Float2& value) = 0;
        virtual void SetValue(const std::string& bufferName, const std::string& memberName, const Float3& value) = 0;
        virtual void SetValue(const std::string& bufferName, const std::string& memberName, const Float4& value) = 0;
        virtual void SetValue(const std::string& bufferName, const std::string& memberName, const Float4x4& value) = 0;

        virtual void SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, int value) = 0;
        virtual void SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, bool value) = 0;
        virtual void SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, float value) = 0;
        virtual void SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const Float2& value) = 0;
        virtual void SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const Float3& value) = 0;
        virtual void SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const Float4& value) = 0;
        virtual void SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const Float4x4& value) = 0;

        virtual void SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, int value) = 0;
        virtual void SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, bool value) = 0;
        virtual void SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, float value) = 0;
        virtual void SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const Float2& value) = 0;
        virtual void SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const Float3& value) = 0;
        virtual void SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const Float4& value) = 0;
        virtual void SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const Float4x4& value) = 0;

        virtual void SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, int value) = 0;
        virtual void SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, bool value) = 0;
        virtual void SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, float value) = 0;
        virtual void SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const Float2& value) = 0;
        virtual void SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const Float3& value) = 0;
        virtual void SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const Float4& value) = 0;
        virtual void SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const Float4x4& value) = 0;
    };
}