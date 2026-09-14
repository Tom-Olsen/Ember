#pragma once
#include "iMaterialShaderManager.h"
#include "vulkanRendererExport.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>



namespace vulkanRendererBackend
{
	// Forward decleration:
	class MaterialManager;
	class MaterialShader;



	class VULKAN_RENDERER_API MaterialShaderManager : public emberBackendInterface::IMaterialShaderManager
	{
		// Friends:
		friend class MaterialManager;

	private: // Structs:
		struct ManagedMaterialShader
		{
			std::string name;
			bool isAccessible;
			std::unique_ptr<MaterialShader> pMaterialShader;

			// Constructor:
			ManagedMaterialShader(std::string name, bool isAccessible, std::unique_ptr<MaterialShader> pMaterialShader);
			
			// Non-copyable:
			ManagedMaterialShader(const ManagedMaterialShader& other) = delete;
			ManagedMaterialShader& operator=(const ManagedMaterialShader& other) = delete;
			
			// Movable:
			ManagedMaterialShader(ManagedMaterialShader&& other) noexcept = default;
			ManagedMaterialShader& operator=(ManagedMaterialShader&& other) noexcept = default;
		};
		struct MaterialShaderSlot
		{
			uint32_t generation;
			ManagedMaterialShader managedMaterialShader;
			
			// Constructor:
			MaterialShaderSlot(uint32_t generation, ManagedMaterialShader managedMaterialShader);
			
			// Non-copyable:
			MaterialShaderSlot(const MaterialShaderSlot& other) = delete;
			MaterialShaderSlot& operator=(const MaterialShaderSlot& other) = delete;
			
			// Movable:
			MaterialShaderSlot(MaterialShaderSlot&& other) noexcept = default;
			MaterialShaderSlot& operator=(MaterialShaderSlot&& other) noexcept = default;
		};

	private: // Members:
		uint32_t m_shadowMapResolution;
		MaterialManager* m_pMaterialManager;
		std::unordered_map<std::string, uint32_t> m_materialShaderIdsMap;
		std::vector<MaterialShaderSlot> m_materialShaderSlots;
		std::vector<uint32_t> m_freeMaterialShaderIds;

	public: // Methods:
		// Constructor/Destructor:
		MaterialShaderManager(uint32_t shadowMapResolution);
		~MaterialShaderManager() override;

		// Non-copyable:
		MaterialShaderManager(const MaterialShaderManager& other) = delete;
		MaterialShaderManager& operator=(const MaterialShaderManager& other) = delete;

		// Non-movable:
		MaterialShaderManager(MaterialShaderManager&& other) = delete;
		MaterialShaderManager& operator=(MaterialShaderManager&& other) = delete;

		// Creators:
		emberCommon::MaterialShaderId CreateGizmoMaterialShader(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name) override;
		emberCommon::MaterialShaderId CreateOutlineMaterialShader(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name);
		emberCommon::MaterialShaderId CreateShadowMaterialShader(const std::filesystem::path& vertexSpv, const std::string& name) override;
		emberCommon::MaterialShaderId CreateDeferredGeometryMaterialShader(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name) override;
		emberCommon::MaterialShaderId CreateDeferredLightingMaterialShader(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name);
		emberCommon::MaterialShaderId CreateForwardMaterialShader(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name) override;
		emberCommon::MaterialShaderId CreatePresentMaterialShader(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name);

		// Getters:
		emberCommon::MaterialShaderId TryGetMaterialShaderId(const std::string& name) const override;
		emberBackendInterface::IMaterialShader* TryGetMaterialShader(emberCommon::MaterialShaderId materialShaderId) const override;
		const std::string* TryGetMaterialShaderName(emberCommon::MaterialShaderId materialShaderId) const override;

		// Deleter:
		void DeleteMaterialShader(emberCommon::MaterialShaderId materialShaderId) override;

		// Debugging:
		void Print() const override;

	private: // Methods:
		// Management:
		emberCommon::MaterialShaderId AddMaterialShader(const std::string& name, bool isAccessible, std::unique_ptr<MaterialShader> pMaterialShader);
		void DestroyMaterialShader(std::unique_ptr<MaterialShader> pMaterialShader);
		void Clear();
		emberCommon::MaterialShaderId FindMaterialShaderId(const std::string& name) const;
		void LinkMaterialManager(MaterialManager* pMaterialManager);
	};
}