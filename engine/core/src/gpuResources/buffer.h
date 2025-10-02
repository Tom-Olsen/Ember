#pragma once
#include "commonBufferUsage.h"
#include <memory>
#include <string>



// Forward decleration:
namespace emberBackendInterface
{
	class IBuffer;
}



namespace emberEngine
{
	class Buffer
	{
		friend class Renderer;
		friend class ShaderProperties;

	private: // Members:
		std::unique_ptr<emberBackendInterface::IBuffer> m_pIBuffer;
		emberBackendInterface::IBuffer* GetInterfaceHandle();

	public: // Methods:
		// Constructor/Destructor:
		Buffer(uint32_t count, uint32_t elementSize, const std::string& name, emberCommon::BufferUsage usage);
		~Buffer();

		// Non-copyable:
		Buffer(const Buffer&) = delete;
		Buffer& operator=(const Buffer&) = delete;

		// Movable:
		Buffer(Buffer&& other) noexcept;
		Buffer& operator=(Buffer&& other) noexcept;

		// Getters:
		std::string GetName() const;
		uint64_t GetSize() const;
		uint32_t GetCount() const;
		uint32_t GetElementSize() const;

		// Upload/Download:
		void Upload(const void* pSrc, uint64_t size);
		void Download(void* pDst, uint64_t size);
	};
}