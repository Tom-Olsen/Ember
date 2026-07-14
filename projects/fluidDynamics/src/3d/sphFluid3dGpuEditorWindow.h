#pragma once
#include "commonGuiFlags.h"
#include "editorWindow.h"
#include "gui.h"
#include "sphFluid3dGpu.h"
#include "transformHandle.h"
#include "rotatedBoundsHandleTarget.h"



namespace emberEditor
{
	struct SphFluid3dGpuEditorWindow : public emberCore::EditorWindow
	{
		// Easy access to emberEngine Gui:
		using Gui = emberCore::Gui;



	private:
		TransformHandle m_transformHandle;
		RotatedBoundsHandleTarget m_boundsHandleTarget;
		fluidDynamics::SphFluid3dGpu* m_pScript;
		bool m_isRunning;
		float m_timeScale;
		float m_physicsTimeScale;
		bool m_useGridOptimization;
		bool m_computeDensityTexture3d;
		int m_particleCount;
		float m_effectRadius;
		float m_mass;
		float m_viscosity;
		float m_surfaceTension;
		float m_collisionDampening;
		float m_targetDensity;
		float m_pressureMultiplier;
		float m_nearPressureRatio;
		float m_gravity;
		float m_maxVelocity;
        RotatedBounds m_bounds;
		float m_attractorRadius;
		float m_attractorStrength;
		int m_colorMode;
		float m_initialDistributionRadius;
		float m_visualRadius;
		bool m_renderParticles;
		bool m_renderVolumetricDensity;
		float m_volumetricDensityRayStepLength;
		float m_volumetricDensityAbsorption;

	public:
		SphFluid3dGpuEditorWindow(fluidDynamics::SphFluid3dGpu* pScript)
		{
			m_name = "Sph Fluid 3d Gpu";
			m_ID = 0;
			m_windowFlags = emberCommon::GuiWindowFlags::none;
			m_wantCaptureEvents = true;
			m_nameID = m_name + "##" + std::to_string(m_ID);
			m_pScript = pScript;
			m_bounds = m_pScript->GetFluidBounds();
			m_boundsHandleTarget.SetBounds(&m_bounds);
            m_transformHandle.SetTarget(&m_boundsHandleTarget);
			GetData();

			// No serialised data available: get member values from script:
			//if (...)
				// GetData();
			// Get member values from data serialization:
			//else
				// Set m_... values and do m_pScript->Set...() for all members.
		}
		void PreRender() override
        {
		    m_transformHandle.ConsumeModeHotkeys();
            m_transformHandle.Update();
            m_transformHandle.Draw();
            if (m_bounds != m_pScript->GetFluidBounds())
	            m_pScript->SetFluidBounds(m_bounds);
        }
		void Render() override
		{
			GetData();

			// Fields:
			Gui::Checkbox("Is Running:", &m_isRunning);
			Gui::DragFloat("Time Scale:", &m_timeScale);
			Gui::DragFloat("Physics Time Scale:", &m_physicsTimeScale);
			Gui::Checkbox("Use Grid Optimization:", &m_useGridOptimization);
			Gui::Checkbox("Compute Density Texture 3d:", &m_computeDensityTexture3d);
			Gui::Text(("Time Step:" + std::to_string(m_pScript->GetTimeStep())).c_str());
			Gui::DragInt("Particle Count:", &m_particleCount);
			Gui::DragFloat("Effect Radius:", &m_effectRadius, 0.1f, 1.0f, "%.8f");
			Gui::DragFloat("Mass:", &m_mass, 0.1f, 1.0f, "%.8f");
			Gui::DragFloat("Viscosity:", &m_viscosity, 0.1f, 1.0f, "%.8f");
			Gui::DragFloat("Surface Tension:", &m_surfaceTension, 0.1f, 1.0f, "%.8f");
			Gui::DragFloat("Collision Dampening:", &m_collisionDampening, 0.1f, 1.0f, "%.8f");
			Gui::DragFloat("Target Density:", &m_targetDensity, 0.1f, 1.0f, "%.8f");
			Gui::DragFloat("Pressure Multiplier:", &m_pressureMultiplier, 0.1f, 1.0f, "%.8f");
			Gui::DragFloat("Near Pressure Ratio:", &m_nearPressureRatio, 0.01f, 0.0f, "%.8f");
			Gui::DragFloat("Gravity:", &m_gravity,0.1f, 1.0f,"%.8f");
			Gui::DragFloat("Max Velocity:", &m_maxVelocity,0.1f, 1.0f,"%.8f");
			Gui::DragFloat("Attractor Radius:", &m_attractorRadius,0.1f, 1.0f,"%.8f");
			Gui::DragFloat("Attractor Strength:", &m_attractorStrength,0.1f, 1.0f,"%.8f");
			Gui::DragInt("Color Mode:", &m_colorMode);
			Gui::DragFloat("Initial Distribution Radius:", &m_initialDistributionRadius, 0.1f, 1.0f, "%.8f");
			Gui::DragFloat("Visual Radius:", &m_visualRadius, 0.1f, 1.0f, "%.8f");
			Gui::Checkbox("Render Particles:", &m_renderParticles);
			Gui::Checkbox("Render Volumetric Density:", &m_renderVolumetricDensity);
			Gui::DragFloat("Volumetric Density Ray Step Length:", &m_volumetricDensityRayStepLength, 0.1f, 1.0f, "%.8f");
			Gui::DragFloat("Volumetric Density Absorption:", &m_volumetricDensityAbsorption, 0.01f, 0.1f, "%.8f");

			// Buttons:
			static bool step = false;
			if (step)
				step = m_isRunning = false;
			if (Gui::Button("Step"))
				step = m_isRunning = true;
			if (Gui::Button("Reset Simulation"))
				m_pScript->Reset();
			if (Gui::Button("Print"))
				m_pScript->Print();

			SetData();
		}

