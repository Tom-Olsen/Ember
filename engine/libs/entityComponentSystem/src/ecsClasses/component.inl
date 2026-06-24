#pragma once
#include "component.h"
#include "entity.h"



namespace emberEcs
{
	template<typename T>
	inline T* Component::GetComponent() const
	{
		return GetEntity().GetComponent<T>();
	}
}