#pragma once
#include "commonGuiFlags.h"
#include "editorWindow.h"
#include "gui.h"
#include "sphFluid3d.h"



namespace emberEditor
{
	struct SphFluid3dEditorWindow : public emberEngine::EditorWindow
	{
		// Easy access to emberEngine Gui:
		using Gui = emberEngine::Gui;



	private:
		fluidDynamics::SphFluid3d* m_pScript;
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
		int m_colorMode;
		int m_initialDistributionShellCount;
		float m_initialDistributionRadius;
		float m_visualRadius;

	public:
		SphFluid3dEditorWindow(fluidDynamics::SphFluid3d* pScript)
		{
			m_name = "Sph Fluid 3d";
			m_ID = 0;
			m_windowFlags = emberCommon::GuiWindowFlag_None;
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
			Gui::Checkbox("Is Running:", &m_isRunning);
			Gui::InputFloat("Time Scale:", &m_timeScale);
			Gui::Checkbox("Use Grid Optimization:", &m_useGridOptimization);
			Gui::Text(("Time Step:" + std::to_string(m_pScript->GetTimeStep())).c_str());
			Gui::InputInt("Particle Count:", &m_particleCount);
			Gui::InputFloat("Effect Radius:", &m_effectRadius, 0.1f, 1.0f, "%.8f");
			Gui::InputFloat("Mass:", &m_mass, 0.1f, 1.0f, "%.8f");
			Gui::InputFloat("Viscosity:", &m_viscosity, 0.1f, 1.0f, "%.8f");
			Gui::InputFloat("Surface Tension:", &m_surfaceTension, 0.1f, 1.0f, "%.8f");
			Gui::InputFloat("Collision Dampening:", &m_collisionDampening, 0.1f, 1.0f, "%.8f");
			Gui::InputFloat("Target Density:", &m_targetDensity, 0.1f, 1.0f, "%.8f");
			Gui::InputFloat("Pressure Multiplier:", &m_pressureMultiplier, 0.1f, 1.0f, "%.8f");
			Gui::InputFloat("Gravity:", &m_gravity, 0.1f, 1.0f, "%.8f");
			Gui::InputFloat("Max Velocity:", &m_maxVelocity, 0.1f, 1.0f, "%.8f");
			Gui::InputInt("Color Mode:", &m_colorMode);
			Gui::InputInt("Distr Shell Count:", &m_initialDistributionShellCount);
			Gui::InputFloat("Distr Radius:", &m_initialDistributionRadius, 0.1f, 1.0f, "%.8f");
			Gui::InputFloat("Visual Radius:", &m_visualRadius, 0.1f, 1.0f, "%.8f");

			if (Gui::Button("Reset Simulation"))
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
			m_colorMode = m_pScript->GetColorMode();
			m_initialDistributionShellCount = m_pScript->GetInitialDistributionShellCount();
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
			m_pScript->SetColorMode(m_colorMode);
			m_pScript->SetInitialDistributionShellCount(m_initialDistributionShellCount);
			m_pScript->SetInitialDistributionRadius(m_initialDistributionRadius);
			m_pScript->SetVisualRadius(m_visualRadius);
		}
	};
}