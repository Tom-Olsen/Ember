#ifndef __INCLUDE_GUARD_sphFluid2dGpuEditorWindow_h__
#define __INCLUDE_GUARD_sphFluid2dGpuEditorWindow_h__
#include "editorWindow.h"
#include "editor.h"
#include "sphFluid2dGpu.h"
#include <imgui.h>



namespace emberEngine
{
	struct SphFluid2dGpuEditorWindow : EditorWindow
	{
	private:
		SphFluid2dGpu* m_pSphFluid2dGpu;
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
		int m_colorMode;
		float m_initialDistributionRadius;
		float m_visualRadius;

	public:
		SphFluid2dGpuEditorWindow(SphFluid2dGpu* pSphFluid2dGpu)
		{
			m_pSphFluid2dGpu = pSphFluid2dGpu;
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
				Editor::Text(("Time Step:" + std::to_string(m_pSphFluid2dGpu->GetTimeStep())).c_str());
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
				Editor::InputInt("Color Mode:", &m_colorMode);
				Editor::InputFloat("Initial Distribution Radius:", &m_initialDistributionRadius, 0.1f, 1.0f, "%.8f");
				Editor::InputFloat("Visual Radius:", &m_visualRadius, 0.1f, 1.0f, "%.8f");

				if (ImGui::Button("Reset Simulation"))
					m_pSphFluid2dGpu->Reset();
			}
			ImGui::End();
			SetData();
		}

	private:
		void GetData()
		{
			m_isRunning = m_pSphFluid2dGpu->GetIsRunning();
			m_timeScale = m_pSphFluid2dGpu->GetTimeScale();
			m_useGridOptimization = m_pSphFluid2dGpu->GetUseGridOptimization();
			m_particleCount = m_pSphFluid2dGpu->GetParticleCount();
			m_effectRadius = m_pSphFluid2dGpu->GetEffectRadius();
			m_mass = m_pSphFluid2dGpu->GetMass();
			m_viscosity = m_pSphFluid2dGpu->GetViscosity();
			m_surfaceTension = m_pSphFluid2dGpu->GetSurfaceTension();
			m_collisionDampening = m_pSphFluid2dGpu->GetCollisionDampening();
			m_targetDensity = m_pSphFluid2dGpu->GetTargetDensity();
			m_pressureMultiplier = m_pSphFluid2dGpu->GetPressureMultiplier();
			m_gravity = m_pSphFluid2dGpu->GetGravity();
			m_maxVelocity = m_pSphFluid2dGpu->GetMaxVelocity();
			m_attractorRadius = m_pSphFluid2dGpu->GetAttractorRadius();
			m_attractorStrength = m_pSphFluid2dGpu->GetAttractorStrength();
			m_colorMode = m_pSphFluid2dGpu->GetColorMode();
			m_initialDistributionRadius = m_pSphFluid2dGpu->GetInitialDistributionRadius();
			m_visualRadius = m_pSphFluid2dGpu->GetVisualRadius();
		}
		void SetData()
		{
			m_pSphFluid2dGpu->SetIsRunning(m_isRunning);
			m_pSphFluid2dGpu->SetTimeScale(m_timeScale);
			m_pSphFluid2dGpu->SetUseGridOptimization(m_useGridOptimization);
			m_pSphFluid2dGpu->SetParticleCount(m_particleCount);
			m_pSphFluid2dGpu->SetEffectRadius(m_effectRadius);
			m_pSphFluid2dGpu->SetMass(m_mass);
			m_pSphFluid2dGpu->SetViscosity(m_viscosity);
			m_pSphFluid2dGpu->SetSurfaceTension(m_surfaceTension);
			m_pSphFluid2dGpu->SetCollisionDampening(m_collisionDampening);
			m_pSphFluid2dGpu->SetTargetDensity(m_targetDensity);
			m_pSphFluid2dGpu->SetPressureMultiplier(m_pressureMultiplier);
			m_pSphFluid2dGpu->SetGravity(m_gravity);
			m_pSphFluid2dGpu->SetMaxVelocity(m_maxVelocity);
			m_pSphFluid2dGpu->SetAttractorRadius(m_attractorRadius);
			m_pSphFluid2dGpu->SetAttractorStrength(m_attractorStrength);
			m_pSphFluid2dGpu->SetColorMode(m_colorMode);
			m_pSphFluid2dGpu->SetInitialDistributionRadius(m_initialDistributionRadius);
			m_pSphFluid2dGpu->SetVisualRadius(m_visualRadius);
		}
	};
}



#endif // __INCLUDE_GUARD_sphFluid2dGpuEditorWindow_h__