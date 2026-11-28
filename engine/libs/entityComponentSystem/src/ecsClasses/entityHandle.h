#pragma once
#include "entityId.h"
#include <stdint.h>



namespace emberEcs
{
	// Forward declarations:
	class Entity;
	class World;



	class EntityHandle
	{
		// Friends:
		friend class Entity;
		friend class World;

	private: // Members:
		EntityId m_entityId = 0;
		uint64_t m_runtimeId = 0;

	public: // Methods:
		// Constructor:
		EntityHandle() = default;

		// Access:
		bool IsValid() const;
		Entity* Get() const;

		// Comparison:
		bool operator==(const EntityHandle& other) const = default;

	private: // Methods:
		// Constructor:
		EntityHandle(EntityId entityId, uint64_t runtimeId);
	};
}