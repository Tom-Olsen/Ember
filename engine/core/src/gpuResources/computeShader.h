#pragma once
#include "commonComputeShaderId.h"
#include "emberCoreExport.h"
#include "emberMath.h"
#include "shader.h"
#include <string>



// Forward decleration:
namespace emberBackendInterface
{
	class IComputeShader;
	class IDescriptorSetBinding;
}



namespace emberCore
{
	// Forward decleration:
	class Compute;
	class CallProperties;



	class EMBER_CORE_API ComputeShader : public Shader
	{
		// Friends:
		friend class Compute;
		friend class CallProperties;
		friend class ComputeShaderManager;

	private: // Members:
		emberCommon::ComputeShaderId m_computeShaderId;

	public: // Methods:
		// Constructor/Destructor:
		ComputeShader(); // for invalid compute shaders only.
		~ComputeShader();

		// Copyable:
		ComputeShader(const ComputeShader&) = default;
		ComputeShader& operator=(const ComputeShader&) = default;

		// Movable:
		ComputeShader(ComputeShader&& other) noexcept = default;
		ComputeShader& operator=(ComputeShader&& other) noexcept = default;

		// Getters:
		Uint3 GetBlockSize() const;
		const std::string& GetName() const;
		bool IsValid() const;

		// Debugging:
		void Print() const;

	private: // Methods:
		explicit ComputeShader(emberCommon::ComputeShaderId computeShaderId);
		emberBackendInterface::IComputeShader* TryGetInterfaceHandle() const;
		emberBackendInterface::IDescriptorSetBinding* TryGetShaderDescriptorSetBinding() const override;
	};
}