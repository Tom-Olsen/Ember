#pragma once
#include "commonCullMode.h"
#include "commonForwardRenderMode.h"
#include "commonGizmoRenderMode.h"
#include "commonMaterialPass.h"
#include "emberCoreExport.h"
#include "commonMaterialId.h"
#include "shader.h"
#include <filesystem>
#include <string>



// Forward decleration:
namespace emberBackendInterface
{
	class IMaterial;
}



namespace emberCore
{
	// Forward declarations:
	class GizmoMaterial;
	class ShadowMaterial;
	class DeferredMaterial;
	class ForwardMaterial;
	class MaterialShader;



	class EMBER_CORE_API Material : public Shader
	{
		// Friends:
		friend class GizmoMaterial;
		friend class ShadowMaterial;
		friend class DeferredMaterial;
		friend class ForwardMaterial;
		friend class MaterialManager;
		friend class Renderer;
		friend class ShaderProperties;

	private: // Members:
		emberCommon::MaterialId m_materialId;

	protected: // Methods:
		Material(emberCommon::MaterialId materialId);
		emberBackendInterface::IMaterial* GetMutableInterfaceHandle() const;
		emberBackendInterface::IDescriptorSetBinding* TryGetShaderDescriptorSetBinding() const override;

	public: // Methods:
		// Constructor/Destructor:
		Material(); // for invalid materials only.
		~Material();

		// Creation/Destruction:
		static GizmoMaterial CreateGizmo(emberCommon::GizmoRenderMode renderMode, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name);
		static GizmoMaterial CreateGizmo(emberCommon::GizmoRenderMode renderMode, const MaterialShader& materialShader, const std::string& name);
		static ShadowMaterial CreateShadow(const std::filesystem::path& vertexSpv, const std::string& name);
		static ShadowMaterial CreateShadow(const MaterialShader& materialShader, const std::string& name);
		static DeferredMaterial CreateDeferredGeometry(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name);
		static DeferredMaterial CreateDeferredGeometry(const MaterialShader& materialShader, const std::string& name);
		static ForwardMaterial CreateForward(emberCommon::ForwardRenderMode renderMode, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name);
		static ForwardMaterial CreateForward(emberCommon::ForwardRenderMode renderMode, const MaterialShader& materialShader, const std::string& name);
		void Destroy();

		// Copyable:
		Material(const Material&) = default;
		Material& operator=(const Material&) = default;

		// Movable:
		Material(Material&& other) = default;
		Material& operator=(Material&& other) = default;

		// Getters:
		const std::string& GetName() const;
		emberCommon::MaterialPass GetMaterialPass() const;
		emberCommon::CullMode GetCullMode() const;
		bool IsValid() const;

		// Debugging:
		void Print() const;

	private: // Methods:
		emberBackendInterface::IMaterial* GetInterfaceHandle() const;
	};
}