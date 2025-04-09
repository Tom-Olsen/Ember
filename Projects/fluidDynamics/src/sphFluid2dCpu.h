#ifndef __INCLUDE_GUARD_sphFluid2dCpu_h__
#define __INCLUDE_GUARD_sphFluid2dCpu_h__
#include "emberEngine.h"
#include <array>
#include <memory>



namespace emberEngine
{
	// Forward decleration:
	struct SphFluid2dCpuEditorWindow;
	struct HashGrid2d;



	class SphFluid2dCpu : public Component
	{
	private: // Members:
		// Management:
		bool m_isRunning;
		float m_timeScale;
		bool m_useGridOptimization;
		uint32_t m_timeStep;
		std::unique_ptr<HashGrid2d> m_pGrid;
		static const std::array<Int2, 9> s_offsets;

		// Data:
		int m_particleCount;
		std::vector<Float2> m_positions;
		std::vector<Float2> m_velocities;
		std::vector<float> m_densities;
		std::vector<Float2> m_normals;
		std::vector<float> m_curvatures;
		std::vector<Float2> m_forceDensities;

		// Runge Kutta fields:
		std::vector<Float2> m_kp1;
		std::vector<Float2> m_kv1;
		std::vector<Float2> m_kp2;
		std::vector<Float2> m_kv2;
		std::vector<Float2> m_tempPositions;
		std::vector<Float2> m_tempVelocities;

		// Physics:
		float m_effectRadius;
		float m_mass;
		float m_viscosity;
		float m_surfaceTension;
		float m_collisionDampening;
		float m_targetDensity;
		float m_pressureMultiplier;
		float m_gravity;
		float m_maxVelocity;

		// User Interaction/Boundaries:
		int m_attractorState;
		Float2 m_attractorPoint;
		float m_attractorRadius;
		float m_attractorStrength;
		Bounds m_fluidBounds;

		// Visuals:
		float m_initialDistributionRadius;
		float m_visualRadius;
		std::unique_ptr<Mesh> m_pParticleMesh;
		std::unique_ptr<Mesh> m_pRingMesh;
		Material* m_pParticleMaterial;
		std::unique_ptr<SphFluid2dCpuEditorWindow> editorWindow;

	public: // Methods:
		SphFluid2dCpu();
		~SphFluid2dCpu();

		// Physics update:
		void Reset();
		void FixedUpdate() override;
		void TimeStepLeapFrog(float deltaT);
		void TimeStepVelocityVerlet(float deltaT);
		void TimeStepRungeKutta2(float deltaT);

		// Setters:
		void SetIsRunning(bool isRunning);
		void SetTimeScale(float timeScale);
		void SetUseGridOptimization(bool useGridOptimization);
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
		float GetAttractorRadius() const;
		float GetAttractorStrength() const;
		Bounds GetFluidBounds() const;
		float GetInitialDistributionRadius() const;
		float GetVisualRadius() const;

		// Overrides:
		void Start() override;
		void Update() override;
		const std::string ToString() const override;

	private:
		// Physics:
		float Density(int particleIndex, const std::vector<Float2>& positions);
		Float2 Normal(int particleIndex, const std::vector<Float2>& positions, const std::vector<float>& densities);
		float Curvature(int particleIndex, const std::vector<Float2>& positions, const std::vector<Float2>& normals, const std::vector<float>& densities);
		Float2 PressureForceDensity(int particleIndex, const std::vector<Float2>& positions, const std::vector<float>& densities);
		Float2 ViscosityForceDensity(int particleIndex, const std::vector<Float2>& positions, const std::vector<Float2>& velocities, const std::vector<float>& densities);
		Float2 SurfaceTensionForceDensity(int particleIndex, const std::vector<Float2>& normals, const std::vector<float>& curvatures);
		Float2 ExternalForceDensity(int particleIndex, const std::vector<Float2>& positions, const std::vector<float>& densities);
		Float2 GravityForceDensity(int particleIndex, const std::vector<float>& densities);
		void BoundaryCollisions(Float2& position, Float2& velocity);
	};
}



#endif // __INCLUDE_GUARD_sphFluid2dCpu_h__