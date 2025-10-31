#pragma once
#include "component.h"
#include "entity.h"



namespace emberEngine
{
	template<typename T>
	inline T* Component::GetComponent() const
	{
		return GetEntity().GetComponent<T>();
	}
}