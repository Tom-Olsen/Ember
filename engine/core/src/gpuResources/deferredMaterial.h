#pragma once
#include "commonCullMode.h"
#include "commonDeferredGeometryRenderState.h"
#include "emberCoreExport.h"
#include "material.h"
#include "shadowMaterial.h"
#include <cstdint>



namespace emberCore
{
	// Forward declarations:
	class MaterialManager;
	class MaterialShader;



	class EMBER_CORE_API DeferredMaterial : public Material
	{
		// Friends:
		friend class MaterialManager;

	public: // Methods:
		// Constructor/Destructor:
		DeferredMaterial();
		~DeferredMaterial();

		// Copyable:
		DeferredMaterial(const DeferredMaterial&) = default;
		DeferredMaterial& operator=(const DeferredMaterial&) = default;

		// Movable:
		DeferredMaterial(DeferredMaterial&& other) noexcept = default;
		DeferredMaterial& operator=(DeferredMaterial&& other) noexcept = default;

		// Creation/Cloning:
		static DeferredMaterial CreateFromShader(const MaterialShader& materialShader, const std::string& name);
		DeferredMaterial Clone(const std::string& name) const;

		// Getters:
		const emberCommon::DeferredGeometryRenderState* GetRenderState() const;
		ShadowMaterial GetShadowMaterial() const;
		int32_t GetRenderQueue() const;

		// Setters:
		void SetCullMode(emberCommon::CullMode cullMode);
		void SetRenderQueue(int32_t renderQueue);
		void SetShadowMaterial(const ShadowMaterial& shadowMaterial);
		void ClearShadowMaterial();

	private: // Methods:
		DeferredMaterial(MaterialId materialId);
	};
}