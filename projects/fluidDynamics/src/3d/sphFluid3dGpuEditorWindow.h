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
		// Time:
		bool m_isRunning;
		float m_timeScale;
		float m_physicsTimeScale;
		// Fluid:
		int m_particleCount;
		float m_initialDistributionRadius;
		float m_effectRadius;
		float m_mass;
		float m_viscosity;
		float m_surfaceTension;
		float m_collisionDampening;
		float m_targetDensity;
		float m_pressureMultiplier;
		float m_nearPressureRatio;
		float m_maxVelocity;
		// Forces:
		float m_gravity;
		float m_attractorRadius;
		float m_attractorStrength;
		// Particle visuals:
		bool m_renderParticles;
		int m_colorMode;
		float m_visualRadius;
		// Volumetric visuals:
		bool m_renderVolumetricDensity;
		Uint3 m_volumetricDensityResolution;
		float m_volumetricDensityRayStepLength;
		float m_volumetricDensityAbsorption;
		Float4 m_volumetricDensityColorLow;
		Float4 m_volumetricDensityColorHigh;
		// Lighting:
		bool m_renderVolumetricLight;
		Uint3 m_volumetricLightingResolution;
		float m_volumetricLightingScattering;
		// Internal:
		RotatedBounds m_fluidBounds;

	public:
		SphFluid3dGpuEditorWindow(fluidDynamics::SphFluid3dGpu* pScript)
		{
			m_name = "Sph Fluid 3d Gpu";
			m_ID = 0;
			m_windowFlags = emberCommon::GuiWindowFlags::none;
			m_wantCaptureEvents = true;
			m_nameID = m_name + "##" + std::to_string(m_ID);
			m_pScript = pScript;
			m_fluidBounds = m_pScript->GetFluidBounds();
			m_boundsHandleTarget.SetBounds(&m_fluidBounds);
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
			if (m_fluidBounds != m_pScript->GetFluidBounds())
				m_pScript->SetFluidBounds(m_fluidBounds);
		}
		void Render() override
		{
			GetData();

			// Time:
			Gui::SeparatorText("Time");
			Gui::DragFloat("Time Scale:", &m_timeScale);
			Gui::DragFloat("Physics Time Scale:", &m_physicsTimeScale);
			Gui::Text(("Time Step:" + std::to_string(m_pScript->GetTimeStep())).c_str());
			// Fluid:
			Gui::SeparatorText("Fluid");
			Gui::DragInt("Particle Count:", &m_particleCount);
			Gui::DragFloat("Initial Distribution Radius:", &m_initialDistributionRadius, 0.1f, 1.0f, "%.8f");
			Gui::DragFloat("Effect Radius:", &m_effectRadius, 0.1f, 1.0f, "%.8f");
			Gui::DragFloat("Mass:", &m_mass, 0.1f, 1.0f, "%.8f");
			Gui::DragFloat("Viscosity:", &m_viscosity, 0.1f, 1.0f, "%.8f");
			Gui::DragFloat("Surface Tension:", &m_surfaceTension, 0.1f, 1.0f, "%.8f");
			Gui::DragFloat("Collision Dampening:", &m_collisionDampening, 0.1f, 1.0f, "%.8f");
			Gui::DragFloat("Target Density:", &m_targetDensity, 0.1f, 1.0f, "%.8f");
			Gui::DragFloat("Pressure Multiplier:", &m_pressureMultiplier, 0.1f, 1.0f, "%.8f");
			Gui::DragFloat("Near Pressure Ratio:", &m_nearPressureRatio, 0.01f, 0.0f, "%.8f");
			Gui::DragFloat("Max Velocity:", &m_maxVelocity,0.1f, 1.0f,"%.8f");
			// Forces:
			Gui::SeparatorText("Forces");
			Gui::DragFloat("Gravity:", &m_gravity,0.1f, 1.0f,"%.8f");
			Gui::DragFloat("Attractor Radius:", &m_attractorRadius,0.1f, 1.0f,"%.8f");
			Gui::DragFloat("Attractor Strength:", &m_attractorStrength,0.1f, 1.0f,"%.8f");
			// Particle visuals:
			Gui::SeparatorText("Particle Visuals");
			Gui::Checkbox("Render Particles:", &m_renderParticles);
			if (m_renderParticles)
			{
				Gui::DragInt("Color Mode:", &m_colorMode);
				Gui::DragFloat("Visual Radius:", &m_visualRadius, 0.1f, 1.0f, "%.8f");
			}
			// Volumetric visuals:
			Gui::SeparatorText("Volumetric Visuals");
			Gui::Checkbox("Render Volumetric Density:", &m_renderVolumetricDensity);
			if (m_renderVolumetricDensity)
			{
				int x = static_cast<int>(m_volumetricDensityResolution.x);
				int y = static_cast<int>(m_volumetricDensityResolution.y);
				int z = static_cast<int>(m_volumetricDensityResolution.z);
				Gui::DragInt("Density Resolution x:", &x, 0.1f, 1.0f);
				Gui::DragInt("Density Resolution y:", &y, 0.1f, 1.0f);
				Gui::DragInt("Density Resolution z:", &z, 0.1f, 1.0f);
				m_volumetricDensityResolution.x = static_cast<uint32_t>(math::Max(1, x));
				m_volumetricDensityResolution.y = static_cast<uint32_t>(math::Max(1, y));
				m_volumetricDensityResolution.z = static_cast<uint32_t>(math::Max(1, z));
				Gui::DragFloat("Ray Step Length:", &m_volumetricDensityRayStepLength, 0.1f, 1.0f, "%.8f");
				Gui::DragFloat("Absorption:", &m_volumetricDensityAbsorption, 0.01f, 0.1f, "%.8f");
				Gui::ColorEdit("Fluid Color Low:", &m_volumetricDensityColorLow);
				Gui::ColorEdit("Fluid Color High:", &m_volumetricDensityColorHigh);
			}
			// Lighting:
			Gui::SeparatorText("Lighting");
			Gui::Checkbox("Render Volumetric Light:", &m_renderVolumetricLight);
			if (m_renderVolumetricLight)
			{
				// ToDo: add support for vector like input/drag widgets: (u)int2/3 float2/3/4, etc.
				int x = static_cast<int>(m_volumetricLightingResolution.x);
				int y = static_cast<int>(m_volumetricLightingResolution.y);
				int z = static_cast<int>(m_volumetricLightingResolution.z);
				Gui::DragInt("Resolution x:", &x, 0.1f, 1.0f);
				Gui::DragInt("Resolution y:", &y, 0.1f, 1.0f);
				Gui::DragInt("Resolution z:", &z, 0.1f, 1.0f);
				m_volumetricLightingResolution.x = static_cast<uint32_t>(math::Max(1, x));
				m_volumetricLightingResolution.y = static_cast<uint32_t>(math::Max(1, y));
				m_volumetricLightingResolution.z = static_cast<uint32_t>(math::Max(1, z));
				Gui::DragFloat("Scattering:", &m_volumetricLightingScattering, 0.1f, 0.0f, "%.8f");
			}

			// Buttons:
			Gui::SeparatorText("Buttons");
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
			// Time:
			m_isRunning = m_pScript->GetIsRunning();
			m_timeScale = m_pScript->GetTimeScale();
			m_physicsTimeScale = m_pScript->GetPhysicsTimeScale();
			// Fluid:
			m_particleCount = m_pScript->GetParticleCount();
			m_initialDistributionRadius = m_pScript->GetInitialDistributionRadius();
			m_effectRadius = m_pScript->GetEffectRadius();
			m_mass = m_pScript->GetMass();
			m_viscosity = m_pScript->GetViscosity();
			m_surfaceTension = m_pScript->GetSurfaceTension();
			m_collisionDampening = m_pScript->GetCollisionDampening();
			m_targetDensity = m_pScript->GetTargetDensity();
			m_pressureMultiplier = m_pScript->GetPressureMultiplier();
			m_nearPressureRatio = m_pScript->GetNearPressureRatio();
			m_maxVelocity = m_pScript->GetMaxVelocity();
			// Forces:
			m_gravity = m_pScript->GetGravity();
			m_attractorRadius = m_pScript->GetAttractorRadius();
			m_attractorStrength = m_pScript->GetAttractorStrength();
			// Visuals:
			m_colorMode = m_pScript->GetColorMode();
			m_visualRadius = m_pScript->GetVisualRadius();
			m_renderParticles = m_pScript->GetRenderParticles();
			m_renderVolumetricDensity = m_pScript->GetRenderVolumetricDensity();
			m_volumetricDensityResolution = m_pScript->GetVolumetricDensityResolution();
			m_volumetricDensityRayStepLength = m_pScript->GetVolumetricDensityRayStepLength();
			m_volumetricDensityAbsorption = m_pScript->GetVolumetricDensityAbsorption();
			m_volumetricDensityColorLow = m_pScript->GetVolumetricDensityColorLow();
			m_volumetricDensityColorHigh = m_pScript->GetVolumetricDensityColorHigh();
			m_renderVolumetricLight = m_pScript->GetRenderVolumetricLight();
			m_volumetricLightingResolution = m_pScript->GetVolumetricLightingResolution();
			m_volumetricLightingScattering = m_pScript->GetVolumetricLightingScattering();
		}
		void SetData()
		{
			// Time:
			m_pScript->SetIsRunning(m_isRunning);
			m_pScript->SetTimeScale(m_timeScale);
			m_pScript->SetPhysicsTimeScale(m_physicsTimeScale);
			// Fluid:
			m_pScript->SetParticleCount(m_particleCount);
			m_pScript->SetInitialDistributionRadius(m_initialDistributionRadius);
			m_pScript->SetEffectRadius(m_effectRadius);
			m_pScript->SetMass(m_mass);
			m_pScript->SetViscosity(m_viscosity);
			m_pScript->SetSurfaceTension(m_surfaceTension);
			m_pScript->SetCollisionDampening(m_collisionDampening);
			m_pScript->SetTargetDensity(m_targetDensity);
			m_pScript->SetPressureMultiplier(m_pressureMultiplier);
			m_pScript->SetNearPressureRatio(m_nearPressureRatio);
			m_pScript->SetMaxVelocity(m_maxVelocity);
			// Forces:
			m_pScript->SetGravity(m_gravity);
			m_pScript->SetAttractorRadius(m_attractorRadius);
			m_pScript->SetAttractorStrength(m_attractorStrength);
			// Visuals:
			m_pScript->SetColorMode(m_colorMode);
			m_pScript->SetVisualRadius(m_visualRadius);
			m_pScript->SetRenderParticles(m_renderParticles);
			m_pScript->SetRenderVolumetricDensity(m_renderVolumetricDensity);
			m_pScript->SetVolumetricDensityResolution(m_volumetricDensityResolution);
			m_pScript->SetVolumetricDensityRayStepLength(m_volumetricDensityRayStepLength);
			m_pScript->SetVolumetricDensityAbsorption(m_volumetricDensityAbsorption);
			m_pScript->SetVolumetricDensityColorLow(m_volumetricDensityColorLow);
			m_pScript->SetVolumetricDensityColorHigh(m_volumetricDensityColorHigh);
			m_pScript->SetRenderVolumetricLight(m_renderVolumetricLight);
			m_pScript->SetVolumetricLightingResolution(m_volumetricLightingResolution);
			m_pScript->SetVolumetricLightingScattering(m_volumetricLightingScattering);
		}
	};
}