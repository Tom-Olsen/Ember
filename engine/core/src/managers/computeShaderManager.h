#pragma once
#include "commonComputeShaderId.h"
#include "commonResourceAccessRights.h"
#include "computeShader.h"
#include "emberCoreExport.h"
#include "namedSlotMap.h"
#include <filesystem>
#include <memory>
#include <string>



// Forward declarations:
namespace emberAssetLoader
{
	struct ComputeShaderAsset;
}
namespace emberBackendInterface
{
	class IComputeShader;
}



namespace emberCore
{
	// Forward declarations:
	class Core;
	class Renderer;



	class EMBER_CORE_API ComputeShaderManager
	{
		// Friends:
		friend class ComputeShader;
		friend class Core;
		friend class Renderer;

	private: // Structs:
		struct ManagedComputeShader
		{
			emberCommon::ResourceAccessRights accessRights;
			std::unique_ptr<emberBackendInterface::IComputeShader> pComputeShader;

			// Constructor/Destructor:
			ManagedComputeShader();
			ManagedComputeShader(const emberCommon::ResourceAccessRights& accessRights, std::unique_ptr<emberBackendInterface::IComputeShader> pComputeShader);
			~ManagedComputeShader();

			// Non-copyable:
			ManagedComputeShader(const ManagedComputeShader&) = delete;
			ManagedComputeShader& operator=(const ManagedComputeShader&) = delete;

			// Movable:
			ManagedComputeShader(ManagedComputeShader&&) noexcept;
			ManagedComputeShader& operator=(ManagedComputeShader&&) noexcept;
		};

	private: // Members:
		static bool s_isInitialized;
		static emberDataStructures::NamedSlotMap<emberCommon::ComputeShaderId, ManagedComputeShader> s_computeShaderSlotMap;

	public: // Methods:
		// Asset loading:
		static void LoadComputeShaderAssets(const std::filesystem::path& directoryPath);

		// Getters:
		static ComputeShader TryGetComputeShader(const std::string& name);

		// Deleter:
		static void DeleteComputeShader(const std::string& name);

		// Debugging:
		static void Print();

	private: // Methods:
		// Initialization/Cleanup:
		static void Init();
		static void Clear();

		// Creation:
		static emberCommon::ComputeShaderId CreateComputeShader(const emberAssetLoader::ComputeShaderAsset& computeShaderAsset);

		// Getters:
		static emberCommon::ComputeShaderId TryGetAccessibleComputeShaderId(const std::string& name);
		static bool IsComputeShaderMutable(emberCommon::ComputeShaderId computeShaderId);
		static emberBackendInterface::IComputeShader* TryGetComputeShaderInterface(emberCommon::ComputeShaderId computeShaderId);
		static std::string GetComputeShaderName(emberCommon::ComputeShaderId computeShaderId);

		// Ownership transfer:
		static std::unique_ptr<emberBackendInterface::IComputeShader> TakeComputeShaderOwnership(const std::string& name);

		// Deleter:
		static void DeleteComputeShader(emberCommon::ComputeShaderId computeShaderId);

		// Management:
		static emberCommon::ComputeShaderId AddComputeShader(const std::string& name, const emberCommon::ResourceAccessRights& accessRights, std::unique_ptr<emberBackendInterface::IComputeShader> pComputeShader);
		static void RetireComputeShader(std::unique_ptr<emberBackendInterface::IComputeShader> pComputeShader);
		static emberCommon::ComputeShaderId FindComputeShaderId(const std::string& name);

		// Delete all constructors:
		ComputeShaderManager() = delete;
		ComputeShaderManager(const ComputeShaderManager&) = delete;
		ComputeShaderManager& operator=(const ComputeShaderManager&) = delete;
		ComputeShaderManager(ComputeShaderManager&&) = delete;
		ComputeShaderManager& operator=(ComputeShaderManager&&) = delete;
		~ComputeShaderManager() = delete;
	};
}