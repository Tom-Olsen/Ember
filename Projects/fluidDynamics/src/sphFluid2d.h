#ifndef __INCLUDE_GUARD_sphFluid2d_h__
#define __INCLUDE_GUARD_sphFluid2d_h__
#include "emberEngine.h"



namespace emberEngine
{
	class SphFluid2d : public Component
	{
	private: // Members:
		bool m_isRunning;

		uint32_t m_particleCount;
		std::vector<Float2> m_positions;
		std::vector<Float2> m_velocities;
		std::vector<float> m_densities;

		float m_particleRadius;
		float m_mass;
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
		void SetParticleRadius(float radius);

		// Getters:
		uint32_t GetParticleCount() const;
		Bounds2d GetFluidBounds() const;
		float GetParticleRadius() const;

		// Overrides:
		void Start() override;
		void Update() override;
		const std::string ToString() const override;

	private:
		void SimulationStep();
		void Reset();
		float SmoothingKernel(float distance);
		float SmoothingKernelGradient(float distance);
		float Pressure(float density);
		float Density(int particleIndex);
		Float2 ComputePressureForce(int particleIndex);
	};
}



#endif // __INCLUDE_GUARD_sphFluid2d_h__