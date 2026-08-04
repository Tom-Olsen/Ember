#pragma once
#include "emberCoreExport.h"
#include "material.h"



namespace emberCore
{
	// Forward declarations:
	class ForwardMaterial;
	class MaterialManager;



	class EMBER_CORE_API ShadowMaterial : public Material
	{
		// Friends:
		friend class ForwardMaterial;
		friend class Material;
		friend class MaterialManager;

	private: // Methods:
		ShadowMaterial(emberBackendInterface::IMaterial* pIMaterial);

	public: // Methods:
		// Constructor/Destructor:
		ShadowMaterial();
		~ShadowMaterial();
	};
}