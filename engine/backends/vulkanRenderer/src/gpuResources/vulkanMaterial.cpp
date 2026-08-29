#include "vulkanMaterial.h"
#include "descriptorSetMacros.h"
#include "vulkanDescriptorSetBinding.h"
#include <stdexcept>
#include <utility>



namespace vulkanRendererBackend
{
	// Public methods:
	// Factories:
	Material Material::CreateGizmo(MaterialShader* pMaterialShader, emberCommon::GizmoRenderMode renderMode, const std::string& debugName)
	{
		Material material(pMaterialShader, debugName);
		material.m_pGizmoRenderState = std::make_unique<emberCommon::GizmoRenderState>(emberCommon::GizmoRenderState::GizmoDefault(renderMode));
		return material;
	}
	Material Material::CreateOutline(MaterialShader* pMaterialShader, const std::string& debugName)
	{
		Material material(pMaterialShader, debugName);
		material.m_pOutlineRenderState = std::make_unique<emberCommon::OutlineRenderState>();
		return material;
	}
	Material Material::CreateShadow(MaterialShader* pMaterialShader, const std::string& debugName)
	{
		Material material(pMaterialShader, debugName);
		material.m_pShadowRenderState = std::make_unique<emberCommon::ShadowRenderState>();
		return material;
	}
	Material Material::CreateDeferredGeometry(MaterialShader* pMaterialShader, const std::string& debugName)
	{
		Material material(pMaterialShader, debugName);
		material.m_pDeferredGeometryRenderState = std::make_unique<emberCommon::DeferredGeometryRenderState>();
		return material;
	}
	Material Material::CreateDeferredLighting(MaterialShader* pMaterialShader, const std::string& debugName)
	{
		Material material(pMaterialShader, debugName);
		material.m_pDeferredLightingRenderState = std::make_unique<emberCommon::DeferredLightingRenderState>();
		return material;
	}
	Material Material::CreateForward(MaterialShader* pMaterialShader, emberCommon::ForwardRenderMode renderMode, const std::string& debugName)
	{
		Material material(pMaterialShader, debugName);
		material.m_pForwardRenderState = std::make_unique<emberCommon::ForwardRenderState>(emberCommon::ForwardRenderState::ForwardDefault(renderMode));
		return material;
	}
	Material Material::CreatePresent(MaterialShader* pMaterialShader, const std::string& debugName)
	{
		Material material(pMaterialShader, debugName);
		material.m_pPresentRenderState = std::make_unique<emberCommon::PresentRenderState>();
		return material;
	}



