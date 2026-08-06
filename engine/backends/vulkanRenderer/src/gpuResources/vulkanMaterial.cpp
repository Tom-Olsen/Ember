#include "vulkanMaterial.h"
#include "descriptorSetMacros.h"
#include "logger.h"
#include "vulkanDescriptorSetBinding.h"
#include <stdexcept>



namespace vulkanRendererBackend
{
	// Public methods:
	// Factories/Destructor:
	Material Material::CreateOutline(const std::string& name, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv)
	{
		Material material(name, emberCommon::MaterialType::outline, MaterialShader::CreateOutline(name, vertexSpv, fragmentSpv));
		material.m_pOutlineRenderState = std::make_unique<emberCommon::OutlineRenderState>();
		return material;
	}
	Material Material::CreateForward(const std::string& name, emberCommon::ForwardRenderMode renderMode, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv)
	{
		Material material(name, emberCommon::MaterialType::forward, MaterialShader::CreateForward(name, vertexSpv, fragmentSpv));
		material.m_pForwardRenderState = std::make_unique<emberCommon::ForwardRenderState>(emberCommon::ForwardRenderState::ForwardDefault(renderMode));
		return material;
	}
	Material Material::CreateGizmo(const std::string& name, emberCommon::GizmoRenderMode renderMode, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv)
	{
		Material material(name, emberCommon::MaterialType::gizmo, MaterialShader::CreateGizmo(name, vertexSpv, fragmentSpv));
		material.m_pGizmoRenderState = std::make_unique<emberCommon::GizmoRenderState>(emberCommon::GizmoRenderState::GizmoDefault(renderMode));
		return material;
	}
	Material Material::CreateShadow(const std::string& name, uint32_t shadowMapResolution, const std::filesystem::path& vertexSpv)
	{
		Material material(name, emberCommon::MaterialType::shadow, MaterialShader::CreateShadow(name, shadowMapResolution, vertexSpv));
		material.m_pShadowRenderState = std::make_unique<emberCommon::ShadowRenderState>();
		return material;
	}
	Material Material::CreatePresent(const std::string& name, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv)
	{
		Material material(name, emberCommon::MaterialType::present, MaterialShader::CreatePresent(name, vertexSpv, fragmentSpv));
		material.m_pPresentRenderState = std::make_unique<emberCommon::PresentRenderState>();
		return material;
	}
	Material Material::CloneForward(const std::string& name, const Material& sourceMaterial)
	{
		if (sourceMaterial.GetMaterialType() != emberCommon::MaterialType::forward)
			throw std::runtime_error("Material::CloneForward(...) failed. Source material is not a forward material.");

		Material material(name, emberCommon::MaterialType::forward, sourceMaterial.m_pMaterialShader);
		material.m_pForwardRenderState = std::make_unique<emberCommon::ForwardRenderState>(*sourceMaterial.m_pForwardRenderState);
		material.m_pShadowMaterial = sourceMaterial.m_pShadowMaterial;
		return material;
	}
	Material Material::CloneForward(const std::string& name, const Material& sourceMaterial, emberCommon::ForwardRenderMode renderMode)
	{
		if (sourceMaterial.GetMaterialType() != emberCommon::MaterialType::forward)
			throw std::runtime_error("Material::CloneForward(...) failed. Source material is not a forward material.");

		Material material(name, emberCommon::MaterialType::forward, sourceMaterial.m_pMaterialShader);
		material.m_pForwardRenderState = std::make_unique<emberCommon::ForwardRenderState>(emberCommon::ForwardRenderState::ForwardDefault(renderMode));
		material.m_pShadowMaterial = sourceMaterial.m_pShadowMaterial;
		return material;
	}
	Material Material::CloneGizmo(const std::string& name, const Material& sourceMaterial)
	{
		if (sourceMaterial.GetMaterialType() != emberCommon::MaterialType::gizmo)
			throw std::runtime_error("Material::CloneGizmo(...) failed. Source material is not a gizmo material.");

		Material material(name, emberCommon::MaterialType::gizmo, sourceMaterial.m_pMaterialShader);
		material.m_pGizmoRenderState = std::make_unique<emberCommon::GizmoRenderState>(*sourceMaterial.m_pGizmoRenderState);
		return material;
	}
	Material Material::CloneGizmo(const std::string& name, const Material& sourceMaterial, emberCommon::GizmoRenderMode renderMode)
	{
		if (sourceMaterial.GetMaterialType() != emberCommon::MaterialType::gizmo)
			throw std::runtime_error("Material::CloneGizmo(...) failed. Source material is not a gizmo material.");

		Material material(name, emberCommon::MaterialType::gizmo, sourceMaterial.m_pMaterialShader);
		material.m_pGizmoRenderState = std::make_unique<emberCommon::GizmoRenderState>(emberCommon::GizmoRenderState::GizmoDefault(renderMode));
		return material;
	}
	Material Material::CloneShadow(const std::string& name, const Material& sourceMaterial)
	{
		if (sourceMaterial.GetMaterialType() != emberCommon::MaterialType::shadow)
			throw std::runtime_error("Material::CloneShadow(...) failed. Source material is not a shadow material.");

		Material material(name, emberCommon::MaterialType::shadow, sourceMaterial.m_pMaterialShader);
		material.m_pShadowRenderState = std::make_unique<emberCommon::ShadowRenderState>(*sourceMaterial.m_pShadowRenderState);
		return material;
	}
	Material::~Material()
	{

	}



