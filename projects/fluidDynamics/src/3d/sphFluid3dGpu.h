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
	private: // Members:
		// Management:
		bool m_isRunning = false;
		bool m_reset = false;
		float m_timeScale = 1.0f;           // increases the number of timesteps per fixedDeltaTime physics timestep. E.g. m_timeScale = 2.0f, means 2 full simulation steps per fixed update.
        float m_physicsTimeScale = 1.25f;   // gets multiplied to certain values physical quantities to make the physics itself interact faster.
		uint32_t m_timeStep = 0;
		int m_particleCount = -1;
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
		int m_colorMode;
		float m_initialDistributionRadius;
		float m_visualRadius;
		bool m_renderParticles;
		bool m_renderVolumetricDensity;
		float m_volumetricDensityRayStepLength;
		float m_volumetricDensityAbsorption;
		Float4 m_volumetricDensityColorLow;
		Float4 m_volumetricDensityColorHigh;
		float m_volumetricVoxelScale;
		Mesh m_particleMesh;
		Mesh m_attractorSphereMesh;
        Mesh m_volumetricDensityCube;
		Material m_particleMaterial;
        Material m_volumetricDensityMaterial;
		ShaderProperties m_shaderProperties;

		// Editor Window:
		std::unique_ptr<emberEditor::SphFluid3dGpuEditorWindow> editorWindow;

	public: // Methods:
		SphFluid3dGpu();
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
        // Particle visuals:
		void SetRenderParticles(bool renderParticles);
		void SetColorMode(int colorMode);
		void SetVisualRadius(float visualRadius);
        // Volumetric visuals:
		void SetRenderVolumetricDensity(bool renderVolumetricDensity);
		void SetVolumetricDensityRayStepLength(float volumetricDensityRayStepLength);
		void SetVolumetricDensityAbsorption(float volumetricDensityAbsorption);
		void SetVolumetricDensityColorLow(const Float4& volumetricDensityColorLow);
		void SetVolumetricDensityColorHigh(const Float4& volumetricDensityColorHigh);
		void SetVolumetricVoxelScale(float volumetricVoxelScale);

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
        // Particle visuals:
		bool GetRenderParticles() const;
		int GetColorMode() const;
		float GetVisualRadius() const;
        // Volumetric visuals:
		bool GetRenderVolumetricDensity() const;
		float GetVolumetricDensityRayStepLength() const;
		float GetVolumetricDensityAbsorption() const;
		Float4 GetVolumetricDensityColorLow() const;
		Float4 GetVolumetricDensityColorHigh() const;
		float GetVolumetricVoxelScale() const;

		// Debugging:
		void Print();

	private: // Methods:
		void RecordReset();
	};
}