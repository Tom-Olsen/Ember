#pragma once
#include "emberCoreExport.h"
#include "emberMath.h"
#include "shader.h"
#include <filesystem>
#include <memory>
#include <string>



// Forward decleration:
namespace emberBackendInterface
{
	class IComputeShader;
}



namespace emberCore
{
	// Forward decleration:
	class Compute;
	class ShaderProperties;



	class EMBER_CORE_API ComputeShader : public Shader
	{
		// Friends:
		friend class Compute;
		friend class ShaderProperties;

	private: // Members:
		std::string m_name;
		std::unique_ptr<emberBackendInterface::IComputeShader> m_pIComputeShader;
		emberBackendInterface::IComputeShader* GetInterfaceHandle();

	public: // Methods:
		// Constructor/Destructor:
		ComputeShader();
		ComputeShader(const std::filesystem::path& computeSpv, const std::string& name);
		~ComputeShader();

		// Non-copyable:
		ComputeShader(const ComputeShader&) = delete;
		ComputeShader& operator=(const ComputeShader&) = delete;

		// Movable:
		ComputeShader(ComputeShader&& other) noexcept;
		ComputeShader& operator=(ComputeShader&& other) noexcept;

		// Getters:
		Uint3 GetBlockSize() const;
		const std::string& GetName() const;

		// Debugging:
		void Print() const;
	};
}