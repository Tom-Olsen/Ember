#include "component.h"
#include "entity.h"



namespace emberEcs
{
	// Public Methods:
	// Constructor/Destructor:
	Component::Component() : m_isEnabled(true)
	{

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
		const Entity* pEntity = m_entity.Get();
		return m_isEnabled && pEntity != nullptr && pEntity->GetIsActive();
	}
	EntityHandle Component::GetEntity() const
	{
		return m_entity;
	}
	Transform* Component::GetTransform() const
	{
		const Entity* pEntity = m_entity.Get();
		return pEntity != nullptr ? pEntity->GetTransform() : nullptr;
	}
}