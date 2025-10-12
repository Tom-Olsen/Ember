#ifndef __INCLUDE_GUARD_sphFluid2dGpuEditorWindow_h__
#define __INCLUDE_GUARD_sphFluid2dGpuEditorWindow_h__
#include "editor.h"
#include "editorWindow.h"
#include "sphFluid2dGpu.h"
#include <imgui.h>



namespace emberEngine
{
	struct SphFluid2dGpuEditorWindow : EditorWindow
	{
	private:
		SphFluid2dGpu* m_pScript;
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
		SphFluid2dGpuEditorWindow(SphFluid2dGpu* pScript)
		{
			m_name = "Sph Fluid 2d Gpu";
			m_ID = 0;
			m_windowFlags = ImGuiWindowFlags_None;
			m_wantCaptureEvents = true;
			m_nameID = m_name + "##" + std::to_string(m_ID);
			m_pScript = pScript;
			GetData();

			// No serialised data available: get member values from script:
			//if (...)
				// GetData();
			// Get member values from data serialization:
			//else
				// Set m_... values and do m_pScript->Set...() for all members.
		}
		void Render() override
		{
			GetData();
			Editor::Checkbox("Is Running:", &m_isRunning);
			Editor::InputFloat("Time Scale:", &m_timeScale);
			Editor::Checkbox("Use Grid Optimization:", &m_useGridOptimization);
			Editor::Text(("Time Step:" + std::to_string(m_pScript->GetTimeStep())).c_str());
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
				m_pScript->Reset();
			SetData();
		}

	private:
		void GetData()
		{
			m_isRunning = m_pScript->GetIsRunning();
			m_timeScale = m_pScript->GetTimeScale();
			m_useGridOptimization = m_pScript->GetUseGridOptimization();
			m_particleCount = m_pScript->GetParticleCount();
			m_effectRadius = m_pScript->GetEffectRadius();
			m_mass = m_pScript->GetMass();
			m_viscosity = m_pScript->GetViscosity();
			m_surfaceTension = m_pScript->GetSurfaceTension();
			m_collisionDampening = m_pScript->GetCollisionDampening();
			m_targetDensity = m_pScript->GetTargetDensity();
			m_pressureMultiplier = m_pScript->GetPressureMultiplier();
			m_gravity = m_pScript->GetGravity();
			m_maxVelocity = m_pScript->GetMaxVelocity();
			m_attractorRadius = m_pScript->GetAttractorRadius();
			m_attractorStrength = m_pScript->GetAttractorStrength();
			m_colorMode = m_pScript->GetColorMode();
			m_initialDistributionRadius = m_pScript->GetInitialDistributionRadius();
			m_visualRadius = m_pScript->GetVisualRadius();
		}
		void SetData()
		{
			m_pScript->SetIsRunning(m_isRunning);
			m_pScript->SetTimeScale(m_timeScale);
			m_pScript->SetUseGridOptimization(m_useGridOptimization);
			m_pScript->SetParticleCount(m_particleCount);
			m_pScript->SetEffectRadius(m_effectRadius);
			m_pScript->SetMass(m_mass);
			m_pScript->SetViscosity(m_viscosity);
			m_pScript->SetSurfaceTension(m_surfaceTension);
			m_pScript->SetCollisionDampening(m_collisionDampening);
			m_pScript->SetTargetDensity(m_targetDensity);
			m_pScript->SetPressureMultiplier(m_pressureMultiplier);
			m_pScript->SetGravity(m_gravity);
			m_pScript->SetMaxVelocity(m_maxVelocity);
			m_pScript->SetAttractorRadius(m_attractorRadius);
			m_pScript->SetAttractorStrength(m_attractorStrength);
			m_pScript->SetColorMode(m_colorMode);
			m_pScript->SetInitialDistributionRadius(m_initialDistributionRadius);
			m_pScript->SetVisualRadius(m_visualRadius);
		}
	};
}



#endif // __INCLUDE_GUARD_sphFluid2dGpuEditorWindow_h__