#include "entityHandle.h"
#include "world.h"



namespace emberEcs
{
	// Public Methods:
	// Access:
	bool EntityHandle::IsValid() const
	{
		return Get() != nullptr;
	}
	Entity* EntityHandle::Get() const
	{
		return m_runtimeId == 0 ? nullptr : World::ResolveEntity(m_entityId, m_runtimeId);
	}



	// Private Methods:
	// Constructor:
	EntityHandle::EntityHandle(EntityId entityId, uint64_t runtimeId)
		: m_entityId(entityId), m_runtimeId(runtimeId)
	{

	}
}