#pragma once
#include "commonCullMode.h"
#include "commonForwardRenderMode.h"
#include "commonGizmoRenderMode.h"
#include "commonMaterialType.h"
#include "emberCoreExport.h"
#include "materialId.h"
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
	class ForwardMaterial;
	class GizmoMaterial;
	class MaterialShader;
	class ShadowMaterial;



	class EMBER_CORE_API Material : public Shader
	{
		// Friends:
		friend class ForwardMaterial;
		friend class GizmoMaterial;
		friend class ShadowMaterial;
		friend class MaterialManager;
		friend class Renderer;
		friend class ShaderProperties;

	private: // Members:
		MaterialId m_materialId;

	protected: // Methods:
		Material(MaterialId materialId);
		emberBackendInterface::IDescriptorSetBinding* GetShaderDescriptorSetBinding() const override;

	public: // Methods:
		// Constructor/Destructor:
		Material(); // for invalid materials only.
		~Material();

		// Creation/Destruction: (register/delete from MaterialManager)
		static Material CreateOutline(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name);
		static Material CreateOutline(const MaterialShader& materialShader, const std::string& name);
		static ForwardMaterial CreateForward(emberCommon::ForwardRenderMode renderMode, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name);
		static ForwardMaterial CreateForward(emberCommon::ForwardRenderMode renderMode, const MaterialShader& materialShader, const std::string& name);
		static GizmoMaterial CreateGizmo(emberCommon::GizmoRenderMode renderMode, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name);
		static GizmoMaterial CreateGizmo(emberCommon::GizmoRenderMode renderMode, const MaterialShader& materialShader, const std::string& name);
		static ShadowMaterial CreateShadow(const std::filesystem::path& vertexSpv, const std::string& name);
		static ShadowMaterial CreateShadow(const MaterialShader& materialShader, const std::string& name);
		static Material CreatePresent(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name);
		static Material CreatePresent(const MaterialShader& materialShader, const std::string& name);
		void Destroy();

		// Copyable:
		Material(const Material&) = default;
		Material& operator=(const Material&) = default;

		// Movable:
		Material(Material&& other) = default;
		Material& operator=(Material&& other) = default;

		// Getters:
		const std::string& GetName() const;
		emberCommon::MaterialType GetMaterialType() const;
		emberCommon::CullMode GetCullMode() const;
		bool IsValid() const;

		// Debugging:
		void Print() const;

	private: // Methods:
		emberBackendInterface::IMaterial* GetInterfaceHandle() const;
	};
}