#ifndef __INCLUDE_GUARD_sphFluid2dCpuEditorWindow_h__
#define __INCLUDE_GUARD_sphFluid2dCpuEditorWindow_h__
#include "editorWindow.h"
#include "editor.h"
#include "sphFluid2dCpu.h"
#include <imgui.h>



namespace emberEngine
{
	struct SphFluid2dCpuEditorWindow : EditorWindow
	{
	private:
		SphFluid2dCpu* m_pSphFluid2dCpu;
		bool m_isRunning;
		float m_timeScale;
		bool m_useGridOptimization;
		int m_particleCount;
		float m_effectRadius;
		float m_mass;
		float m_viscosity;
		float m_surfaceTension;
		float m_collisionDampening;
		float m_targetDensity;
		float m_pressureMultiplier;
		float m_gravity;
		float m_maxVelocity;
		float m_attractorRadius;
		float m_attractorStrength;
		float m_initialDistributionRadius;
		float m_visualRadius;

	public:
		SphFluid2dCpuEditorWindow(SphFluid2dCpu* pSphFluid2dCpu)
		{
			m_pSphFluid2dCpu = pSphFluid2dCpu;
			GetData();

			// No serialised data available: get member values from script:
			//if (...)
				// GetData();
			// Get member values from data serialization:
			//else
				// Set m_... values and do m_script->Set...() for all members.
		}
		void Render() override
		{
			GetData();
			ImGui::Begin("Sph Fluid 2d Editor Window");
			{
				Editor::Checkbox("Is Running:", &m_isRunning);
				Editor::InputFloat("Time Scale:", &m_timeScale);
				Editor::Checkbox("Use Grid Optimization:", &m_useGridOptimization);
				Editor::Text(("Time Step:" + std::to_string(m_pSphFluid2dCpu->GetTimeStep())).c_str());
				Editor::InputInt("Particle Count:", &m_particleCount);
				Editor::InputFloat("Effect Radius:", &m_effectRadius, 0.1f, 1.0f, "%.8f");
				Editor::InputFloat("Mass:", &m_mass, 0.1f, 1.0f, "%.8f");
				Editor::InputFloat("Viscosity:", &m_viscosity, 0.1f, 1.0f, "%.8f");
				Editor::InputFloat("Surface Tension:", &m_surfaceTension, 0.1f, 1.0f, "%.8f");
				Editor::InputFloat("Collision Dampening:", &m_collisionDampening, 0.1f, 1.0f, "%.8f");
				Editor::InputFloat("Target Density:", &m_targetDensity, 0.1f, 1.0f, "%.8f");
				Editor::InputFloat("Pressure Multiplier:", &m_pressureMultiplier, 0.1f, 1.0f, "%.8f");
				Editor::InputFloat("Gravity:", &m_gravity,0.1f, 1.0f,"%.8f");
				Editor::InputFloat("Max Velocity:", &m_maxVelocity,0.1f, 1.0f,"%.8f");
				Editor::InputFloat("Attractor Radius:", &m_attractorRadius,0.1f, 1.0f,"%.8f");
				Editor::InputFloat("Attractor Strength:", &m_attractorStrength,0.1f, 1.0f,"%.8f");
				Editor::InputFloat("Visual Radius:", &m_visualRadius, 0.1f, 1.0f, "%.8f");
				Editor::InputFloat("Initial Distribution Radius:", &m_initialDistributionRadius, 0.1f, 1.0f, "%.8f");

				if (ImGui::Button("Reset Simulation"))
					m_pSphFluid2dCpu->Reset();
			}
			ImGui::End();
			SetData();
		}

	private:
		void GetData()
		{
			m_isRunning = m_pSphFluid2dCpu->GetIsRunning();
			m_timeScale = m_pSphFluid2dCpu->GetTimeScale();
			m_useGridOptimization = m_pSphFluid2dCpu->GetUseGridOptimization();
			m_particleCount = m_pSphFluid2dCpu->GetParticleCount();
			m_effectRadius = m_pSphFluid2dCpu->GetEffectRadius();
			m_mass = m_pSphFluid2dCpu->GetMass();
			m_viscosity = m_pSphFluid2dCpu->GetViscosity();
			m_surfaceTension = m_pSphFluid2dCpu->GetSurfaceTension();
			m_collisionDampening = m_pSphFluid2dCpu->GetCollisionDampening();
			m_targetDensity = m_pSphFluid2dCpu->GetTargetDensity();
			m_pressureMultiplier = m_pSphFluid2dCpu->GetPressureMultiplier();
			m_gravity = m_pSphFluid2dCpu->GetGravity();
			m_maxVelocity = m_pSphFluid2dCpu->GetMaxVelocity();
			m_attractorRadius = m_pSphFluid2dCpu->GetAttractorRadius();
			m_attractorStrength = m_pSphFluid2dCpu->GetAttractorStrength();
			m_visualRadius = m_pSphFluid2dCpu->GetVisualRadius();
			m_initialDistributionRadius = m_pSphFluid2dCpu->GetInitialDistributionRadius();
		}
		void SetData()
		{
			m_pSphFluid2dCpu->SetIsRunning(m_isRunning);
			m_pSphFluid2dCpu->SetTimeScale(m_timeScale);
			m_pSphFluid2dCpu->SetUseGridOptimization(m_useGridOptimization);
			m_pSphFluid2dCpu->SetParticleCount(m_particleCount);
			m_pSphFluid2dCpu->SetEffectRadius(m_effectRadius);
			m_pSphFluid2dCpu->SetMass(m_mass);
			m_pSphFluid2dCpu->SetViscosity(m_viscosity);
			m_pSphFluid2dCpu->SetSurfaceTension(m_surfaceTension);
			m_pSphFluid2dCpu->SetCollisionDampening(m_collisionDampening);
			m_pSphFluid2dCpu->SetTargetDensity(m_targetDensity);
			m_pSphFluid2dCpu->SetPressureMultiplier(m_pressureMultiplier);
			m_pSphFluid2dCpu->SetGravity(m_gravity);
			m_pSphFluid2dCpu->SetMaxVelocity(m_maxVelocity);
			m_pSphFluid2dCpu->SetAttractorRadius(m_attractorRadius);
			m_pSphFluid2dCpu->SetAttractorStrength(m_attractorStrength);
			m_pSphFluid2dCpu->SetVisualRadius(m_visualRadius);
			m_pSphFluid2dCpu->SetInitialDistributionRadius(m_initialDistributionRadius);
		}
	};
}



#endif // __INCLUDE_GUARD_sphFluid2dCpuEditorWindow_h__