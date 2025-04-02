#ifndef __INCLUDE_GUARD_sphFluid2d_h__
#define __INCLUDE_GUARD_sphFluid2d_h__
#include "emberEngine.h"
#include <array>
#include <memory>



namespace emberEngine
{
	// Forward decleration:
	struct SphFluid2dEditorWindow;
	struct InfinitGrid2d;



	class SphFluid2d : public Component
	{
	private: // Members:
		// Management:
		bool m_isRunning;
		bool m_useGridOptimization;
		uint32_t m_timeStep;
		std::unique_ptr<InfinitGrid2d> m_pGrid;
		static const std::array<Int2, 9> s_offsets;

		// Data:
		uint32_t m_particleCount;
		std::vector<Float2> m_positions;
		std::vector<Float2> m_velocities;
		std::vector<float> m_densities;
		std::vector<Float2> m_forceDensities;

		// Physics:
		float m_effectRadius;
		float m_mass;
		float m_viscosity;
		float m_collisionDampening;
		float m_targetDensity;
		float m_pressureMultiplier;
		float m_gravity;

		// User Interaction/Boundaries:
		float m_attractorRadius;
		float m_attractorStrength;
		Bounds2d m_fluidBounds;

		// Visuals:
		float m_visualRadius;
		Mesh* m_pQuad;
		Material* m_pParticleMaterial;



		std::unique_ptr<SphFluid2dEditorWindow> editorWindow;

	public: // Methods:
		SphFluid2d();
		~SphFluid2d();
		void Reset();

		// Setters:
		void SetIsRunning(bool isRunning);
		void SetUseGridOptimization(bool useGridOptimization);
		void SetParticleCount(uint32_t particleCount);
		void SetFluidBounds(const Bounds2d& bounds);
		void SetEffectRadius(float effectRadius);
		void SetVisualRadius(float visualRadius);
		void SetMass(float mass);
		void SetViscosity(float viscosity);
		void SetCollisionDampening(float mass);
		void SetTargetDensity(float targetDensity);
		void SetPressureMultiplier(float pressureMultiplier);
		void SetGravity(float gravity);

		// Getters:
		bool GetIsRunning() const;
		bool GetUseGridOptimization() const;
		uint32_t GetTimeStep() const;
		uint32_t GetParticleCount() const;
		Bounds2d GetFluidBounds() const;
		float GetEffectRadius() const;
		float GetVisualRadius() const;
		float GetMass() const;
		float GetViscosity() const;
		float GetCollisionDampening() const;
		float GetTargetDensity() const;
		float GetPressureMultiplier() const;
		float GetGravity() const;

		// Overrides:
		void Start() override;
		void Update() override;
		void FixedUpdate() override;
		const std::string ToString() const override;

	private:
		// Physics:
		float Density(int particleIndex);
		Float2 PressureForceDensity(int particleIndex);
		Float2 ViscosityForceDensity(int particleIndex);
		Float2 GravityForceDensity(int particleIndex);
		void BoundaryCollisions(Float2& position, Float2& velocity);
	};
}



#endif // __INCLUDE_GUARD_sphFluid2d_h__