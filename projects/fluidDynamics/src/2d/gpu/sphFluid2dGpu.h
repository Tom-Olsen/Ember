#pragma once
#include "emberEngine.h"
#include "sphFluid2dGpuSolver.h"
using namespace emberEngine;



// Forward decleration:
namespace emberEditor
{
	struct SphFluid2dGpuEditorWindow;
}



namespace fluidDynamics
{
	class SphFluid2dGpu : public Component
	{
	private: // Members:
		// Management:
		bool m_isRunning;
		bool m_reset;
		float m_timeScale;
		uint32_t m_timeStep;
		int m_particleCount;

		// Settings:
		SphFluid2dGpuSolver::Settings m_settings;
		SphFluid2dGpuSolver::Attractor m_attractor;

		// Data:
		SphFluid2dGpuSolver::Data m_data;
		SphFluid2dGpuSolver::RungeKutta m_rungeKutta;
		SphFluid2dGpuSolver::ComputeShaders m_computeShaders;

		// Visuals:
		int m_colorMode;
		float m_initialDistributionRadius;
		float m_visualRadius;
		Mesh m_particleMesh;
		Mesh m_ringMesh;
		Material m_particleMaterial;
		ShaderProperties m_shaderProperties;

		// Editor Window:
		std::unique_ptr<emberEditor::SphFluid2dGpuEditorWindow> editorWindow;

	public: // Methods:
		SphFluid2dGpu();
		~SphFluid2dGpu();

		// Overrides:
		void FixedUpdate() override;
		void Update() override;

		// Setters:
		void Reset();
		void SetIsRunning(bool isRunning);
		void SetTimeScale(float timeScale);
		void SetUseHashGridOptimization(bool useGridOptimization);
		void SetParticleCount(int particleCount);
		void SetEffectRadius(float effectRadius);
		void SetMass(float mass);
		void SetViscosity(float viscosity);
		void SetSurfaceTension(float surfaceTension);
		void SetCollisionDampening(float collisionDampening);
		void SetTargetDensity(float targetDensity);
		void SetPressureMultiplier(float pressureMultiplier);
		void SetGravity(float gravity);
		void SetMaxVelocity(float maxVelocity);
		void SetFluidBounds(const Bounds& bounds);
		void SetAttractorRadius(float attractorRadius);
		void SetAttractorStrength(float attractorStrength);
		void SetAttractorState(int attractorState);
		void SetAttractorPoint(const Float2& attractorPoint);
		void SetColorMode(int colorMode);
		void SetInitialDistributionRadius(float initialDistributionRadius);
		void SetVisualRadius(float visualRadius);

		// Getters:
		bool GetIsRunning() const;
		float GetTimeScale() const;
		bool GetUseGridOptimization() const;
		uint32_t GetTimeStep() const;
		int GetParticleCount() const;
		float GetEffectRadius() const;
		float GetMass() const;
		float GetViscosity() const;
		float GetSurfaceTension() const;
		float GetCollisionDampening() const;
		float GetTargetDensity() const;
		float GetPressureMultiplier() const;
		float GetGravity() const;
		float GetMaxVelocity() const;
		Bounds GetFluidBounds() const;
		float GetAttractorRadius() const;
		float GetAttractorStrength() const;
		int GetColorMode() const;
		float GetInitialDistributionRadius() const;
		float GetVisualRadius() const;
	};
}