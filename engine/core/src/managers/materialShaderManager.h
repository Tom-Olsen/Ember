#pragma once
#include "commonMaterialShaderCreateInfo.h"
#include "commonMaterialShaderId.h"
#include "emberCoreExport.h"
#include "materialShader.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>



// Forward declarations:
namespace emberAssetLoader
{
	struct MaterialAsset;
}
namespace emberBackendInterface
{
	class IMaterialShader;
}



namespace emberCore
{
	// Forward declarations:
	class Core;
	class MaterialManager;



	class EMBER_CORE_API MaterialShaderManager
	{
		// Friends:
		friend class Core;
		friend class MaterialManager;
		friend class MaterialShader;

	private: // Structs:
		struct ManagedMaterialShader
		{
			emberCommon::MaterialShaderCreateInfo createInfo;
			std::unique_ptr<emberBackendInterface::IMaterialShader> pMaterialShader;

			// Constructor/Destructor:
			ManagedMaterialShader();
			ManagedMaterialShader(emberCommon::MaterialShaderCreateInfo createInfo, std::unique_ptr<emberBackendInterface::IMaterialShader> pMaterialShader);
			~ManagedMaterialShader();

			// Non-copyable:
			ManagedMaterialShader(const ManagedMaterialShader&) = delete;
			ManagedMaterialShader& operator=(const ManagedMaterialShader&) = delete;

			// Movable:
			ManagedMaterialShader(ManagedMaterialShader&&) noexcept;
			ManagedMaterialShader& operator=(ManagedMaterialShader&&) noexcept;
		};
		struct MaterialShaderSlot
		{
			uint32_t generation;
			ManagedMaterialShader managedMaterialShader;

			// Constructor/Destructor:
			MaterialShaderSlot(uint32_t generation, ManagedMaterialShader managedMaterialShader);
			~MaterialShaderSlot();

			// Non-copyable:
			MaterialShaderSlot(const MaterialShaderSlot&) = delete;
			MaterialShaderSlot& operator=(const MaterialShaderSlot&) = delete;

			// Movable:
			MaterialShaderSlot(MaterialShaderSlot&&) noexcept;
			MaterialShaderSlot& operator=(MaterialShaderSlot&&) noexcept;
		};

	private: // Members:
		static bool s_isInitialized;
		static std::unordered_map<std::string, uint32_t> s_materialShaderIdsMap;
		static std::vector<MaterialShaderSlot> s_materialShaderSlots;
		static std::vector<uint32_t> s_freeMaterialShaderIds;

	private: // Methods:
		// Initialization/Cleanup:
		static void Init();
		static void Clear();

		// Creation:
		static MaterialShader CreateMaterialShader(const emberAssetLoader::MaterialAsset& materialAsset);

		// Getters:
		static emberBackendInterface::IMaterialShader* TryGetMaterialShaderInterface(emberCommon::MaterialShaderId materialShaderId);
		static const std::string* TryGetMaterialShaderName(emberCommon::MaterialShaderId materialShaderId);

		// Management:
		static emberCommon::MaterialShaderId AddMaterialShader(const emberCommon::MaterialShaderCreateInfo& createInfo, std::unique_ptr<emberBackendInterface::IMaterialShader> pMaterialShader);
		static void RetireMaterialShader(std::unique_ptr<emberBackendInterface::IMaterialShader> pMaterialShader);
		static emberCommon::MaterialShaderId FindMaterialShaderId(const std::string& name);
		static void InvalidateMaterialShaderSlot(uint32_t index);

		// Delete all constructors:
		MaterialShaderManager() = delete;
		MaterialShaderManager(const MaterialShaderManager&) = delete;
		MaterialShaderManager& operator=(const MaterialShaderManager&) = delete;
		MaterialShaderManager(MaterialShaderManager&&) = delete;
		MaterialShaderManager& operator=(MaterialShaderManager&&) = delete;
		~MaterialShaderManager() = delete;
	};
}