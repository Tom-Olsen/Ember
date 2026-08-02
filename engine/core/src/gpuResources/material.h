#pragma once
#include "commonMaterialRenderState.h"
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
	class EMBER_CORE_API Material : public Shader
	{
		// Friends:
		friend class MaterialManager;
		friend class Renderer;
		friend class ShaderProperties;

	private: // Members:
		emberBackendInterface::IMaterial* m_pIMaterial;

	private: // Methods:
		emberBackendInterface::IMaterial* const GetInterfaceHandle() const;
		Material(emberBackendInterface::IMaterial* pIMaterial);

	public: // Methods:
		// Constructor/Destructor:
		Material(); // for invalid materials only.
		~Material();

		// Creation/Destruction: (register/delete from MaterialManager)
		static Material CreateForward(emberCommon::RenderMode renderMode, const std::string& name, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv = "");
		static Material CreateShadow(const std::string& name, const std::filesystem::path& vertexSpv);
		void Destroy();

		// Copyable:
		Material(const Material&) = default;
		Material& operator=(const Material&) = default;

		// Movable:
		Material(Material&& other) = default;
		Material& operator=(Material&& other) = default;

		// Setters:
		void SetDefaultRenderState(const emberCommon::MaterialRenderState& defaultRenderState);
		void SetShadowMaterial(const Material& shadowMaterial);

		// Getters:
		const std::string& GetName() const;
		const emberCommon::MaterialRenderState& GetDefaultRenderState() const;
		Material GetShadowMaterial() const;     // throws on fail.
		Material TryGetShadowMaterial() const;  // returns invalid Material on fail.
		bool IsValid() const;

		// Debugging:
		void Print() const;
	};
}