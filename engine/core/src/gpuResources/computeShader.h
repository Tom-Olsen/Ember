#pragma once
#include <memory>



// Forward decleration:
namespace emberBackendInterface
{
	class IComputeShader;
}



namespace emberEngine
{
	class ComputeShader
	{
	private: // Members:
		std::unique_ptr<emberBackendInterface::IComputeShader> m_pIComputeShader;

	public: // Methods:
		// Constructor/Destructor:
		ComputeShader(const std::string& name, const std::filesystem::path& computeSpv);
		~ComputeShader();

		// Getters:
		Uint3 GetBlockSize() const;
	};
}