#include "sphFluid2d.h"



namespace emberEngine
{
	// Constructor/Destructor:
	SphFluid2d::SphFluid2d()
	{
		m_isRunning = false;
		m_particleCount = 1;

		m_particleRadius = 0.05f;
		m_mass = 1.0f;
		m_targetDensity = 1.0f;
		m_pressureMultiplyer = 1.0f;
		m_gravity = 1.0f;

		m_fluidBounds = Bounds2d(Float2::zero, Float2(16.0f / 9.0f, 1.0f));
		m_pQuad = MeshManager::GetMesh("unitQuad");
		m_pParticleMaterial = MaterialManager::GetMaterial("particleMaterial");
	}
	SphFluid2d::~SphFluid2d()
	{

	}



	// Setters:
	void SphFluid2d::SetParticleCount(uint32_t particleCount)
	{
		m_particleCount = particleCount;
		Reset();
	}
	void SphFluid2d::SetFluidBounds(const Bounds2d& bounds)
	{
		m_fluidBounds = bounds;
	}
	void SphFluid2d::SetParticleRadius(float radius)
	{
		m_particleRadius = radius;
	}



	// Getters:
	uint32_t SphFluid2d::GetParticleCount() const
	{
		return m_particleCount;
	}
	Bounds2d SphFluid2d::GetFluidBounds() const
	{
		return m_fluidBounds;
	}
	float SphFluid2d::GetParticleRadius() const
	{
		return m_particleRadius;
	}



	// Overrides:
	void SphFluid2d::Start()
	{

	}
	void SphFluid2d::Update()
	{
		if (EventSystem::KeyDown(SDLK_SPACE))
			m_isRunning = !m_isRunning;

		Float4x4 localToWorld = GetTransform()->GetLocalToWorldMatrix();
		Graphics::DrawBounds(localToWorld, m_fluidBounds, 0.01f, Float4::white, false, false);
		for (uint32_t i = 0; i < m_particleCount; i++)
		{
			Float4x4 matrix = localToWorld * Float4x4::Translate(Float3(m_positions[i], 0)) * Float4x4::Scale(m_particleRadius);
			ShaderProperties* pShaderProperties = Graphics::DrawMesh(m_pQuad, m_pParticleMaterial, matrix, false, false);
			pShaderProperties->SetValue("SurfaceProperties", "diffuseColor", Float4::blue);
		}
	}
	const std::string SphFluid2d::ToString() const
	{
		return "SphFluid2d";
	}



	// Private methods:
	void SphFluid2d::SimulationStep()
	{
		for (int i = 0; i < m_particleCount; i++)
		{
			m_densities[i] = Density(i);
			Float2 pressureForce = ComputePressureForce(i);
			Float2 acceleration = pressureForce / m_densities[i];
			m_velocities[i] += acceleration * Timer::GetDeltaTime() + Float2::back * m_gravity;
		}

		for (int i = 0; i < m_particleCount; i++)
		{
			m_positions[i] += m_velocities[i] * Timer::GetDeltaTime();
			//ResolveCollisions(m_positions[i], m_velocities[i]);
		}
	}
	void SphFluid2d::Reset()
	{
		m_positions.resize(m_particleCount);
		m_velocities.resize(m_particleCount);
		m_densities.resize(m_particleCount);

		Float2 min = m_fluidBounds.GetMin();
		Float2 max = m_fluidBounds.GetMax();
		float border = 0.05f * math::Min(m_fluidBounds.GetSize().x, m_fluidBounds.GetSize().y);
		for (uint32_t i = 0; i < m_particleCount; i++)
		{
			m_positions[i].x = math::Random::Uniform(min.x + border, max.x - border);
			m_positions[i].y = math::Random::Uniform(min.y + border, max.y - border);
			m_velocities[i].x = math::Random::Uniform01() - 0.5f;
			m_velocities[i].y = math::Random::Uniform01() - 0.5f;
		}
		for (uint32_t i = 0; i < m_particleCount; i++)
			m_densities[i] = Density(i);
	}
	float SphFluid2d::SmoothingKernel(float distance)
	{
		float volume = math::pi * math::Pow(m_particleRadius, 4) / 6.0f;
		if (distance >= m_particleRadius)
			return 0.0f;
		else
		{
			float x = m_particleRadius - distance;
			return x * x / volume;
		}
	}
	float SphFluid2d::SmoothingKernelGradient(float distance)
	{
		if (distance >= m_particleRadius)
			return 0.0f;
		return 12.0f * (m_particleRadius - distance) / (math::pi * math::Pow(m_particleRadius, 4));
	}
	float SphFluid2d::Pressure(float density)
	{
		float densityError = density - m_targetDensity;
		return densityError * m_pressureMultiplyer;
	}
	float SphFluid2d::Density(int particleIndex)
	{
		float density = 0;
		for (int i = 0; i < m_particleCount; i++)
		{
			if (i == particleIndex)
				continue;
			float distance = (m_positions[i] - m_positions[particleIndex]).Length();
			if (distance < m_particleRadius)
				density += m_mass * SmoothingKernel(distance);
		}
		return density;
	}
	Float2 SphFluid2d::ComputePressureForce(int particleIndex)
	{
		Float2 pressureForce = Float2::zero;
		float particlePressure = Pressure(m_densities[particleIndex]);
		for (int i = 0; i < m_particleCount; i++)
		{
			float distance = (m_positions[particleIndex] - m_positions[i]).Length();
			if (distance < m_particleRadius)
			{
				Float2 direction;
				if (distance < 1e-8f)
					direction = math::Random::UniformDirection2();
				else
					direction = (m_positions[particleIndex] - m_positions[i]) / distance;
				float otherParticlePressure = Pressure(m_densities[i]);
				float sharedPressure = 0.5f * (particlePressure + otherParticlePressure);
				pressureForce += direction * m_mass * sharedPressure * SmoothingKernelGradient(distance) / m_densities[i];
			}
		}
		return pressureForce;
	}

	// General formulas that can be adapted for any property (e.g pressure):
	//float SphFluid2d::ComputeProperty(Float2 position)
	//{
	//	float property = 0;
	//	for (int i = 0; i < m_particleCount; i++)
	//	{
	//		float distance = (position - m_particleCount[i]).Length();
	//		if (distance < m_particleRadius)
	//			property += m_mass * property[i] * SmoothingKernel(distance) / m_densities[i];
	//	}
	//  return property;
	//}
	//Float2 SphFluid2d::ComputePropertyGradient(Float2 position)
	//{
	//	Float2 propertyGradient = Float2::zero;
	//	for (int i = 0; i < m_particleCount; i++)
	//	{
	//		float distance = (position - m_particleCount[i]).Length();
	//		if (distance < m_particleRadius)
	//		{
	//			Float2 direction = (position - m_particleCount[i]) / distance;
	//			propertyGradient += direction * m_mass * property[i] * SmoothingKernelGradient(distance) / m_densities[i];
	//		}
	//	}
	//	return propertyGradient;
	//}
}