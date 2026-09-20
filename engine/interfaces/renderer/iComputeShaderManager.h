#pragma once
#include "commonComputeShaderId.h"
#include <string>



// Forward declarations:
namespace emberAssetLoader
{
	struct ComputeShaderAsset;
}



namespace emberBackendInterface
{
	// Forward declarations:
	class IComputeShader;



	class IComputeShaderManager
	{
	public: // Methods:
		// Virtual destructor for v-table:
		virtual ~IComputeShaderManager() = default;

		// Asset creation/initialization:
		virtual emberCommon::ComputeShaderId CreateComputeShader(const emberAssetLoader::ComputeShaderAsset& computeShaderAsset) = 0;
		virtual void InitializeDefaultComputeShaders() = 0;

		// Getters:
		virtual emberCommon::ComputeShaderId TryGetComputeShaderId(const std::string& name) const = 0;
		virtual IComputeShader* TryGetComputeShader(emberCommon::ComputeShaderId computeShaderId) const = 0;
		virtual const std::string* TryGetComputeShaderName(emberCommon::ComputeShaderId computeShaderId) const = 0;
		virtual bool IsComputeShaderMutable(emberCommon::ComputeShaderId computeShaderId) const = 0;

		// Debugging:
		virtual void Print() const = 0;
	};
}