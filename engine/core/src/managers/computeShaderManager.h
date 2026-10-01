#pragma once
#include "commonComputeShaderId.h"
#include "commonResourceAccessRights.h"
#include "computeShader.h"
#include "emberCoreExport.h"
#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>



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
			std::string name;
			emberCommon::ResourceAccessRights accessRights;
			std::unique_ptr<emberBackendInterface::IComputeShader> pComputeShader;

			// Constructor/Destructor:
			ManagedComputeShader();
			ManagedComputeShader(std::string name, const emberCommon::ResourceAccessRights& accessRights, std::unique_ptr<emberBackendInterface::IComputeShader> pComputeShader);
			~ManagedComputeShader();

			// Non-copyable:
			ManagedComputeShader(const ManagedComputeShader&) = delete;
			ManagedComputeShader& operator=(const ManagedComputeShader&) = delete;

			// Movable:
			ManagedComputeShader(ManagedComputeShader&&) noexcept;
			ManagedComputeShader& operator=(ManagedComputeShader&&) noexcept;
		};
		struct ComputeShaderSlot
		{
			uint32_t generation;
			ManagedComputeShader managedComputeShader;

			// Constructor/Destructor:
			ComputeShaderSlot(uint32_t generation, ManagedComputeShader managedComputeShader);
			~ComputeShaderSlot();

			// Non-copyable:
			ComputeShaderSlot(const ComputeShaderSlot&) = delete;
			ComputeShaderSlot& operator=(const ComputeShaderSlot&) = delete;

			// Movable:
			ComputeShaderSlot(ComputeShaderSlot&&) noexcept;
			ComputeShaderSlot& operator=(ComputeShaderSlot&&) noexcept;
		};

	private: // Members:
		static bool s_isInitialized;
		static std::unordered_map<std::string, uint32_t> s_computeShaderIdsMap;
		static std::vector<ComputeShaderSlot> s_computeShaderSlots;
		static std::vector<uint32_t> s_freeComputeShaderIds;

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
		static const std::string* TryGetComputeShaderName(emberCommon::ComputeShaderId computeShaderId);

		// Ownership transfer:
		static std::unique_ptr<emberBackendInterface::IComputeShader> TakeComputeShaderInterface(const std::string& name);

		// Deleter:
		static void DeleteComputeShader(emberCommon::ComputeShaderId computeShaderId);

		// Management:
		static emberCommon::ComputeShaderId AddComputeShader(const std::string& name, const emberCommon::ResourceAccessRights& accessRights, std::unique_ptr<emberBackendInterface::IComputeShader> pComputeShader);
		static void RetireComputeShader(std::unique_ptr<emberBackendInterface::IComputeShader> pComputeShader);
		static emberCommon::ComputeShaderId FindComputeShaderId(const std::string& name);
		static void InvalidateComputeShaderSlot(uint32_t index);

		// Delete all constructors:
		ComputeShaderManager() = delete;
		ComputeShaderManager(const ComputeShaderManager&) = delete;
		ComputeShaderManager& operator=(const ComputeShaderManager&) = delete;
		ComputeShaderManager(ComputeShaderManager&&) = delete;
		ComputeShaderManager& operator=(ComputeShaderManager&&) = delete;
		~ComputeShaderManager() = delete;
	};
}