	// Movable:
	Material::Material(Material&& other) noexcept = default;
	Material& Material::operator=(Material&& other) noexcept = default;



	// Setters:
	void Material::SetShadowMaterial(emberBackendInterface::IMaterial* pShadowMaterial)
	{
		if (!m_pMaterialShader->HasPipeline(PipelineType::forward))
		{
			LOG_WARN("Material::SetShadowMaterial(...) ignored. Only forward materials can override their shadow material.");
			return;
		}
		if (pShadowMaterial == nullptr)
		{
			m_pShadowMaterial = nullptr;
			return;
		}

		Material* pVulkanShadowMaterial = static_cast<Material*>(pShadowMaterial);
		if (!pVulkanShadowMaterial->m_pMaterialShader->HasPipeline(PipelineType::shadow))
		{
			LOG_WARN("Material::SetShadowMaterial(...) ignored. '{}' is not a shadow material.", pVulkanShadowMaterial->GetName());
			return;
		}
		m_pShadowMaterial = pVulkanShadowMaterial;
	}
	void Material::SetRenderQueue(int32_t renderQueue)
	{
		switch (GetMaterialType())
		{
			case emberCommon::MaterialType::forward:
				m_pForwardRenderState->renderQueue = renderQueue;
				return;
			case emberCommon::MaterialType::gizmo:
				m_pGizmoRenderState->renderQueue = renderQueue;
				return;
			default:
				throw std::runtime_error("Material::SetRenderQueue(...) failed. Render queue is not dynamic for this material type.");
		}
	}
	void Material::SetCullMode(emberCommon::CullMode cullMode)
	{
		switch (GetMaterialType())
		{
			case emberCommon::MaterialType::forward:
				m_pForwardRenderState->cullMode = cullMode;
				return;
			case emberCommon::MaterialType::gizmo:
				m_pGizmoRenderState->cullMode = cullMode;
				return;
			default:
				throw std::runtime_error("Material::SetCullMode(...) failed. Cull mode is not dynamic for this material type.");
		}
	}
	void Material::SetForwardRenderMode(emberCommon::ForwardRenderMode renderMode)
	{
		if (GetMaterialType() != emberCommon::MaterialType::forward)
			throw std::runtime_error("Material::SetForwardRenderMode(...) failed. Material is not a forward material.");
		m_pForwardRenderState = std::make_unique<emberCommon::ForwardRenderState>(emberCommon::ForwardRenderState::ForwardDefault(renderMode));
	}
	void Material::SetGizmoRenderMode(emberCommon::GizmoRenderMode renderMode)
	{
		if (GetMaterialType() != emberCommon::MaterialType::gizmo)
			throw std::runtime_error("Material::SetGizmoRenderMode(...) failed. Material is not a gizmo material.");
		m_pGizmoRenderState = std::make_unique<emberCommon::GizmoRenderState>(emberCommon::GizmoRenderState::GizmoDefault(renderMode));
	}



