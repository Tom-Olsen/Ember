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
	class MaterialShader;



	class EMBER_CORE_API ForwardMaterial : public Material
	{
		// Friends:
		friend class MaterialManager;

	public: // Methods:
		// Constructor/Destructor:
		ForwardMaterial();
		~ForwardMaterial();

		// Copyable:
		ForwardMaterial(const ForwardMaterial&) = default;
		ForwardMaterial& operator=(const ForwardMaterial&) = default;

		// Movable:
		ForwardMaterial(ForwardMaterial&& other) noexcept = default;
		ForwardMaterial& operator=(ForwardMaterial&& other) noexcept = default;

		// Creation/Cloning:
		static ForwardMaterial CreateFromShader(emberCommon::ForwardRenderMode renderMode, const MaterialShader& materialShader, const std::string& name);
		ForwardMaterial Clone(const std::string& name) const;
		ForwardMaterial Clone(const std::string& name, emberCommon::ForwardRenderMode renderMode) const;
		ForwardMaterial CloneWithDefaultBindings(const std::string& name) const;
		ForwardMaterial CloneWithDefaultBindings(const std::string& name, emberCommon::ForwardRenderMode renderMode) const;

		// Getters:
		emberCommon::ForwardRenderMode GetRenderMode() const;
		const emberCommon::ForwardRenderState* GetRenderState() const;
		ShadowMaterial GetShadowMaterial() const;
		int32_t GetRenderQueue() const;
		bool GetIsTransparent() const;

		// Setters:
		void SetRenderMode(emberCommon::ForwardRenderMode renderMode);
		void SetCullMode(emberCommon::CullMode cullMode);
		void SetRenderQueue(int32_t renderQueue);
		void SetShadowMaterial(const ShadowMaterial& shadowMaterial);
		void ResetShadowMaterial();

	private: // Methods:
		ForwardMaterial(emberCommon::MaterialId materialId);
	};
}