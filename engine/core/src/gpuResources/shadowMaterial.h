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

		// Copyable:
		ShadowMaterial(const ShadowMaterial&) = default;
		ShadowMaterial& operator=(const ShadowMaterial&) = default;

		// Movable:
		ShadowMaterial(ShadowMaterial&& other) noexcept = default;
		ShadowMaterial& operator=(ShadowMaterial&& other) noexcept = default;

		// Cloning:
		ShadowMaterial Clone(const std::string& name) const;
	};
}