	// Getters:
	const std::string& Material::GetName() const
	{
		return m_name;
	}
	emberCommon::MaterialType Material::GetMaterialType() const
	{
		return m_materialType;
	}
	Material* Material::GetShadowMaterial() const
	{
		return m_pShadowMaterial;
	}
	emberBackendInterface::IDescriptorSetBinding* Material::GetShaderDescriptorSetBinding() const
	{
		return GetDescriptorSetBinding();
	}
	int32_t Material::GetRenderQueue() const
	{
		switch (GetMaterialType())
		{
			case emberCommon::MaterialType::forward:
				return m_pForwardRenderState->renderQueue;
			case emberCommon::MaterialType::gizmo:
				return m_pGizmoRenderState->renderQueue;
			default:
				return 0;
		}
	}
	emberCommon::CullMode Material::GetCullMode() const
	{
		switch (GetMaterialType())
		{
			case emberCommon::MaterialType::forward:
				return m_pForwardRenderState->cullMode;
			case emberCommon::MaterialType::gizmo:
				return m_pGizmoRenderState->cullMode;
			case emberCommon::MaterialType::outline:
				return m_pOutlineRenderState->cullMode;
			case emberCommon::MaterialType::shadow:
				return m_pShadowRenderState->cullMode;
			case emberCommon::MaterialType::present:
				return m_pPresentRenderState->cullMode;
			default:
				throw std::runtime_error("Material::GetCullMode(...) failed. Unsupported material type.");
		}
	}
	bool Material::IsTransparent() const
	{
		switch (GetMaterialType())
		{
			case emberCommon::MaterialType::forward:
				return m_pForwardRenderState->renderMode == emberCommon::ForwardRenderMode::transparent;
			case emberCommon::MaterialType::gizmo:
				return m_pGizmoRenderState->renderMode == emberCommon::GizmoRenderMode::transparent;
			default:
				return false;
		}
	}
	emberCommon::ForwardRenderMode Material::GetForwardRenderMode() const
	{
		if (GetMaterialType() != emberCommon::MaterialType::forward)
			throw std::runtime_error("Material::GetForwardRenderMode(...) failed. Material is not a forward material.");
		return m_pForwardRenderState->renderMode;
	}
	emberCommon::GizmoRenderMode Material::GetGizmoRenderMode() const
	{
		if (GetMaterialType() != emberCommon::MaterialType::gizmo)
			throw std::runtime_error("Material::GetGizmoRenderMode(...) failed. Material is not a gizmo material.");
		return m_pGizmoRenderState->renderMode;
	}
	const emberCommon::ForwardRenderState& Material::GetForwardRenderState() const
	{
		if (GetMaterialType() != emberCommon::MaterialType::forward)
			throw std::runtime_error("Material::GetForwardRenderState(...) failed. Material is not a forward material.");
		return *m_pForwardRenderState;
	}
	const emberCommon::GizmoRenderState& Material::GetGizmoRenderState() const
	{
		if (GetMaterialType() != emberCommon::MaterialType::gizmo)
			throw std::runtime_error("Material::GetGizmoRenderState(...) failed. Material is not a gizmo material.");
		return *m_pGizmoRenderState;
	}
	Shader* Material::GetShader() const
	{
		return static_cast<Shader*>(m_pMaterialShader.get());
	}
	MaterialShader* Material::GetMaterialShader() const
	{
		return m_pMaterialShader.get();
	}
	const VkPipelineLayout& Material::GetVkPipelineLayout() const
	{
		return m_pMaterialShader->GetVkPipelineLayout();
	}
	DescriptorSetBinding* Material::GetDescriptorSetBinding() const
	{
		return m_pShaderDescriptorSetBinding.get();
	}



	// Debugging:
	void Material::Print() const
	{
		m_pMaterialShader->PrintShaderInfo();
	}



	// Private methods:
	// Constructor:
	Material::Material(const std::string& name, emberCommon::MaterialType materialType, std::shared_ptr<MaterialShader> pMaterialShader)
		: m_name(name)
		, m_materialType(materialType)
	{
		m_pMaterialShader = std::move(pMaterialShader);
		m_pShaderDescriptorSetBinding = std::make_unique<DescriptorSetBinding>(static_cast<Shader*>(m_pMaterialShader.get()), SHADER_SET_INDEX);
		m_pShadowMaterial = nullptr;
	}
}