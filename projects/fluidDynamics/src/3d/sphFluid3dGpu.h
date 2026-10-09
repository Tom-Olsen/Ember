#pragma once
#include "emberEngine.h"
#include "sphFluid3dGpuSolver.h"
using namespace emberCore;
using namespace emberEcs;



// Forward decleration:
namespace emberEditor
{
	struct SphFluid3dGpuEditorWindow;
}



namespace fluidDynamics
{
	class SphFluid3dGpu : public Component
	{
	public: // Enums:
		enum class RenderMode
		{
			particles,
			cloud,
			water
		};

	private: // Members:
		// Management:
		bool m_isRunning = false;
		bool m_reset = false;
		bool m_pendingVolumetricDensityResolutionChange = false;
		bool m_pendingVolumetricLightingResolutionChange = false;
		float m_timeScale = 1.0f;			// increases the number of timesteps per fixedDeltaTime physics timestep. E.g. m_timeScale = 2.0f, means 2 full simulation steps per fixed update.
		float m_physicsTimeScale = 1.25f;	// gets multiplied to certain values physical quantities to make the physics itself interact faster.
		uint32_t m_timeStep = 0;
		int m_particleCount = -1;
		float m_initialDistributionRadius;
		bool m_forceSetters = false;

		// Settings:
		SphFluid3dGpuSolver::Settings m_settings;
		SphFluid3dGpuSolver::Attractor m_attractor;

		// Data:
		SphFluid3dGpuSolver::ScratchData m_scratchData;
		SphFluid3dGpuSolver::ComputeShaders m_computeShaders;
		SphFluid3dGpuSolver::TripleData m_tripleData;
		PhysicsTripleBufferState m_tripleBufferState;
		uint64_t m_pendingResetSessionID = Compute::Physics::invalidPhysicsSessionID;

		// Visuals:
		RenderMode m_renderMode = RenderMode::water;
		bool m_pendingRenderRefresh = false;
		uint32_t m_lastRenderFrameIndex = PhysicsTripleBufferState::invalidFrameIndex;
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
		float m_waterStepLength;
		uint32_t m_waterMaxStepCount;
		uint32_t m_waterMaxRefinementStepCount;

        // Internal:
		Mesh m_particleMesh;
		Mesh m_attractorSphereMesh;
		Mesh m_volumetricDensityCube;
		ForwardMaterial m_particleMaterial;
		ForwardMaterial m_volumeRaycastMaterial;
		ComputeShader m_waterComputeShader;
		CallProperties m_callProperties;
		Texture m_environmentMap;

		// Editor Window:
		std::unique_ptr<emberEditor::SphFluid3dGpuEditorWindow> editorWindow;

	public: // Methods:
		SphFluid3dGpu(Texture environmentMap);
		~SphFluid3dGpu();

		// Overrides:
		void Start() override;
		void FixedUpdate() override;
		void Update() override;

		// Setters:
		// Management:
		void Reset();
		void SetIsRunning(bool isRunning);
		void SetUseHashGridOptimization(bool useGridOptimization);
		void SetFluidBounds(const RotatedBounds& bounds);
		// Time:
		void SetTimeScale(float timeScale);
		void SetPhysicsTimeScale(float physicsTimeScale);
		// Fluid:
		void SetParticleCount(int particleCount);
		void SetInitialDistributionRadius(float initialDistributionRadius);
		void SetEffectRadius(float effectRadius);
		void SetMass(float mass);
		void SetViscosity(float viscosity);
		void SetSurfaceTension(float surfaceTension);
		void SetCollisionDampening(float collisionDampening);
		void SetTargetDensity(float targetDensity);
		void SetPressureMultiplier(float pressureMultiplier);
		void SetNearPressureRatio(float nearPressureRatio);
		void SetMaxVelocity(float maxVelocity);
		// Forces:
		void SetGravity(float gravity);
		void SetAttractorRadius(float attractorRadius);
		void SetAttractorStrength(float attractorStrength);
		void SetAttractorState(int attractorState);
		void SetAttractorPoint(const Float3& attractorPoint);
		// Visuals:
		void SetRenderMode(RenderMode renderMode);
		// Particles:
		void SetColorMode(int colorMode);
		void SetVisualRadius(float visualRadius);
		// Cloud:
		void SetVolumetricDensityResolution(const Uint3& volumetricDensityResolution);
		void SetVolumetricDensityRayStepLength(float volumetricDensityRayStepLength);
		void SetVolumetricDensityAbsorption(float volumetricDensityAbsorption);
		void SetVolumetricScattering(const Float3& volumetricScattering);
		void SetRenderVolumetricLight(bool renderVolumetricLight);
		void SetVolumetricLightingResolution(const Uint3& volumetricLightingResolution);
		// Water:
		void SetWaterSurfaceDensity(float waterSurfaceDensity);
		void SetWaterIndexOfRefraction(float waterIndexOfRefraction);
		void SetWaterAbsorption(const Float3& waterAbsorption);
		void SetWaterStepLength(float waterStepLength);
		void SetWaterMaxStepCount(uint32_t waterMaxStepCount);
		void SetWaterMaxRefinementStepCount(uint32_t waterMaxRefinementStepCount);

		// Getters:
		// Management:
		bool GetIsRunning() const;
		bool GetUseGridOptimization() const;
		RotatedBounds GetFluidBounds() const;
		// Time:
		float GetTimeScale() const;
		float GetPhysicsTimeScale() const;
		uint32_t GetTimeStep() const;
		// Fluid:
		int GetParticleCount() const;
		float GetInitialDistributionRadius() const;
		float GetEffectRadius() const;
		float GetMass() const;
		float GetViscosity() const;
		float GetSurfaceTension() const;
		float GetCollisionDampening() const;
		float GetTargetDensity() const;
		float GetPressureMultiplier() const;
		float GetNearPressureRatio() const;
		float GetMaxVelocity() const;
		// Forces:
		float GetGravity() const;
		float GetAttractorRadius() const;
		float GetAttractorStrength() const;
		int GetAttractorState() const;
		Float3 GetAttractorPoint() const;
		// Visuals:
		RenderMode GetRenderMode() const;
		// Particles:
		int GetColorMode() const;
		float GetVisualRadius() const;
		// Cloud:
		bool GetRenderVolumetricDensity() const;
		bool GetRenderScreenSpaceFluid() const;
		Uint3 GetVolumetricDensityResolution() const;
		float GetVolumetricDensityRayStepLength() const;
		float GetVolumetricDensityAbsorption() const;
		Float3 GetVolumetricScattering() const;
		bool GetRenderVolumetricLight() const;
		Uint3 GetVolumetricLightingResolution() const;
		// Water:
		float GetWaterSurfaceDensity() const;
		float GetWaterIndexOfRefraction() const;
		Float3 GetWaterAbsorption() const;
		float GetWaterStepLength() const;
		uint32_t GetWaterMaxStepCount() const;
		uint32_t GetWaterMaxRefinementStepCount() const;

		// Debugging:
		void Print();

	private: // Methods:
		void RecordReset();
		void RefreshRenderData();
		void RecordVolumetricRenderData(SphFluid3dGpuSolver::ComputeShaders& computeShaders, uint32_t dataIndex);
		bool TryGetDirectionalLightBounds(RotatedBounds& lightBounds);
	};
}