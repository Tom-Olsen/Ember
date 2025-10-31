#include "component.h"
#include "entity.h"
#include "scene.h"



namespace emberEngine
{
	// Public methods:
	// Constructor/Destructor:
	Component::Component()
	{
		m_isEnabled = true;
	}
	Component::~Component()
	{

	}



	// Setters:
	void Component::SetIsEnabled(bool isEnabled)
	{
		m_isEnabled = isEnabled;
	}



	// Getters:
	const std::string& Component::GetName() const
	{
		return m_name;
	}
	bool Component::GetIsEnabled() const
	{
		return m_isEnabled;
	}
	bool Component::GetIsActive() const
	{
		return m_isEnabled && GetEntity().GetIsActive();
	}
	Entity const Component::GetEntity() const
	{
		return Entity(m_entityHandle, m_pScene->GetEntityNamesMap()[m_entityHandle], m_pScene);
	}
	Transform* Component::GetTransform() const
	{
		return GetEntity().GetTransform();
	}
}