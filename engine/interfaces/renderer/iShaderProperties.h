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

        // Setters:
		// Simple members:
        virtual void SetInt(const std::string& bufferName, const std::string& memberName, int value) = 0;
        virtual void SetBool(const std::string& bufferName, const std::string& memberName, bool value) = 0;
        virtual void SetFloat(const std::string& bufferName, const std::string& memberName, float value) = 0;
        virtual void SetFloat2(const std::string& bufferName, const std::string& memberName, const Float2& value) = 0;
        virtual void SetFloat3(const std::string& bufferName, const std::string& memberName, const Float3& value) = 0;
        virtual void SetFloat4(const std::string& bufferName, const std::string& memberName, const Float4& value) = 0;
        virtual void SetFloat4x4(const std::string& bufferName, const std::string& memberName, const Float4x4& value) = 0;
		// Array members:
        virtual void SetInt(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, int value) = 0;
        virtual void SetBool(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, bool value) = 0;
        virtual void SetFloat(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, float value) = 0;
        virtual void SetFloat2(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const Float2& value) = 0;
        virtual void SetFloat3(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const Float3& value) = 0;
        virtual void SetFloat4(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const Float4& value) = 0;
        virtual void SetFloat4x4(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const Float4x4& value) = 0;
		// Array of structs members:
        virtual void SetInt(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, int value) = 0;
        virtual void SetBool(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, bool value) = 0;
        virtual void SetFloat(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, float value) = 0;
        virtual void SetFloat2(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const Float2& value) = 0;
        virtual void SetFloat3(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const Float3& value) = 0;
        virtual void SetFloat4(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const Float4& value) = 0;
        virtual void SetFloat4x4(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const Float4x4& value) = 0;
		// Array of arrays members:
        virtual void SetInt(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, int value) = 0;
        virtual void SetBool(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, bool value) = 0;
        virtual void SetFloat(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, float value) = 0;
        virtual void SetFloat2(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const Float2& value) = 0;
        virtual void SetFloat3(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const Float3& value) = 0;
        virtual void SetFloat4(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const Float4& value) = 0;
        virtual void SetFloat4x4(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const Float4x4& value) = 0;

        // Getters:
		// Simple members:
        virtual int GetInt(const std::string& bufferName, const std::string& memberName) const = 0;
		virtual bool GetBool(const std::string& bufferName, const std::string& memberName) const = 0;
		virtual float GetFloat(const std::string& bufferName, const std::string& memberName) const = 0;
		virtual Float2 GetFloat2(const std::string& bufferName, const std::string& memberName) const = 0;
		virtual Float3 GetFloat3(const std::string& bufferName, const std::string& memberName) const = 0;
		virtual Float4 GetFloat4(const std::string& bufferName, const std::string& memberName) const = 0;
		virtual Float4x4 GetFloat4x4(const std::string& bufferName, const std::string& memberName) const = 0;
		// Array members:
		virtual int GetInt(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex) const = 0;
		virtual bool GetBool(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex) const = 0;
		virtual float GetFloat(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex) const = 0;
		virtual Float2 GetFloat2(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex) const = 0;
		virtual Float3 GetFloat3(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex) const = 0;
		virtual Float4 GetFloat4(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex) const = 0;
		virtual Float4x4 GetFloat4x4(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex) const = 0;
		// Array of structs members:
		virtual int GetInt(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName) const = 0;
		virtual bool GetBool(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName) const = 0;
		virtual float GetFloat(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName) const = 0;
		virtual Float2 GetFloat2(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName) const = 0;
		virtual Float3 GetFloat3(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName) const = 0;
		virtual Float4 GetFloat4(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName) const = 0;
		virtual Float4x4 GetFloat4x4(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName) const = 0;
		// Array of arrays members:
		virtual int GetInt(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex) const = 0;
		virtual bool GetBool(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex) const = 0;
		virtual float GetFloat(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex) const = 0;
		virtual Float2 GetFloat2(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex) const = 0;
		virtual Float3 GetFloat3(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex) const = 0;
		virtual Float4 GetFloat4(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex) const = 0;
		virtual Float4x4 GetFloat4x4(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex) const = 0;
    };
}