	private:
		void GetData()
		{
			m_isRunning = m_pScript->GetIsRunning();
			m_timeScale = m_pScript->GetTimeScale();
			m_physicsTimeScale = m_pScript->GetPhysicsTimeScale();
			m_useGridOptimization = m_pScript->GetUseGridOptimization();
			m_computeDensityTexture3d = m_pScript->GetComputeDensityTexture3d();
			m_particleCount = m_pScript->GetParticleCount();
			m_effectRadius = m_pScript->GetEffectRadius();
			m_mass = m_pScript->GetMass();
			m_viscosity = m_pScript->GetViscosity();
			m_surfaceTension = m_pScript->GetSurfaceTension();
			m_collisionDampening = m_pScript->GetCollisionDampening();
			m_targetDensity = m_pScript->GetTargetDensity();
			m_pressureMultiplier = m_pScript->GetPressureMultiplier();
			m_nearPressureRatio = m_pScript->GetNearPressureRatio();
			m_gravity = m_pScript->GetGravity();
			m_maxVelocity = m_pScript->GetMaxVelocity();
			m_attractorRadius = m_pScript->GetAttractorRadius();
			m_attractorStrength = m_pScript->GetAttractorStrength();
			m_colorMode = m_pScript->GetColorMode();
			m_initialDistributionRadius = m_pScript->GetInitialDistributionRadius();
			m_visualRadius = m_pScript->GetVisualRadius();
			m_renderParticles = m_pScript->GetRenderParticles();
			m_renderVolumetricDensity = m_pScript->GetRenderVolumetricDensity();
			m_volumetricDensityRayStepLength = m_pScript->GetVolumetricDensityRayStepLength();
			m_volumetricDensityAbsorption = m_pScript->GetVolumetricDensityAbsorption();
		}
		void SetData()
		{
			m_pScript->SetIsRunning(m_isRunning);
			m_pScript->SetTimeScale(m_timeScale);
			m_pScript->SetPhysicsTimeScale(m_physicsTimeScale);
			m_pScript->SetUseHashGridOptimization(m_useGridOptimization);
			m_pScript->SetComputeDensityTexture3d(m_computeDensityTexture3d);
			m_pScript->SetParticleCount(m_particleCount);
			m_pScript->SetEffectRadius(m_effectRadius);
			m_pScript->SetMass(m_mass);
			m_pScript->SetViscosity(m_viscosity);
			m_pScript->SetSurfaceTension(m_surfaceTension);
			m_pScript->SetCollisionDampening(m_collisionDampening);
			m_pScript->SetTargetDensity(m_targetDensity);
			m_pScript->SetPressureMultiplier(m_pressureMultiplier);
			m_pScript->SetNearPressureRatio(m_nearPressureRatio);
			m_pScript->SetGravity(m_gravity);
			m_pScript->SetMaxVelocity(m_maxVelocity);
			m_pScript->SetAttractorRadius(m_attractorRadius);
			m_pScript->SetAttractorStrength(m_attractorStrength);
			m_pScript->SetColorMode(m_colorMode);
			m_pScript->SetInitialDistributionRadius(m_initialDistributionRadius);
			m_pScript->SetVisualRadius(m_visualRadius);
			m_pScript->SetRenderParticles(m_renderParticles);
			m_pScript->SetRenderVolumetricDensity(m_renderVolumetricDensity);
			m_pScript->SetVolumetricDensityRayStepLength(m_volumetricDensityRayStepLength);
			m_pScript->SetVolumetricDensityAbsorption(m_volumetricDensityAbsorption);
		}
	};
}