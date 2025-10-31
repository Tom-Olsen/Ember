#include "entityState.h"



namespace emberEngine
{
	// Public methods:
	// Constructor/Destructor:
	EntityState::EntityState(bool isEnabled)
	{
		m_isEnabled = isEnabled;
	}
	EntityState::~EntityState()
	{

	}



	// Getters/Setters:
	void EntityState::SetIsEnabled(bool isEnabled)
	{
		m_isEnabled = isEnabled;
	}
	bool EntityState::GetIsEnabled() const
	{
		return m_isEnabled;
	}
}