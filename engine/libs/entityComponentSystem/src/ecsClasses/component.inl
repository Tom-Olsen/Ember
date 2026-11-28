#pragma once
#include "component.h"
#include "entity.inl"



namespace emberEcs
{
	template<typename T>
	inline T* Component::GetComponent() const
	{
		const Entity* pEntity = m_entity.Get();
		return pEntity != nullptr ? pEntity->GetComponent<T>() : nullptr;
	}
}