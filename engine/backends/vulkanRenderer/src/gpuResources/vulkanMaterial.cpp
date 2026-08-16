#include "vulkanMaterial.h"
#include "descriptorSetMacros.h"
#include "vulkanDescriptorSetBinding.h"
#include <stdexcept>
#include <utility>



namespace vulkanRendererBackend
{
	// Public methods:
	// Factories/Destructor:
	Material Material::CreateOutline(MaterialShader* pMaterialShader, const std::string& debugName)
	{
		Material material(emberCommon::MaterialType::outline, pMaterialShader, debugName);
		material.m_pOutlineRenderState = std::make_unique<emberCommon::OutlineRenderState>();
		return material;
	}
	Material Material::CreateForward(MaterialShader* pMaterialShader, emberCommon::ForwardRenderMode renderMode, const std::string& debugName)
	{
		Material material(emberCommon::MaterialType::forward, pMaterialShader, debugName);
		material.m_pForwardRenderState = std::make_unique<emberCommon::ForwardRenderState>(emberCommon::ForwardRenderState::ForwardDefault(renderMode));
		return material;
	}
	Material Material::CreateGizmo(MaterialShader* pMaterialShader, emberCommon::GizmoRenderMode renderMode, const std::string& debugName)
	{
		Material material(emberCommon::MaterialType::gizmo, pMaterialShader, debugName);
		material.m_pGizmoRenderState = std::make_unique<emberCommon::GizmoRenderState>(emberCommon::GizmoRenderState::GizmoDefault(renderMode));
		return material;
	}
	Material Material::CreateShadow(MaterialShader* pMaterialShader, const std::string& debugName)
	{
		Material material(emberCommon::MaterialType::shadow, pMaterialShader, debugName);
		material.m_pShadowRenderState = std::make_unique<emberCommon::ShadowRenderState>();
		return material;
	}
	Material Material::CreatePresent(MaterialShader* pMaterialShader, const std::string& debugName)
	{
		Material material(emberCommon::MaterialType::present, pMaterialShader, debugName);
		material.m_pPresentRenderState = std::make_unique<emberCommon::PresentRenderState>();
		return material;
	}
	Material Material::CloneForward(const Material& sourceMaterial, const std::string& debugName)
	{
		if (sourceMaterial.GetMaterialType() != emberCommon::MaterialType::forward)
			throw std::runtime_error("Material::CloneForward(...) failed. Source material is not a forward material.");

		std::unique_ptr<DescriptorSetBinding> pDescriptorSetBinding = std::make_unique<DescriptorSetBinding>(*sourceMaterial.m_pShaderDescriptorSetBinding, debugName);
		Material material(emberCommon::MaterialType::forward, sourceMaterial.m_pMaterialShader, std::move(pDescriptorSetBinding), debugName);
		material.m_pForwardRenderState = std::make_unique<emberCommon::ForwardRenderState>(*sourceMaterial.m_pForwardRenderState);
		return material;
	}
	Material Material::CloneGizmo(const Material& sourceMaterial, const std::string& debugName)
	{
		if (sourceMaterial.GetMaterialType() != emberCommon::MaterialType::gizmo)
			throw std::runtime_error("Material::CloneGizmo(...) failed. Source material is not a gizmo material.");

		std::unique_ptr<DescriptorSetBinding> pDescriptorSetBinding = std::make_unique<DescriptorSetBinding>(*sourceMaterial.m_pShaderDescriptorSetBinding, debugName);
		Material material(emberCommon::MaterialType::gizmo, sourceMaterial.m_pMaterialShader, std::move(pDescriptorSetBinding), debugName);
		material.m_pGizmoRenderState = std::make_unique<emberCommon::GizmoRenderState>(*sourceMaterial.m_pGizmoRenderState);
		return material;
	}
	Material Material::CloneShadow(const Material& sourceMaterial, const std::string& debugName)
	{
		if (sourceMaterial.GetMaterialType() != emberCommon::MaterialType::shadow)
			throw std::runtime_error("Material::CloneShadow(...) failed. Source material is not a shadow material.");

		std::unique_ptr<DescriptorSetBinding> pDescriptorSetBinding = std::make_unique<DescriptorSetBinding>(*sourceMaterial.m_pShaderDescriptorSetBinding, debugName);
		Material material(emberCommon::MaterialType::shadow, sourceMaterial.m_pMaterialShader, std::move(pDescriptorSetBinding), debugName);
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
	emberCommon::MaterialType Material::GetMaterialType() const
	{
		return m_materialType;
	}
	emberBackendInterface::IDescriptorSetBinding* Material::GetShaderDescriptorSetBinding() const
	{
		return static_cast<emberBackendInterface::IDescriptorSetBinding*>(m_pShaderDescriptorSetBinding.get());
	}
	DescriptorSetBinding* Material::GetDescriptorSetBinding() const
	{
		return m_pShaderDescriptorSetBinding.get();
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
		return static_cast<Shader*>(m_pMaterialShader);
	}
	MaterialShader* Material::GetMaterialShader() const
	{
		return m_pMaterialShader;
	}
	const VkPipelineLayout& Material::GetVkPipelineLayout() const
	{
		return m_pMaterialShader->GetVkPipelineLayout();
	}
	const std::string& Material::GetDebugName() const
	{
		return m_debugName;
	}



	// Debugging:
	void Material::Print() const
	{
		m_pMaterialShader->PrintShaderInfo();
	}



	// Private methods:
	// Constructor:
	Material::Material(emberCommon::MaterialType materialType, MaterialShader* pMaterialShader, const std::string& debugName)
		: m_debugName(debugName)
		, m_materialType(materialType)
		, m_pMaterialShader(pMaterialShader)
	{
		if (m_pMaterialShader == nullptr)
			throw std::runtime_error("Material::Material(...) failed. MaterialShader is null.");
		m_pShaderDescriptorSetBinding = std::make_unique<DescriptorSetBinding>(static_cast<Shader*>(m_pMaterialShader), SHADER_SET_INDEX, debugName);
	}
	Material::Material(emberCommon::MaterialType materialType, MaterialShader* pMaterialShader, std::unique_ptr<DescriptorSetBinding> pShaderDescriptorSetBinding, const std::string& debugName)
		: m_debugName(debugName)
		, m_materialType(materialType)
		, m_pMaterialShader(pMaterialShader)
		, m_pShaderDescriptorSetBinding(std::move(pShaderDescriptorSetBinding))
	{
		if (m_pMaterialShader == nullptr)
			throw std::runtime_error("Material::Material(...) failed. MaterialShader is null.");
		if (m_pShaderDescriptorSetBinding == nullptr)
			throw std::runtime_error("Material::Material(...) failed. DescriptorSetBinding is null.");
	}
}