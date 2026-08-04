#pragma once
#include "commonCullMode.h"
#include "commonForwardRenderMode.h"
#include "commonForwardRenderState.h"
#include "emberCoreExport.h"
#include "material.h"
#include "shadowMaterial.h"
#include <cstdint>



namespace emberCore
{
	// Forward declarations:
	class MaterialManager;



	class EMBER_CORE_API ForwardMaterial : public Material
	{
		// Friends:
		friend class MaterialManager;

	private: // Methods:
		ForwardMaterial(emberBackendInterface::IMaterial* pIMaterial);

	public: // Methods:
		// Constructor/Destructor:
		ForwardMaterial();
		~ForwardMaterial();

		// Getters:
		emberCommon::ForwardRenderMode GetRenderMode() const;
		const emberCommon::ForwardRenderState& GetRenderState() const;
		ShadowMaterial GetShadowMaterial() const;     // throws on fail.
		ShadowMaterial TryGetShadowMaterial() const;  // returns invalid ShadowMaterial on fail.
		int32_t GetRenderQueue() const;
		bool GetIsTransparent() const;

		// Setters:
		void SetRenderMode(emberCommon::ForwardRenderMode renderMode);
		void SetCullMode(emberCommon::CullMode cullMode);
		void SetRenderQueue(int32_t renderQueue);
		void SetShadowMaterial(const ShadowMaterial& shadowMaterial);
	};
}