#ifndef __INCLUDE_GUARD_sphFluid2dGpu_h__
#define __INCLUDE_GUARD_sphFluid2dGpu_h__
#include "emberEngine.h"
#include <array>
#include <memory>



namespace emberEngine
{
	// Forward decleration:
	struct SphFluid2dGpuEditorWindow;



	class SphFluid2dGpu : public Component
	{
	private: // Members:
		// Management:
		bool m_isRunning;
		float m_timeScale;
		bool m_useGridOptimization;
		uint32_t m_timeStep;

		// Data:
		uint32_t m_particleCount;
		//std::vector<Float2> m_positions;
		//std::vector<Float2> m_velocities;
		//std::vector<float> m_densities;
		//std::vector<Float2> m_normals;
		//std::vector<float> m_curvatures;
		//std::vector<Float2> m_forceDensities;

		// Runge Kutta fields:
		//std::vector<Float2> m_kp1;
		//std::vector<Float2> m_kp2;
		//std::vector<Float2> m_kv1;
		//std::vector<Float2> m_kv2;
		//std::vector<Float2> m_tempPositions;
		//std::vector<Float2> m_tempVelocities;

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
		float m_visualRadius;
		Mesh* m_pParticleMesh;
		std::unique_ptr<Mesh> m_pRingMesh;
		Material* m_pParticleMaterial;
		std::unique_ptr<SphFluid2dGpuEditorWindow> editorWindow;

	public: // Methods:
		SphFluid2dGpu();
		~SphFluid2dGpu();
		
		// Physics update:
		void Reset();
		void FixedUpdate() override;

		// Setters:
		void SetIsRunning(bool isRunning);
		void SetTimeScale(float timeScale);
		void SetUseGridOptimization(bool useGridOptimization);
		void SetParticleCount(uint32_t particleCount);
		void SetFluidBounds(const Bounds& bounds);
		void SetEffectRadius(float effectRadius);
		void SetVisualRadius(float visualRadius);
		void SetMass(float mass);
		void SetViscosity(float viscosity);
		void SetSurfaceTension(float surfaceTension);
		void SetCollisionDampening(float mass);
		void SetTargetDensity(float targetDensity);
		void SetPressureMultiplier(float pressureMultiplier);
		void SetGravity(float gravity);
		void SetMaxVelocity(float maxVelocity);
		void SetAttractorRadius(float attractorRadius);
		void SetAttractorStrength(float attractorStrength);

		// Getters:
		bool GetIsRunning() const;
		float GetTimeScale() const;
		bool GetUseGridOptimization() const;
		uint32_t GetTimeStep() const;
		uint32_t GetParticleCount() const;
		Bounds GetFluidBounds() const;
		float GetEffectRadius() const;
		float GetVisualRadius() const;
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

		// Overrides:
		void Start() override;
		void Update() override;
		const std::string ToString() const override;
		
	private:
		// Physics:
		void Density();
		void Normal();
		void Curvature();
		void PressureForceDensity();
		void ViscosityForceDensity();
		void SurfaceTensionForceDensity();
		void GravityForceDensity();
		void ExternalForceDensity();
		void BoundaryCollisions();
	};
}



#endif // __INCLUDE_GUARD_sphFluid2dGpu_h__