	// Cloning:
	Material Material::CloneGizmo(const Material& sourceMaterial, const std::string& debugName)
	{
		if (sourceMaterial.GetMaterialPass() != emberCommon::MaterialPass::gizmo)
			throw std::runtime_error("Material::CloneGizmo(...) failed. Source material is not a gizmo material.");

		std::unique_ptr<DescriptorSetBinding> pDescriptorSetBinding = std::make_unique<DescriptorSetBinding>(*sourceMaterial.m_pShaderDescriptorSetBinding, debugName);
		Material material(sourceMaterial.GetMaterialShader(), std::move(pDescriptorSetBinding), debugName);
		material.m_pGizmoRenderState = std::make_unique<emberCommon::GizmoRenderState>(*sourceMaterial.m_pGizmoRenderState);
		return material;
	}
	Material Material::CloneOutline(const Material& sourceMaterial, const std::string& debugName)
	{
		if (sourceMaterial.GetMaterialPass() != emberCommon::MaterialPass::outline)
			throw std::runtime_error("Material::CloneOutline(...) failed. Source material is not an outline material.");

		std::unique_ptr<DescriptorSetBinding> pDescriptorSetBinding = std::make_unique<DescriptorSetBinding>(*sourceMaterial.m_pShaderDescriptorSetBinding, debugName);
		Material material(sourceMaterial.GetMaterialShader(), std::move(pDescriptorSetBinding), debugName);
		material.m_pOutlineRenderState = std::make_unique<emberCommon::OutlineRenderState>(*sourceMaterial.m_pOutlineRenderState);
		return material;
	}
	Material Material::CloneShadow(const Material& sourceMaterial, const std::string& debugName)
	{
		if (sourceMaterial.GetMaterialPass() != emberCommon::MaterialPass::shadow)
			throw std::runtime_error("Material::CloneShadow(...) failed. Source material is not a shadow material.");

		std::unique_ptr<DescriptorSetBinding> pDescriptorSetBinding = std::make_unique<DescriptorSetBinding>(*sourceMaterial.m_pShaderDescriptorSetBinding, debugName);
		Material material(sourceMaterial.GetMaterialShader(), std::move(pDescriptorSetBinding), debugName);
		material.m_pShadowRenderState = std::make_unique<emberCommon::ShadowRenderState>(*sourceMaterial.m_pShadowRenderState);
		return material;
	}
	Material Material::CloneDeferredGeometry(const Material& sourceMaterial, const std::string& debugName)
	{
		if (sourceMaterial.GetMaterialPass() != emberCommon::MaterialPass::deferredGeometry)
			throw std::runtime_error("Material::CloneDeferredGeometry(...) failed. Source material is not a deferred geometry material.");

		std::unique_ptr<DescriptorSetBinding> pDescriptorSetBinding = std::make_unique<DescriptorSetBinding>(*sourceMaterial.m_pShaderDescriptorSetBinding, debugName);
		Material material(sourceMaterial.GetMaterialShader(), std::move(pDescriptorSetBinding), debugName);
		material.m_pDeferredGeometryRenderState = std::make_unique<emberCommon::DeferredGeometryRenderState>(*sourceMaterial.m_pDeferredGeometryRenderState);
		return material;
	}
	Material Material::CloneDeferredLighting(const Material& sourceMaterial, const std::string& debugName)
	{
		if (sourceMaterial.GetMaterialPass() != emberCommon::MaterialPass::deferredLighting)
			throw std::runtime_error("Material::CloneDeferredLighting(...) failed. Source material is not a deferred lighting material.");

		std::unique_ptr<DescriptorSetBinding> pDescriptorSetBinding = std::make_unique<DescriptorSetBinding>(*sourceMaterial.m_pShaderDescriptorSetBinding, debugName);
		Material material(sourceMaterial.GetMaterialShader(), std::move(pDescriptorSetBinding), debugName);
		material.m_pDeferredLightingRenderState = std::make_unique<emberCommon::DeferredLightingRenderState>(*sourceMaterial.m_pDeferredLightingRenderState);
		return material;
	}
	Material Material::CloneForward(const Material& sourceMaterial, const std::string& debugName)
	{
		if (sourceMaterial.GetMaterialPass() != emberCommon::MaterialPass::forward)
			throw std::runtime_error("Material::CloneForward(...) failed. Source material is not a forward material.");

		std::unique_ptr<DescriptorSetBinding> pDescriptorSetBinding = std::make_unique<DescriptorSetBinding>(*sourceMaterial.m_pShaderDescriptorSetBinding, debugName);
		Material material(sourceMaterial.GetMaterialShader(), std::move(pDescriptorSetBinding), debugName);
		material.m_pForwardRenderState = std::make_unique<emberCommon::ForwardRenderState>(*sourceMaterial.m_pForwardRenderState);
		return material;
	}
	Material Material::ClonePresent(const Material& sourceMaterial, const std::string& debugName)
	{
		if (sourceMaterial.GetMaterialPass() != emberCommon::MaterialPass::present)
			throw std::runtime_error("Material::ClonePresent(...) failed. Source material is not a present material.");

		std::unique_ptr<DescriptorSetBinding> pDescriptorSetBinding = std::make_unique<DescriptorSetBinding>(*sourceMaterial.m_pShaderDescriptorSetBinding, debugName);
		Material material(sourceMaterial.GetMaterialShader(), std::move(pDescriptorSetBinding), debugName);
		material.m_pPresentRenderState = std::make_unique<emberCommon::PresentRenderState>(*sourceMaterial.m_pPresentRenderState);
		return material;
	}



	// Destructor:
	Material::~Material()
	{

	}



