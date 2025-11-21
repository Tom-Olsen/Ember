#pragma once
#include "emberEngine.h"
#include "hashGrid2d.h"
#include "sphFluid2dCpuSolver.h"
using namespace emberEngine;



// Forward decleration:
namespace emberEditor
{
	struct SphFluid2dCpuEditorWindow;
}



namespace fluidDynamics
{
	class SphFluid2dCpu : public Component
	{
	private: // Members:
		// Management:
		bool m_isRunning;
		bool m_reset;
		float m_timeScale;
		bool m_useHashGridOptimization;
		uint32_t m_timeStep;
		int m_particleCount;

		// Settings:
		SphFluid2dCpuSolver::Settings m_settings;
		SphFluid2dCpuSolver::Attractor m_attractor;

		// Data:
		HashGrid2d m_hashGrid;
		SphFluid2dCpuSolver::Data m_data;
		SphFluid2dCpuSolver::RungeKutta m_rungeKutta;

		// Visuals:
		int m_colorMode;
		float m_initialDistributionRadius;
		float m_visualRadius;
		Mesh m_particleMesh;
		Mesh m_ringMesh;
		Material m_particleMaterial;

		// Editor Window:
		std::unique_ptr<emberEditor::SphFluid2dCpuEditorWindow> editorWindow;

	public: // Methods:
		SphFluid2dCpu();
		~SphFluid2dCpu();

		// Physics update:
		void Reset();
		void FixedUpdate() override;

		// Setters:
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
		void SetAttractorRadius(float attractorRadius);
		void SetAttractorStrength(float attractorStrength);
		void SetFluidBounds(const Bounds& bounds);
		void SetColorMode(int colorMode);
		void SetInitialDistributionRadius(float initialDistributionRadius);
		void SetVisualRadius(float visualRadius);
		
		// Getters:
		bool GetIsRunning() const;
		float GetTimeScale() const;
		bool GetUseHashGridOptimization() const;
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
		float GetAttractorRadius() const;
		float GetAttractorStrength() const;
		Bounds GetFluidBounds() const;
		int GetColorMode() const;
		float GetInitialDistributionRadius() const;
		float GetVisualRadius() const;

		// Overrides:
		void Update() override;
	};
}