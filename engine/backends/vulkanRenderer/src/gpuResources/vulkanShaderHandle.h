#pragma once
#include "vulkanGpuResourceHandle.h"
#include "vulkanRendererExport.h"
#include <cstddef>



namespace vulkanRendererBackend
{
	// Forward declarations:
	class Shader;



	class VULKAN_RENDERER_API ShaderHandle
	{
	private: // Members:
		GpuResourceHandle m_resourceHandle;

	public: // Methods:
		// Constructor/Destructor:
		ShaderHandle();
		explicit ShaderHandle(const Shader& shader);
		~ShaderHandle();

		// Copyable:
		ShaderHandle(const ShaderHandle&) = default;
		ShaderHandle& operator=(const ShaderHandle&) = default;

		// Movable:
		ShaderHandle(ShaderHandle&&) noexcept = default;
		ShaderHandle& operator=(ShaderHandle&&) noexcept = default;

		// Getters:
		Shader* Get() const;
		Shader* TryGet() const;
		bool IsValid() const;
		bool IsEmpty() const;

		// Comparison operators:
		bool operator==(const ShaderHandle& other) const;
		bool operator!=(const ShaderHandle& other) const;

		// Hashing:
		struct Hasher
		{
			size_t operator()(const ShaderHandle& shaderHandle) const;
		};
	};
}