#include "componentRegistry.h"



namespace emberEngine
{
	std::vector<void(*)(entt::registry&)>& ComponentRegistry::GetStartRegistry()
	{
		static std::vector<void(*)(entt::registry&)> s_startRegistry;
		return s_startRegistry;
	}
	std::vector<void(*)(entt::registry&)>& ComponentRegistry::GetFixedUpdateRegistry()
	{
		static std::vector<void(*)(entt::registry&)> s_fixedUpdateRegistry;
		return s_fixedUpdateRegistry;
	}
	std::vector<void(*)(entt::registry&)>& ComponentRegistry::GetUpdateRegistry()
	{
		static std::vector<void(*)(entt::registry&)> s_updateRegistry;
		return s_updateRegistry;
	}
	std::vector<void(*)(entt::registry&)>& ComponentRegistry::GetLateUpdateRegistry()
	{
		static std::vector<void(*)(entt::registry&)> s_lateUpdateRegistry;
		return s_lateUpdateRegistry;
	}
	std::vector<void(*)(entt::registry&)>& ComponentRegistry::GetDestructionRegistry()
	{
		static std::vector<void(*)(entt::registry&)> s_destructionRegistry;
		return s_destructionRegistry;
	}
}