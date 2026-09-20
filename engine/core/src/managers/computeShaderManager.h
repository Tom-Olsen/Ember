#pragma once
#include "commonComputeShaderId.h"
#include "computeShader.h"
#include "emberCoreExport.h"
#include <filesystem>
#include <string>



// Forward declarations:
namespace emberAssetLoader
{
	struct ComputeShaderAsset;
}
namespace emberBackendInterface
{
	class IComputeShader;
	class IComputeShaderManager;
}



namespace emberCore
{
	// Forward declarations:
	class Renderer;



	/// <summary>
	/// Static facade for the backend compute shader manager.
	/// ComputeShader is a non-owning, generational handle to a backend-owned slot.
	/// </summary>
	class EMBER_CORE_API ComputeShaderManager
	{
		// Friends:
		friend class ComputeShader;
		friend class Renderer;

	private: // Members:
		static emberBackendInterface::IComputeShaderManager* s_pIComputeShaderManager;

	public: // Methods:
		// Asset loading:
		static void LoadComputeShaderAssets(const std::filesystem::path& directoryPath);

		// Getters:
		static ComputeShader TryGetComputeShader(const std::string& name);

		// Debugging:
		static void Print();

	private: // Methods:
		// Initialization/Cleanup:
		static void Init();
		static void Clear();

		// Getters:
		static bool IsComputeShaderMutable(emberCommon::ComputeShaderId computeShaderId);
		static emberBackendInterface::IComputeShader* TryGetComputeShaderInterface(emberCommon::ComputeShaderId computeShaderId);
		static const std::string* TryGetComputeShaderName(emberCommon::ComputeShaderId computeShaderId);

        // Delete all constructors:
		ComputeShaderManager() = delete;
		ComputeShaderManager(const ComputeShaderManager&) = delete;
		ComputeShaderManager& operator=(const ComputeShaderManager&) = delete;
		ComputeShaderManager(ComputeShaderManager&&) = delete;
		ComputeShaderManager& operator=(ComputeShaderManager&&) = delete;
		~ComputeShaderManager() = delete;
	};
}