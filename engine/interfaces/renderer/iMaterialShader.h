#pragma once
#include "commonMaterialType.h"



namespace emberBackendInterface
{
	class IMaterialShader
	{
	public: // Methods:
		// Virtual destructor for v-table:
		virtual ~IMaterialShader() = default;

		// Getters:
		virtual emberCommon::MaterialType GetMaterialType() const = 0;
	};
}