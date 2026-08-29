#pragma once
#include "commonMaterialShaderId.h"
#include <filesystem>
#include <string>



namespace emberBackendInterface
{
	class IMaterialShader;



	class IMaterialShaderManager
	{
	public: // Methods:
		// Virtual destructor for v-table:
		virtual ~IMaterialShaderManager() = default;

		// Creators:
		virtual emberCommon::MaterialShaderId CreateGizmoMaterialShader(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name) = 0;
		virtual emberCommon::MaterialShaderId CreateShadowMaterialShader(const std::filesystem::path& vertexSpv, const std::string& name) = 0;
		virtual emberCommon::MaterialShaderId CreateDeferredGeometryMaterialShader(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name) = 0;
		virtual emberCommon::MaterialShaderId CreateForwardMaterialShader(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name) = 0;

		// Getters:
		virtual emberCommon::MaterialShaderId TryGetMaterialShaderId(const std::string& name) const = 0;
		virtual IMaterialShader* TryGetMaterialShader(emberCommon::MaterialShaderId materialShaderId) const = 0;
		virtual const std::string* TryGetMaterialShaderName(emberCommon::MaterialShaderId materialShaderId) const = 0;

		// Deleter:
		virtual void DeleteMaterialShader(emberCommon::MaterialShaderId materialShaderId) = 0;

		// Debugging:
		virtual void Print() const = 0;
	};
}