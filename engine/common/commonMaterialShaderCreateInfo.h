#pragma once
#include "commonMaterialPass.h"
#include <filesystem>
#include <string>



namespace emberCommon
{
	struct MaterialShaderCreateInfo
	{
		std::filesystem::path vertexBinaryPath;
		std::filesystem::path fragmentBinaryPath;
		MaterialPass materialPass;
		std::string name;

		// Equality operator overloading:
		inline bool operator==(const MaterialShaderCreateInfo& other) const
		{
			return vertexBinaryPath == other.vertexBinaryPath
				&& fragmentBinaryPath == other.fragmentBinaryPath
				&& materialPass == other.materialPass
				&& name == other.name;
		}
	};
}