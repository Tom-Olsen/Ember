#pragma once
#include "emberMath.h"
#include <memory>
#include <filesystem>
#include <string>



// Forward decleration:
namespace emberBackendInterface
{
	class IComputeShader;
}



namespace emberEngine
{
	// Forward decleration:
	class Compute;
	class ShaderProperties;



	class ComputeShader
	{
		// Friends:
		friend class Compute;
		friend class ShaderProperties;

	private: // Members:
		std::unique_ptr<emberBackendInterface::IComputeShader> m_pIComputeShader;
		emberBackendInterface::IComputeShader* GetInterfaceHandle();

	public: // Methods:
		// Constructor/Destructor:
		ComputeShader(const std::string& name, const std::filesystem::path& computeSpv);
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
	};
}