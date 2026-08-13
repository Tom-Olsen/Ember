#pragma once
#include "commonMaterialType.h"
#include "emberCoreExport.h"
#include <filesystem>
#include <string>



// Forward decleration:
namespace emberBackendInterface
{
	class IMaterialShader;
}



namespace emberCore
{
	class EMBER_CORE_API MaterialShader
	{
		// Friends:
		friend class MaterialManager;
		friend class MaterialShaderManager;

	private: // Members:
		std::string m_name;
		emberCommon::MaterialType m_materialType;
		emberBackendInterface::IMaterialShader* m_pIMaterialShader;

	public: // Methods:
		// Constructor/Destructor:
		MaterialShader();
		MaterialShader(emberCommon::MaterialType materialType, emberBackendInterface::IMaterialShader* pIMaterialShader, const std::string& name);
		~MaterialShader();

		// Creation:
		static MaterialShader CreateOutline(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name);
		static MaterialShader CreateForward(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name);
		static MaterialShader CreateGizmo(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name);
		static MaterialShader CreateShadow(const std::filesystem::path& vertexSpv, const std::string& name);
		static MaterialShader CreatePresent(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name);

		// Non-copyable:
		MaterialShader(const MaterialShader&) = delete;
		MaterialShader& operator=(const MaterialShader&) = delete;

		// Movable:
		MaterialShader(MaterialShader&& other) noexcept;
		MaterialShader& operator=(MaterialShader&& other) noexcept;

		// Getters:
		const std::string& GetName() const;
		emberCommon::MaterialType GetMaterialType() const;
		bool IsValid() const;

	private: // Methods:
		emberBackendInterface::IMaterialShader* GetInterfaceHandle() const;
	};
}