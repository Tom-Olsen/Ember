#pragma once
#include "commonMaterialPass.h"



namespace emberBackendInterface
{
	class IMaterialShader
	{
	public: // Methods:
		// Virtual destructor for v-table:
		virtual ~IMaterialShader() = default;

		// Getters:
		virtual emberCommon::MaterialPass GetMaterialPass() const = 0;
	};
}