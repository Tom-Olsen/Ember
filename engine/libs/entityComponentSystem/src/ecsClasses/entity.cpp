#include "entity.h"
#include "entity.inl"
#include "transform.h"
#include "world.h"



namespace emberEcs
{
	// Public Methods:
	// Setters:
	void Entity::SetName(const std::string& name)
	{
		m_name = name;
	}
	void Entity::SetIsEnabled(bool isEnabled)
	{
		m_isEnabled = isEnabled;
	}



	// Getters:
	EntityId Entity::GetId() const
	{
		return m_entityId;
	}
	const std::string& Entity::GetName() const
	{
		return m_name;
	}
	bool Entity::GetIsEnabled() const
	{
		return m_isEnabled;
	}
	bool Entity::GetIsActive() const
	{
		return m_isEnabled;
	}
	Transform* Entity::GetTransform() const
	{
		return GetComponent<Transform>();
	}



	// Entity management:
	void Entity::Destroy()
	{
		World::DestroyEntity(m_entityId);
	}



	// Private Methods:
	// Constructor/Destructor:
	Entity::Entity(EntityId entityId, uint64_t runtimeId, entt::entity registryEntry, const std::string& name)
		: m_entityId(entityId), m_runtimeId(runtimeId), m_registryEntry(registryEntry), m_name(name), m_isEnabled(true)
	{

	}
	Entity::~Entity() = default;
}