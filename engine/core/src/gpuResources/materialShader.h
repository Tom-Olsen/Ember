#pragma once
#include "commonMaterialPass.h"
#include "commonMaterialShaderId.h"
#include "emberCoreExport.h"
#include <string>



// Forward declarations:
namespace emberBackendInterface
{
	class IMaterialShader;
}



namespace emberCore
{
	// Forward declarations:
	class MaterialManager;
	class MaterialShaderManager;



	class EMBER_CORE_API MaterialShader
	{
		// Friends:
		friend class MaterialManager;
		friend class MaterialShaderManager;

	private: // Members:
		emberCommon::MaterialShaderId m_materialShaderId;

	public: // Methods:
		// Constructor/Destructor:
		MaterialShader(); // for invalid material shaders only.
		~MaterialShader();

		// Copyable:
		MaterialShader(const MaterialShader&) = default;
		MaterialShader& operator=(const MaterialShader&) = default;

		// Movable:
		MaterialShader(MaterialShader&&) noexcept = default;
		MaterialShader& operator=(MaterialShader&&) noexcept = default;

		// Getters:
		const std::string& GetName() const;
		emberCommon::MaterialPass GetMaterialPass() const;
		bool IsValid() const;

		// Debugging:
		void Print() const;

	private: // Methods:
		explicit MaterialShader(emberCommon::MaterialShaderId materialShaderId);
		emberBackendInterface::IMaterialShader* TryGetInterfaceHandle() const;
	};
}