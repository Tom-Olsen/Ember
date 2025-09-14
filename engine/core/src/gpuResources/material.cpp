#include "material.h"
#include "vulkanMaterial.h"



namespace emberEngine
{
	// Public methods:
	// Constructor/Destructor:
	Material::Material(MaterialType type, const std::string& name, uint32_t renderQueue, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv)
	{
		m_pIMaterial = std::make_unique<vulkanRendererBackend::Material>(type, name, renderQueue, vertexSpv, fragmentSpv)
	}
	Material::~Material()
	{
		m_pIMaterial.release();
	}



	// Getters:
	MaterialType Material::GetType() const
	{
		return m_pIMaterial->GetType();
	}
	uint32_t Material::GetRenderQueue() const
	{
		return m_pIMaterial->GetRenderQueue();
	}
}