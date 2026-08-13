#include "materialShader.h"
#include "iMaterialShader.h"
#include "renderer.h"
#include <utility>



namespace emberCore
{
	// Public methods:
	// Constructor/Destructor:
	MaterialShader::MaterialShader()
	{
		m_name = "";
		m_materialType = emberCommon::MaterialType::count;
		m_pIMaterialShader = nullptr;
	}
	MaterialShader::MaterialShader(emberCommon::MaterialType materialType, emberBackendInterface::IMaterialShader* pIMaterialShader, const std::string& name)
	{
		m_name = name;
		m_materialType = materialType;
		m_pIMaterialShader = pIMaterialShader;
	}
	MaterialShader::~MaterialShader()
	{
		if (m_pIMaterialShader != nullptr)
			Renderer::DestroyMaterialShader(m_pIMaterialShader);
	}



	// Creation:
	MaterialShader MaterialShader::CreateOutline(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name)
	{
		emberBackendInterface::IMaterialShader* iMaterialShader = Renderer::CreateOutlineMaterialShader(vertexSpv, fragmentSpv, name);
		return MaterialShader(emberCommon::MaterialType::outline, iMaterialShader, name);
	}
	MaterialShader MaterialShader::CreateForward(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name)
	{
		emberBackendInterface::IMaterialShader* iMaterialShader = Renderer::CreateForwardMaterialShader(vertexSpv, fragmentSpv, name);
		return MaterialShader(emberCommon::MaterialType::forward, iMaterialShader, name);
	}
	MaterialShader MaterialShader::CreateGizmo(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name)
	{
		emberBackendInterface::IMaterialShader* iMaterialShader = Renderer::CreateGizmoMaterialShader(vertexSpv, fragmentSpv, name);
		return MaterialShader(emberCommon::MaterialType::gizmo, iMaterialShader, name);
	}
	MaterialShader MaterialShader::CreateShadow(const std::filesystem::path& vertexSpv, const std::string& name)
	{
		emberBackendInterface::IMaterialShader* iMaterialShader = Renderer::CreateShadowMaterialShader(vertexSpv, name);
		return MaterialShader(emberCommon::MaterialType::shadow, iMaterialShader, name);
	}
	MaterialShader MaterialShader::CreatePresent(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name)
	{
		emberBackendInterface::IMaterialShader* iMaterialShader = Renderer::CreatePresentMaterialShader(vertexSpv, fragmentSpv, name);
		return MaterialShader(emberCommon::MaterialType::present, iMaterialShader, name);
	}



	// Movable:
	MaterialShader::MaterialShader(MaterialShader&& other) noexcept
	{
		m_name = std::move(other.m_name);
		m_materialType = other.m_materialType;
		m_pIMaterialShader = other.m_pIMaterialShader;
		other.m_materialType = emberCommon::MaterialType::count;
		other.m_pIMaterialShader = nullptr;
	}
	MaterialShader& MaterialShader::operator=(MaterialShader&& other) noexcept
	{
		if (this != &other)
		{
			if (m_pIMaterialShader != nullptr)
				Renderer::DestroyMaterialShader(m_pIMaterialShader);

			m_name = std::move(other.m_name);
			m_materialType = other.m_materialType;
			m_pIMaterialShader = other.m_pIMaterialShader;
			other.m_materialType = emberCommon::MaterialType::count;
			other.m_pIMaterialShader = nullptr;
		}
		return *this;
	}



	// Getters:
	const std::string& MaterialShader::GetName() const
	{
		return m_name;
	}
	emberCommon::MaterialType MaterialShader::GetMaterialType() const
	{
		return m_materialType;
	}
	bool MaterialShader::IsValid() const
	{
		return m_pIMaterialShader != nullptr;
	}



	// Private methods:
	emberBackendInterface::IMaterialShader* MaterialShader::GetInterfaceHandle() const
	{
		return m_pIMaterialShader;
	}
}