	// Movable:
	Material::Material(Material&& other) noexcept = default;
	Material& Material::operator=(Material&& other) noexcept = default;



	// Setters:
	void Material::SetRenderQueue(int32_t renderQueue)
	{
		switch (GetMaterialPass())
		{
			case emberCommon::MaterialPass::gizmo:
				m_pGizmoRenderState->renderQueue = renderQueue;
				return;
			case emberCommon::MaterialPass::deferredGeometry:
				m_pDeferredGeometryRenderState->renderQueue = renderQueue;
				return;
			case emberCommon::MaterialPass::forward:
				m_pForwardRenderState->renderQueue = renderQueue;
				return;
			default:
				throw std::runtime_error("Material::SetRenderQueue(...) failed. Render queue is not dynamic for this material pass.");
		}
	}
	void Material::SetCullMode(emberCommon::CullMode cullMode)
	{
		switch (GetMaterialPass())
		{
			case emberCommon::MaterialPass::gizmo:
				m_pGizmoRenderState->cullMode = cullMode;
				return;
			case emberCommon::MaterialPass::deferredGeometry:
				m_pDeferredGeometryRenderState->cullMode = cullMode;
				return;
			case emberCommon::MaterialPass::forward:
				m_pForwardRenderState->cullMode = cullMode;
				return;
			default:
				throw std::runtime_error("Material::SetCullMode(...) failed. Cull mode is not dynamic for this material pass.");
		}
	}
	void Material::SetGizmoRenderMode(emberCommon::GizmoRenderMode renderMode)
	{
		if (GetMaterialPass() != emberCommon::MaterialPass::gizmo)
			throw std::runtime_error("Material::SetGizmoRenderMode(...) failed. Material is not a gizmo material.");
		m_pGizmoRenderState = std::make_unique<emberCommon::GizmoRenderState>(emberCommon::GizmoRenderState::GizmoDefault(renderMode));
	}
	void Material::SetForwardRenderMode(emberCommon::ForwardRenderMode renderMode)
	{
		if (GetMaterialPass() != emberCommon::MaterialPass::forward)
			throw std::runtime_error("Material::SetForwardRenderMode(...) failed. Material is not a forward material.");
		m_pForwardRenderState = std::make_unique<emberCommon::ForwardRenderState>(emberCommon::ForwardRenderState::ForwardDefault(renderMode));
	}



