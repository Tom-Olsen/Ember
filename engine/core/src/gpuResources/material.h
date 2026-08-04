#pragma once
#include "commonCullMode.h"
#include "commonForwardRenderMode.h"
#include "commonGizmoRenderMode.h"
#include "commonMaterialType.h"
#include "emberCoreExport.h"
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
		emberBackendInterface::IMaterial* m_pIMaterial;

	private: // Methods:
		emberBackendInterface::IMaterial* const GetInterfaceHandle() const;

	protected: // Methods:
		Material(emberBackendInterface::IMaterial* pIMaterial);

	public: // Methods:
		// Constructor/Destructor:
		Material(); // for invalid materials only.
		~Material();

		// Creation/Destruction: (register/delete from MaterialManager)
		static ForwardMaterial CreateForward(emberCommon::ForwardRenderMode renderMode, const std::string& name, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv = "");
		static GizmoMaterial CreateGizmo(emberCommon::GizmoRenderMode renderMode, const std::string& name, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv = "");
		static ShadowMaterial CreateShadow(const std::string& name, const std::filesystem::path& vertexSpv);
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
	};
}