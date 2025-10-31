#pragma once
#include <vector>
#include <entt/entt.hpp>



namespace emberEngine
{
	struct ComponentRegistry
	{
		// Callback registry getters:
		static std::vector<void(*)(entt::registry&)>& GetStartRegistry();
		static std::vector<void(*)(entt::registry&)>& GetFixedUpdateRegistry();
		static std::vector<void(*)(entt::registry&)>& GetUpdateRegistry();
		static std::vector<void(*)(entt::registry&)>& GetLateUpdateRegistry();



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

			GetStartRegistry().push_back(startCallback);
			GetFixedUpdateRegistry().push_back(fixedUpdateCallback);
			GetUpdateRegistry().push_back(updateCallback);
			GetLateUpdateRegistry().push_back(lateUpdateCallback);
		}
	};
}
