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
		// Visuals:
		fluidDynamics::SphFluid3dGpu::RenderMode m_renderMode;
		// Particles:
		int m_colorMode;
		float m_visualRadius;
		// Cloud:
		Uint3 m_volumetricDensityResolution;
		float m_volumetricDensityRayStepLength;
		float m_volumetricDensityAbsorption;
		Float3 m_volumetricScattering;
		bool m_renderVolumetricLight;
		Uint3 m_volumetricLightingResolution;
		// Water:
		float m_waterSurfaceDensity;
		float m_waterIndexOfRefraction;
		Float3 m_waterAbsorption;
		uint32_t m_waterMaxStepCount;
		uint32_t m_waterMaxRefinementStepCount;
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
			// Visuals:
			Gui::SeparatorText("Particle Visuals");
			if (emberCore::Gui::Selectable("0 - Particles", m_renderMode == fluidDynamics::SphFluid3dGpu::RenderMode::particles))
				m_renderMode = fluidDynamics::SphFluid3dGpu::RenderMode::particles;
			if (emberCore::Gui::Selectable("1 - Cloud", m_renderMode == fluidDynamics::SphFluid3dGpu::RenderMode::cloud))
				m_renderMode = fluidDynamics::SphFluid3dGpu::RenderMode::cloud;
			if (emberCore::Gui::Selectable("2 - Water", m_renderMode == fluidDynamics::SphFluid3dGpu::RenderMode::water))
				m_renderMode = fluidDynamics::SphFluid3dGpu::RenderMode::water;
			
			switch (m_renderMode)
			{
				case fluidDynamics::SphFluid3dGpu::RenderMode::particles:
				{
					Gui::DragInt("Color Mode:", &m_colorMode);
					Gui::DragFloat("Visual Radius:", &m_visualRadius, 0.1f, 1.0f, "%.8f");
					break;
				}
				case fluidDynamics::SphFluid3dGpu::RenderMode::cloud:
				{
					Gui::DragUint3("Density Resolution:", &m_volumetricDensityResolution);
					m_volumetricDensityResolution = Uint3::Max(Uint3::one, m_volumetricDensityResolution);
					Gui::DragFloat("Ray Step Length:", &m_volumetricDensityRayStepLength, 0.1f, 1.0f, "%.8f");
					Gui::DragFloat("Absorption:", &m_volumetricDensityAbsorption, 0.01f, 0.1f, "%.8f");
					Gui::DragFloat3("Scattering:", &m_volumetricScattering, 0.01f, 0.1f, "%.8f");
					Gui::SeparatorText("Lighting");
					Gui::Checkbox("Render Volumetric Light:", &m_renderVolumetricLight);
					if (m_renderVolumetricLight)
					{
						Gui::DragUint3("Resolution:", &m_volumetricLightingResolution);
						m_volumetricLightingResolution = Uint3::Max(Uint3::one, m_volumetricLightingResolution);
					}
					break;
				}
				case fluidDynamics::SphFluid3dGpu::RenderMode::water:
				{
					Gui::DragFloat("Surface Density:", &m_waterSurfaceDensity, 0.1f, 1.0f, "%.8f");
					Gui::DragFloat("Index Of Refraction:", &m_waterIndexOfRefraction, 0.01f, 0.1f, "%.8f");
					Gui::DragFloat3("Absorption:", &m_waterAbsorption, 0.0001f, 0.001f, "%.8f");
					Gui::DragUint("Max Step Count:", &m_waterMaxStepCount);
					Gui::DragUint("Max Refinement Step Count:", &m_waterMaxRefinementStepCount);
					break;
				}
				default:
					break;
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
			m_renderMode = m_pScript->GetRenderMode();
			// Particles:
			m_colorMode = m_pScript->GetColorMode();
			m_visualRadius = m_pScript->GetVisualRadius();
			// Cloud:
			m_volumetricDensityResolution = m_pScript->GetVolumetricDensityResolution();
			m_volumetricDensityRayStepLength = m_pScript->GetVolumetricDensityRayStepLength();
			m_volumetricDensityAbsorption = m_pScript->GetVolumetricDensityAbsorption();
			m_volumetricScattering = m_pScript->GetVolumetricScattering();
			m_renderVolumetricLight = m_pScript->GetRenderVolumetricLight();
			m_volumetricLightingResolution = m_pScript->GetVolumetricLightingResolution();
			// Water:
			m_waterSurfaceDensity = m_pScript->GetWaterSurfaceDensity();
			m_waterIndexOfRefraction = m_pScript->GetWaterIndexOfRefraction();
			m_waterAbsorption = m_pScript->GetWaterAbsorption();
			m_waterMaxStepCount = m_pScript->GetWaterMaxStepCount();
			m_waterMaxRefinementStepCount = m_pScript->GetWaterMaxRefinementStepCount();
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
			m_pScript->SetRenderMode(m_renderMode);
			// Particles:
			m_pScript->SetColorMode(m_colorMode);
			m_pScript->SetVisualRadius(m_visualRadius);
			// Cloud:
			m_pScript->SetVolumetricDensityResolution(m_volumetricDensityResolution);
			m_pScript->SetVolumetricDensityRayStepLength(m_volumetricDensityRayStepLength);
			m_pScript->SetVolumetricDensityAbsorption(m_volumetricDensityAbsorption);
			m_pScript->SetVolumetricScattering(m_volumetricScattering);
			m_pScript->SetRenderVolumetricLight(m_renderVolumetricLight);
			m_pScript->SetVolumetricLightingResolution(m_volumetricLightingResolution);
			// Water:
			m_pScript->SetWaterSurfaceDensity(m_waterSurfaceDensity);
			m_pScript->SetWaterIndexOfRefraction(m_waterIndexOfRefraction);
			m_pScript->SetWaterAbsorption(m_waterAbsorption);
			m_pScript->SetWaterMaxStepCount(m_waterMaxStepCount);
			m_pScript->SetWaterMaxRefinementStepCount(m_waterMaxRefinementStepCount);
		}
	};
}