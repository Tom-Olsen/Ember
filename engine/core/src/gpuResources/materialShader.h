#pragma once
#include "commonMaterialType.h"
#include "emberCoreExport.h"
#include "materialShaderId.h"
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
		MaterialShaderId m_materialShaderId;

	public: // Methods:
		// Constructor/Destructor:
		MaterialShader();
		~MaterialShader();

		// Creation/Destruction:
		static MaterialShader CreateForward(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name);
		static MaterialShader CreateGizmo(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name);
		static MaterialShader CreateShadow(const std::filesystem::path& vertexSpv, const std::string& name);
		void Destroy();

		// Copyable:
		MaterialShader(const MaterialShader&) = default;
		MaterialShader& operator=(const MaterialShader&) = default;

		// Movable:
		MaterialShader(MaterialShader&& other) noexcept = default;
		MaterialShader& operator=(MaterialShader&& other) noexcept = default;

		// Getters:
		const std::string& GetName() const;
		emberCommon::MaterialType GetMaterialType() const;
		bool IsValid() const;

	private: // Methods:
		MaterialShader(MaterialShaderId materialShaderId);
		emberBackendInterface::IMaterialShader* GetInterfaceHandle() const;
	};
}