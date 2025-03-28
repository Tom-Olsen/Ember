#ifndef __INCLUDE_GUARD_sphFluid2dEditorWindow_h__
#define __INCLUDE_GUARD_sphFluid2dEditorWindow_h__
#include "editorWindow.h"
#include "sphFluid2d.h"
#include <imgui.h>



namespace emberEngine
{
	struct SphFluid2dEditorWindow : EditorWindow
	{
	private:
		SphFluid2d* m_pSphFluid2d;
		bool m_isRunning;
		int m_particleCount;
		float m_effectRadius;
		float m_visualRadius;
		float m_mass;
		float m_collisionDampening;
		float m_targetDensity;
		float m_pressureMultiplier;
		float m_gravity;

	public:
		SphFluid2dEditorWindow(SphFluid2d* pSphFluid2d)
		{
			m_pSphFluid2d = pSphFluid2d;
		}
		void Render() override
		{
			m_isRunning = m_pSphFluid2d->GetIsRunning();
			m_particleCount = m_pSphFluid2d->GetParticleCount();
			m_effectRadius = m_pSphFluid2d->GetEffectRadius();
			m_visualRadius = m_pSphFluid2d->GetVisualRadius();
			m_mass = m_pSphFluid2d->GetMass();
			m_collisionDampening = m_pSphFluid2d->GetCollisionDampening();
			m_targetDensity = m_pSphFluid2d->GetTargetDensity();
			m_pressureMultiplier = m_pSphFluid2d->GetPressureMultiplier();
			m_gravity = m_pSphFluid2d->GetGravity();

			ImGui::Begin("Sph Fluid 2d Editor Window");
			{
				ImGui::Checkbox("Is Running:", &m_isRunning);
				ImGui::Text(("Time Step:" + std::to_string(m_pSphFluid2d->GetTimeStep())).c_str());
				ImGui::InputInt("Particle Count:", &m_particleCount);
				ImGui::InputFloat("Effect Radius:", &m_effectRadius);
				ImGui::InputFloat("Visual Radius:", &m_visualRadius);
				ImGui::InputFloat("Mass:", &m_mass);
				ImGui::InputFloat("Collision Dampening:", &m_collisionDampening);
				ImGui::InputFloat("Target Density:", &m_targetDensity);
				ImGui::InputFloat("Pressure Multiplier:", &m_pressureMultiplier);
				ImGui::InputFloat("Gravity:", &m_gravity);

				if (ImGui::Button("Reset Simulation"))
					m_pSphFluid2d->Reset();
			}
			ImGui::End();

			m_pSphFluid2d->SetIsRunning(m_isRunning);
			m_pSphFluid2d->SetParticleCount(m_particleCount);
			m_pSphFluid2d->SetEffectRadius(m_effectRadius);
			m_pSphFluid2d->SetVisualRadius(m_visualRadius);
			m_pSphFluid2d->SetMass(m_mass);
			m_pSphFluid2d->SetCollisionDampening(m_collisionDampening);
			m_pSphFluid2d->SetTargetDensity(m_targetDensity);
			m_pSphFluid2d->SetPressureMultiplier(m_pressureMultiplier);
			m_pSphFluid2d->SetGravity(m_gravity);
		}
	};
}



#endif // __INCLUDE_GUARD_sphFluid2dEditorWindow_h__