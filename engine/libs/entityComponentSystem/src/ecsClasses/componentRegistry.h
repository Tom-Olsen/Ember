#pragma once
#include <vector>
#include <entt/entt.hpp>



namespace emberEcs
{
	struct ComponentRegistry
	{
		// Callback registry getters:
		static std::vector<void(*)(entt::registry&)>& GetStartRegistry();
		static std::vector<void(*)(entt::registry&)>& GetFixedUpdateRegistry();
		static std::vector<void(*)(entt::registry&)>& GetEarlyUpdateRegistry();
		static std::vector<void(*)(entt::registry&)>& GetUpdateRegistry();
		static std::vector<void(*)(entt::registry&)>& GetLateUpdateRegistry();
		static std::vector<void(*)(entt::registry&)>& GetDestructionRegistry();



		// Register new componet class:
		template<typename T>
		static inline void Register()
		{
			auto startCallback = [](entt::registry& registry)
			{
				registry.view<T>().each([](entt::entity entity, T& component)
				{
					if (component.GetIsActive())
						component.Start();
				});
			};
			auto fixedUpdateCallback = [](entt::registry& registry)
			{
				registry.view<T>().each([](entt::entity entity, T& component)
				{
					if (component.GetIsActive())
						component.FixedUpdate();
				});
			};
			auto earlyUpdateCallback = [](entt::registry& registry)
			{
				registry.view<T>().each([](entt::entity entity, T& component)
				{
					if (component.GetIsActive())
						component.EarlyUpdate();
				});
			};
			auto updateCallback = [](entt::registry& registry)
			{
				registry.view<T>().each([](entt::entity entity, T& component)
				{
					if (component.GetIsActive())
						component.Update();
				});
			};
			auto lateUpdateCallback = [](entt::registry& registry)
			{
				registry.view<T>().each([](entt::entity entity, T& component)
				{
					if (component.GetIsActive())
						component.LateUpdate();
				});
			};
			auto destroyCallback = [](entt::registry& registry)
			{
				registry.clear<T>();
			};

			GetStartRegistry().push_back(startCallback);
			GetFixedUpdateRegistry().push_back(fixedUpdateCallback);
			GetEarlyUpdateRegistry().push_back(earlyUpdateCallback);
			GetUpdateRegistry().push_back(updateCallback);
			GetLateUpdateRegistry().push_back(lateUpdateCallback);
			GetDestructionRegistry().push_back(destroyCallback);
		}
	};
}