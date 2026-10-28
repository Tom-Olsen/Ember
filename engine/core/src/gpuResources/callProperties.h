#pragma once
#include "emberCoreExport.h"
#include "emberMath.h"
#include <cstdint>
#include <memory>
#include <string>



// Forward decleration:
namespace emberBackendInterface
{
	class IBuffer;
	class IDescriptorSetBinding;
}



namespace emberCore
{
	// Forward decleration:
	class Buffer;
	class Compute;
	class Material;
	class StorageBuffer;
	class Texture;



	class EMBER_CORE_API CallProperties
	{
		// Friends:
		friend class Renderer;
		friend class Compute;

	private: // Members:
		mutable bool m_ownsICallDescriptorSetBinding;
		mutable bool m_callDescriptorSetBindingExpired;
		mutable uint64_t m_callDescriptorSetBindingGeneration;
		mutable emberBackendInterface::IDescriptorSetBinding* m_pICallDescriptorSetBinding; // conditional ownership, depending on usecase.
		std::unique_ptr<CallProperties> m_pShadowProperties;	// callProperties of associated shadow draw call.

	public: // Methods:
		// Constructors/Destructor:
		CallProperties();
		CallProperties(const Material& material);
		~CallProperties();

		// Non-copyable:
		CallProperties(const CallProperties&) = delete;
		CallProperties& operator=(const CallProperties&) = delete;

		// Movable:
		CallProperties(CallProperties&& other) noexcept;
		CallProperties& operator=(CallProperties&& other) noexcept;

		// Setters:
		void SetTexture(const std::string& name, const Texture& texture);
		void SetBuffer(const std::string& name, Buffer& buffer);

		// Uniform Buffer Setters:
		// Simple members:
		void SetValue(const std::string& bufferName, const std::string& memberName, int value);
		void SetValue(const std::string& bufferName, const std::string& memberName, bool value);
		void SetValue(const std::string& bufferName, const std::string& memberName, float value);
		void SetValue(const std::string& bufferName, const std::string& memberName, const Float2& value);
		void SetValue(const std::string& bufferName, const std::string& memberName, const Float3& value);
		void SetValue(const std::string& bufferName, const std::string& memberName, const Float4& value);
		void SetValue(const std::string& bufferName, const std::string& memberName, const Float4x4& value);
		// Array members:
		void SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, int value);
		void SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, bool value);
		void SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, float value);
		void SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const Float2& value);
		void SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const Float3& value);
		void SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const Float4& value);
		void SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const Float4x4& value);
		// Struct members inside arrays:
		void SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, int value);
		void SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, bool value);
		void SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, float value);
		void SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const Float2& value);
		void SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const Float3& value);
		void SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const Float4& value);
		void SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const Float4x4& value);
		// Arrays inside arrays:
		void SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, int value);
		void SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, bool value);
		void SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, float value);
		void SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const Float2& value);
		void SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const Float3& value);
		void SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const Float4& value);
		void SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const Float4x4& value);

		// Getters:
		bool HasBinding(const std::string& name);
		bool HasShadowProperties();
		CallProperties& GetShadowProperties();

		// Uniform Buffer Getters:
		// Simple members:
		int GetInt(const std::string& bufferName, const std::string& memberName) const;
		bool GetBool(const std::string& bufferName, const std::string& memberName) const;
		float GetFloat(const std::string& bufferName, const std::string& memberName) const;
		Float2 GetFloat2(const std::string& bufferName, const std::string& memberName) const;
		Float3 GetFloat3(const std::string& bufferName, const std::string& memberName) const;
		Float4 GetFloat4(const std::string& bufferName, const std::string& memberName) const;
		Float4x4 GetFloat4x4(const std::string& bufferName, const std::string& memberName) const;
		// Array members:
		int GetInt(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex) const;
		bool GetBool(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex) const;
		float GetFloat(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex) const;
		Float2 GetFloat2(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex) const;
		Float3 GetFloat3(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex) const;
		Float4 GetFloat4(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex) const;
		Float4x4 GetFloat4x4(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex) const;
		// Array of structs members:
		int GetInt(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName) const;
		bool GetBool(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName) const;
		float GetFloat(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName) const;
		Float2 GetFloat2(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName) const;
		Float3 GetFloat3(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName) const;
		Float4 GetFloat4(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName) const;
		Float4x4 GetFloat4x4(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName) const;
		// Array of arrays members:
		int GetInt(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex) const;
		bool GetBool(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex) const;
		float GetFloat(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex) const;
		Float2 GetFloat2(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex) const;
		Float3 GetFloat3(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex) const;
		Float4 GetFloat4(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex) const;
		Float4x4 GetFloat4x4(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex) const;

		// Debugging:
		void Print() const;
		void PrintMaps() const;

	private: // Methods:
		// Pooling constructor:
		// Wraps non-owned call descriptor set bindings supplied by Renderer/Compute call pools.
		CallProperties(emberBackendInterface::IDescriptorSetBinding* pICallDescriptorSetBinding);
		void ValidateCallDescriptorSetBinding() const;
		void SetShadowProperties(CallProperties&& shadowProperties);
		emberBackendInterface::IDescriptorSetBinding* GetCallInterfaceHandle();
		emberBackendInterface::IDescriptorSetBinding* GetValidCallInterfaceHandle() const;

		// Setters:
		void SetBuffer(const std::string& name, emberBackendInterface::IBuffer& buffer);
	};
}