	// Getters:
	emberCommon::MaterialPass Material::GetMaterialPass() const
	{
		return GetMaterialShader()->GetMaterialPass();
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
		switch (GetMaterialPass())
		{
			case emberCommon::MaterialPass::gizmo:
				return m_pGizmoRenderState->renderQueue;
			case emberCommon::MaterialPass::deferredGeometry:
				return m_pDeferredGeometryRenderState->renderQueue;
			case emberCommon::MaterialPass::forward:
				return m_pForwardRenderState->renderQueue;
			default:
				return 0;
		}
	}
	emberCommon::CullMode Material::GetCullMode() const
	{
		switch (GetMaterialPass())
		{
			case emberCommon::MaterialPass::gizmo:
				return m_pGizmoRenderState->cullMode;
			case emberCommon::MaterialPass::outline:
				return m_pOutlineRenderState->cullMode;
			case emberCommon::MaterialPass::shadow:
				return m_pShadowRenderState->cullMode;
			case emberCommon::MaterialPass::deferredGeometry:
				return m_pDeferredGeometryRenderState->cullMode;
			case emberCommon::MaterialPass::deferredLighting:
				return m_pDeferredLightingRenderState->cullMode;
			case emberCommon::MaterialPass::forward:
				return m_pForwardRenderState->cullMode;
			case emberCommon::MaterialPass::present:
				return m_pPresentRenderState->cullMode;
			default:
				throw std::runtime_error("Material::GetCullMode(...) failed. Unsupported material pass.");
		}
	}
	bool Material::IsTransparent() const
	{
		switch (GetMaterialPass())
		{
			case emberCommon::MaterialPass::forward:
				return m_pForwardRenderState->renderMode == emberCommon::ForwardRenderMode::transparent;
			case emberCommon::MaterialPass::gizmo:
				return m_pGizmoRenderState->renderMode == emberCommon::GizmoRenderMode::transparent;
			default:
				return false;
		}
	}
	emberCommon::GizmoRenderMode Material::GetGizmoRenderMode() const
	{
		if (GetMaterialPass() != emberCommon::MaterialPass::gizmo)
			throw std::runtime_error("Material::GetGizmoRenderMode(...) failed. Material is not a gizmo material.");
		return m_pGizmoRenderState->renderMode;
	}
	emberCommon::ForwardRenderMode Material::GetForwardRenderMode() const
	{
		if (GetMaterialPass() != emberCommon::MaterialPass::forward)
			throw std::runtime_error("Material::GetForwardRenderMode(...) failed. Material is not a forward material.");
		return m_pForwardRenderState->renderMode;
	}
	const emberCommon::GizmoRenderState& Material::GetGizmoRenderState() const
	{
		if (GetMaterialPass() != emberCommon::MaterialPass::gizmo)
			throw std::runtime_error("Material::GetGizmoRenderState(...) failed. Material is not a gizmo material.");
		return *m_pGizmoRenderState;
	}
	const emberCommon::DeferredGeometryRenderState& Material::GetDeferredGeometryRenderState() const
	{
		if (GetMaterialPass() != emberCommon::MaterialPass::deferredGeometry)
			throw std::runtime_error("Material::GetDeferredGeometryRenderState(...) failed. Material is not a deferred geometry material.");
		return *m_pDeferredGeometryRenderState;
	}
	const emberCommon::DeferredLightingRenderState& Material::GetDeferredLightingRenderState() const
	{
		if (GetMaterialPass() != emberCommon::MaterialPass::deferredLighting)
			throw std::runtime_error("Material::GetDeferredLightingRenderState(...) failed. Material is not a deferred lighting material.");
		return *m_pDeferredLightingRenderState;
	}
	const emberCommon::ForwardRenderState& Material::GetForwardRenderState() const
	{
		if (GetMaterialPass() != emberCommon::MaterialPass::forward)
			throw std::runtime_error("Material::GetForwardRenderState(...) failed. Material is not a forward material.");
		return *m_pForwardRenderState;
	}
	Shader* Material::GetShader() const
	{
		return m_materialShaderHandle.Get();
	}
	MaterialShader* Material::GetMaterialShader() const
	{
		return static_cast<MaterialShader*>(m_materialShaderHandle.Get());
	}
	const VkPipelineLayout& Material::GetVkPipelineLayout() const
	{
		return GetMaterialShader()->GetVkPipelineLayout();
	}
	const std::string& Material::GetDebugName() const
	{
		return m_debugName;
	}



	// Debugging:
	void Material::Print() const
	{
		GetMaterialShader()->PrintShaderInfo();
	}



	// Private methods:
	// Constructor:
	Material::Material(MaterialShader* pMaterialShader, const std::string& debugName)
		: m_debugName(debugName)
	{
		if (pMaterialShader == nullptr)
			throw std::runtime_error("Material::Material(...) failed. MaterialShader is null.");
		m_materialShaderHandle = ShaderHandle(*pMaterialShader);
		m_pShaderDescriptorSetBinding = std::make_unique<DescriptorSetBinding>(static_cast<Shader*>(pMaterialShader), SHADER_SET_INDEX, debugName);
	}
	Material::Material(MaterialShader* pMaterialShader, std::unique_ptr<DescriptorSetBinding> pShaderDescriptorSetBinding, const std::string& debugName)
		: m_debugName(debugName)
		, m_pShaderDescriptorSetBinding(std::move(pShaderDescriptorSetBinding))
	{
		if (pMaterialShader == nullptr)
			throw std::runtime_error("Material::Material(...) failed. MaterialShader is null.");
		if (m_pShaderDescriptorSetBinding == nullptr)
			throw std::runtime_error("Material::Material(...) failed. DescriptorSetBinding is null.");
		m_materialShaderHandle = ShaderHandle(*pMaterialShader);
	}
}