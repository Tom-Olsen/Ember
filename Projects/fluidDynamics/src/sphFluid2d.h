#ifndef __INCLUDE_GUARD_sphFluid2d_h__
#define __INCLUDE_GUARD_sphFluid2d_h__
#include "emberEngine.h"



namespace emberEngine
{
	class SphFluid2d : public Component
	{
	private: // Members:
		bool m_isRunning;
		uint32_t m_timeStep;

		uint32_t m_particleCount;
		std::vector<Float2> m_positions;
		std::vector<Float2> m_velocities;
		std::vector<float> m_densities;
		std::vector<Float2> m_forceDensities;

		float m_effectRadius;
		float m_visualRadius;
		float m_mass;
		float m_restitution;
		float m_targetDensity;
		float m_pressureMultiplyer;
		float m_gravity;

		Bounds2d m_fluidBounds;
		Mesh* m_pQuad;
		Material* m_pParticleMaterial;

	public: // Methods:
		SphFluid2d();
		~SphFluid2d();

		// Setters:
		void SetParticleCount(uint32_t particleCount);
		void SetFluidBounds(const Bounds2d& bounds);
		void SetEffectRadius(float effectRadius);
		void SetVisualRadius(float visualRadius);

		// Getters:
		uint32_t GetParticleCount() const;
		Bounds2d GetFluidBounds() const;
		float GetEffectRadius() const;
		float GetVisualRadius() const;

		// Overrides:
		void Start() override;
		void Update() override;
		void FixedUpdate() override;
		const std::string ToString() const override;

	private:
		// Management:
		void Reset();

		// Physics:
		float Density(int particleIndex);
		Float2 PressureForceDensity(int particleIndex);
		void BoundaryCollisions(Float2& position, Float2& velocity);
	};
}



#endif // __INCLUDE_GUARD_sphFluid2d_h__