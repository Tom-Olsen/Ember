#pragma once
#include "emberEngine.h"



namespace emberEcs
{
	class EntityState : public Component
	{
		friend class GameObject;

	private: // Members:
		bool m_isEnabled;

	public: // Methods:
		// Constructor/Destructor:
		EntityState(bool isEnabled = true);
		~EntityState();

		// Getters/Setters:
		void SetIsEnabled(bool isEnabled);
		bool GetIsEnabled() const;
	};
}