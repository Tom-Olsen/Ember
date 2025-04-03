#ifndef __INCLUDE_GUARD_sphFluid2dEditorWindow_h__
#define __INCLUDE_GUARD_sphFluid2dEditorWindow_h__
#include "editorWindow.h"
#include "editor.h"
#include "sphFluid2d.h"
#include <imgui.h>



namespace emberEngine
{
	struct SphFluid2dEditorWindow : EditorWindow
	{
	private:
		SphFluid2d* m_pSphFluid2d;
		bool m_isRunning;
		float m_timeScale;
		bool m_useGridOptimization;
		int m_particleCount;
		float m_effectRadius;
		float m_visualRadius;
		float m_mass;
		float m_viscosity;
		float m_surfaceTension;
		float m_collisionDampening;
		float m_targetDensity;
		float m_pressureMultiplier;
		float m_gravity;
		float m_maxVelocity;

	public:
		SphFluid2dEditorWindow(SphFluid2d* pSphFluid2d)
		{
			m_pSphFluid2d = pSphFluid2d;

			// No serialised data available: get member values from script:
			//if (...)
			{
				m_isRunning = m_pSphFluid2d->GetIsRunning();
				m_timeScale = m_pSphFluid2d->GetTimeScale();
				m_useGridOptimization = m_pSphFluid2d->GetUseGridOptimization();
				m_particleCount = m_pSphFluid2d->GetParticleCount();
				m_effectRadius = m_pSphFluid2d->GetEffectRadius();
				m_visualRadius = m_pSphFluid2d->GetVisualRadius();
				m_mass = m_pSphFluid2d->GetMass();
				m_viscosity = m_pSphFluid2d->GetViscosity();
				m_surfaceTension = m_pSphFluid2d->GetSurfaceTension();
				m_collisionDampening = m_pSphFluid2d->GetCollisionDampening();
				m_targetDensity = m_pSphFluid2d->GetTargetDensity();
				m_pressureMultiplier = m_pSphFluid2d->GetPressureMultiplier();
				m_gravity = m_pSphFluid2d->GetGravity();
				m_maxVelocity = m_pSphFluid2d->GetMaxVelocity();
			}
			// Get member values from data serialization:
			//else
			{
				// Set m_... values and do m_script->Set...() for all members.
			}
		}
		void Render() override
		{
			m_isRunning = m_pSphFluid2d->GetIsRunning();
			m_timeScale = m_pSphFluid2d->GetTimeScale();
			m_useGridOptimization = m_pSphFluid2d->GetUseGridOptimization();
			m_particleCount = m_pSphFluid2d->GetParticleCount();
			m_effectRadius = m_pSphFluid2d->GetEffectRadius();
			m_visualRadius = m_pSphFluid2d->GetVisualRadius();
			m_mass = m_pSphFluid2d->GetMass();
			m_viscosity = m_pSphFluid2d->GetViscosity();
			m_surfaceTension = m_pSphFluid2d->GetSurfaceTension();
			m_collisionDampening = m_pSphFluid2d->GetCollisionDampening();
			m_targetDensity = m_pSphFluid2d->GetTargetDensity();
			m_pressureMultiplier = m_pSphFluid2d->GetPressureMultiplier();
			m_gravity = m_pSphFluid2d->GetGravity();
			m_maxVelocity = m_pSphFluid2d->GetMaxVelocity();

			ImGui::Begin("Sph Fluid 2d Editor Window");
			{
				Editor::Checkbox("Is Running:", &m_isRunning);
				Editor::InputFloat("Time Scale:", &m_timeScale);
				Editor::Checkbox("Use Grid Optimization:", &m_useGridOptimization);
				Editor::Text(("Time Step:" + std::to_string(m_pSphFluid2d->GetTimeStep())).c_str());
				Editor::InputInt("Particle Count:", &m_particleCount);
				Editor::InputFloat("Effect Radius:", &m_effectRadius, 0.1f, 1.0f, "%.8f");
				Editor::InputFloat("Visual Radius:", &m_visualRadius, 0.1f, 1.0f, "%.8f");
				Editor::InputFloat("Mass:", &m_mass, 0.1f, 1.0f, "%.8f");
				Editor::InputFloat("Viscosity:", &m_viscosity, 0.1f, 1.0f, "%.8f");
				Editor::InputFloat("Surface Tension:", &m_surfaceTension, 0.1f, 1.0f, "%.8f");
				Editor::InputFloat("Collision Dampening:", &m_collisionDampening, 0.1f, 1.0f, "%.8f");
				Editor::InputFloat("Target Density:", &m_targetDensity, 0.1f, 1.0f, "%.8f");
				Editor::InputFloat("Pressure Multiplier:", &m_pressureMultiplier, 0.1f, 1.0f, "%.8f");
				Editor::InputFloat("Gravity:", &m_gravity,0.1f, 1.0f,"%.8f");
				Editor::InputFloat("Max Velocity:", &m_maxVelocity,0.1f, 1.0f,"%.8f");

				if (ImGui::Button("Reset Simulation"))
					m_pSphFluid2d->Reset();
			}
			ImGui::End();

			m_pSphFluid2d->SetIsRunning(m_isRunning);
			m_pSphFluid2d->SetTimeScale(m_timeScale);
			m_pSphFluid2d->SetUseGridOptimization(m_useGridOptimization);
			m_pSphFluid2d->SetParticleCount(m_particleCount);
			m_pSphFluid2d->SetEffectRadius(m_effectRadius);
			m_pSphFluid2d->SetVisualRadius(m_visualRadius);
			m_pSphFluid2d->SetMass(m_mass);
			m_pSphFluid2d->SetViscosity(m_viscosity);
			m_pSphFluid2d->SetSurfaceTension(m_surfaceTension);
			m_pSphFluid2d->SetCollisionDampening(m_collisionDampening);
			m_pSphFluid2d->SetTargetDensity(m_targetDensity);
			m_pSphFluid2d->SetPressureMultiplier(m_pressureMultiplier);
			m_pSphFluid2d->SetGravity(m_gravity);
			m_pSphFluid2d->SetMaxVelocity(m_maxVelocity);
		}
	};
}



#endif // __INCLUDE_GUARD_sphFluid2dEditorWindow_h__