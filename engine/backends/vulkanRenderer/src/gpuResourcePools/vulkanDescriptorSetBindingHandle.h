#pragma once
#include "vulkanShaderHandle.h"



namespace vulkanRendererBackend
{
	// Forward declarations:
	class DescriptorSetBinding;



	class DescriptorSetBindingHandle
	{
	private: // Members:
		DescriptorSetBinding* m_pDescriptorSetBinding;
		ShaderHandle m_poolShaderHandle;  // Invalid if m_pDescriptorSetBinding does not originate from the pooling system.

	public: // Methods:
		// Constructors/Destructor:
		DescriptorSetBindingHandle();
		explicit DescriptorSetBindingHandle(DescriptorSetBinding* pDescriptorSetBinding);
		DescriptorSetBindingHandle(const ShaderHandle& poolShaderHandle, DescriptorSetBinding* pDescriptorSetBinding);
		~DescriptorSetBindingHandle();

		// Getters:
		DescriptorSetBinding* Get() const;
		const ShaderHandle& GetPoolShaderHandle() const;
		bool IsValid() const;
		bool IsPooled() const;
	};
}