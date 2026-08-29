#pragma once
#include "commonCullMode.h"
#include "commonGizmoRenderMode.h"
#include "commonGizmoRenderState.h"
#include "emberCoreExport.h"
#include "material.h"
#include <cstdint>



namespace emberCore
{
	// Forward declarations:
	class MaterialManager;
	class MaterialShader;



	class EMBER_CORE_API GizmoMaterial : public Material
	{
		// Friends:
		friend class MaterialManager;

	public: // Methods:
		// Constructor/Destructor:
		GizmoMaterial();
		~GizmoMaterial();

		// Copyable:
		GizmoMaterial(const GizmoMaterial&) = default;
		GizmoMaterial& operator=(const GizmoMaterial&) = default;

		// Movable:
		GizmoMaterial(GizmoMaterial&& other) noexcept = default;
		GizmoMaterial& operator=(GizmoMaterial&& other) noexcept = default;

		// Creation/Cloning:
		static GizmoMaterial CreateFromShader(emberCommon::GizmoRenderMode renderMode, const MaterialShader& materialShader, const std::string& name);
		GizmoMaterial Clone(const std::string& name) const;
		GizmoMaterial Clone(const std::string& name, emberCommon::GizmoRenderMode renderMode) const;

		// Getters:
		emberCommon::GizmoRenderMode GetRenderMode() const;
		const emberCommon::GizmoRenderState* GetRenderState() const;
		int32_t GetRenderQueue() const;
		bool GetIsTransparent() const;

		// Setters:
		void SetRenderMode(emberCommon::GizmoRenderMode renderMode);
		void SetCullMode(emberCommon::CullMode cullMode);
		void SetRenderQueue(int32_t renderQueue);

	private: // Methods:
		GizmoMaterial(emberCommon::MaterialId materialId);
	};
}