#include "material.h"
#include "vulkanMaterial.h"



namespace emberEngine
{
	// Private methods:
	emberBackendInterface::IMaterial* Material::GetInterfaceHandle()
	{
		return m_pIMaterial.get();
	}



	// Public methods:
	// Constructor/Destructor:
	Material::Material(emberCommon::MaterialType type, const std::string& name, uint32_t renderQueue, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv)
	{
		m_pIMaterial = std::make_unique<vulkanRendererBackend::Material>(type, name, renderQueue, vertexSpv, fragmentSpv);
	}
	Material::~Material()
	{
		m_pIMaterial.release();
	}



	// Movable:
	Material::Material(Material&& other) = default;
	Material& Material::operator=(Material&& other) = default;



	// Getters:
	const std::string& Material::GetName() const
	{
		return m_pIMaterial->GetName();
	}
	emberCommon::MaterialType Material::GetType() const
	{
		return m_pIMaterial->GetType();
	}
	uint32_t Material::GetRenderQueue() const
	{
		return m_pIMaterial->GetRenderQueue();
	}
}