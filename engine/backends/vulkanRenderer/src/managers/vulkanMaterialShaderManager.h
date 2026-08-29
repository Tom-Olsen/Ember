#pragma once
#include "iMaterialShaderManager.h"
#include "vulkanRendererExport.h"
#include <memory>
#include <string>
#include <unordered_map>
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
		};
		struct MaterialShaderSlot
		{
			uint32_t generation;
			ManagedMaterialShader managedMaterialShader;
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