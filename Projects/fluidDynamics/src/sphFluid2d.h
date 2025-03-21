#ifndef __INCLUDE_GUARD_sphFluid2d_h__
#define __INCLUDE_GUARD_sphFluid2d_h__
#include "emberEngine.h"



namespace emberEngine
{
	class SphFluid2d : public Component
	{
	private: // Members:
		uint32_t m_particleCount;
		std::vector<Float2> m_positions;
		bool m_isRunning;
		Bounds2d m_fluidBounds;
		float m_particleRadius;
		Mesh* pQuad;
		Material* pParticleMaterial;

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
		void Reset();
	};
}



#endif // __INCLUDE_GUARD_sphFluid2d_h__