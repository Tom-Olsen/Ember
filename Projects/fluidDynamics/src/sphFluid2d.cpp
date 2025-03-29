#include "sphFluid2d.h"
#include "sphFluid2dEditorWindow.h"
#include "smoothingKernals.h"



namespace emberEngine
{
	// Constructor/Destructor:
	SphFluid2d::SphFluid2d()
	{
		m_isRunning = false;
		m_timeStep = 0;
		m_particleCount = 200;

		m_effectRadius = 0.5f;
		m_visualRadius = 0.05f;
		m_mass = 1.0f;
		m_viscosity = 0.0f;
		m_collisionDampening = 0.95f;
		m_targetDensity = 180.0f;
		m_pressureMultiplier = 50.0f;
		m_gravity = 0.0f;

		m_fluidBounds = Bounds2d(Float2::zero, 0.5f * Float2(16.0f / 9.0f, 1.0f));
		m_pQuad = MeshManager::GetMesh("unitQuad");
		m_pParticleMaterial = MaterialManager::GetMaterial("particleMaterial");
		Reset();

		editorWindow = std::make_unique<SphFluid2dEditorWindow>(this);
	}
	SphFluid2d::~SphFluid2d()
	{

	}
	void SphFluid2d::Reset()
	{
		m_timeStep = 0;
		m_positions.resize(m_particleCount);
		m_velocities.resize(m_particleCount);
		m_densities.resize(m_particleCount);
		m_forceDensities.resize(m_particleCount);

		Float2 min = m_fluidBounds.GetMin();
		Float2 max = m_fluidBounds.GetMax();
		float border = 0.05f * math::Min(m_fluidBounds.GetSize().x, m_fluidBounds.GetSize().y);
		for (uint32_t i = 0; i < m_particleCount; i++)
		{
			m_positions[i].x = math::Random::Uniform(min.x + border, max.x - border);
			m_positions[i].y = math::Random::Uniform(min.y + border, max.y - border);
			m_velocities[i].x = math::Random::Uniform01() - 0.5f;
			m_velocities[i].y = math::Random::Uniform01() - 0.5f;
			m_forceDensities[i] = 0;
		}
		for (uint32_t i = 0; i < m_particleCount; i++)
			m_densities[i] = Density(i);
	}



	// Physics update:
	void SphFluid2d::FixedUpdate()
	{
		if (!m_isRunning)
			return;

		// Compute density:
		for (int i = 0; i < m_particleCount; i++)
			m_densities[i] = Density(i);

		// Compute force densities:
		for (int i = 0; i < m_particleCount; i++)
		{
			m_forceDensities[i] = PressureForceDensity(i);
			m_forceDensities[i] += ViscosityForceDensity(i);
			m_forceDensities[i] += GravityForceDensity(i);
			//m_forceDensities[i] += SurfaceTensionForceDensity(i);
			//m_forceDensities[i] += ExternalForceDensity(i);
		}

		// Solve equation of motion ODE for each sph particle with leapfrog solver:
		if (m_timeStep == 0)
		{
			for (int i = 0; i < m_particleCount; i++)
			{
				Float2 acceleration = m_forceDensities[i] / m_densities[i];
				m_velocities[i] += 0.5f * acceleration * Time::GetFixedDeltaTime() / 10.0f;
			}
		}
		for (int i = 0; i < m_particleCount; i++)
		{
			Float2 acceleration = m_forceDensities[i] / m_densities[i];
			m_positions[i] += m_velocities[i] * Time::GetFixedDeltaTime() / 10.0f;
			m_velocities[i] += acceleration * Time::GetFixedDeltaTime() / 10.0f;
			BoundaryCollisions(m_positions[i], m_velocities[i]);
		}

		m_timeStep++;
	}


	// Setters:
	void SphFluid2d::SetIsRunning(bool isRunning)
	{
		m_isRunning = isRunning;
	}
	void SphFluid2d::SetParticleCount(uint32_t particleCount)
	{
		if (m_particleCount != particleCount)
		{
			m_particleCount = particleCount;
			Reset();
		}
	}
	void SphFluid2d::SetFluidBounds(const Bounds2d& bounds)
	{
		m_fluidBounds = bounds;
	}
	void SphFluid2d::SetEffectRadius(float effectRadius)
	{
		m_effectRadius = effectRadius;
	}
	void SphFluid2d::SetVisualRadius(float visualRadius)
	{
		m_visualRadius = visualRadius;
	}
	void SphFluid2d::SetMass(float mass)
	{
		m_mass = mass;
	}
	void SphFluid2d::SetViscosity(float viscosity)
	{
		m_viscosity = viscosity;
	}
	void SphFluid2d::SetCollisionDampening(float collisionDampening)
	{
		m_collisionDampening = collisionDampening;
	}
	void SphFluid2d::SetTargetDensity(float targetDensity)
	{
		m_targetDensity = targetDensity;
	}
	void SphFluid2d::SetPressureMultiplier(float pressureMultiplier)
	{
		m_pressureMultiplier = pressureMultiplier;
	}
	void SphFluid2d::SetGravity(float gravity)
	{
		m_gravity = gravity;
	}



	// Getters:
	bool SphFluid2d::GetIsRunning() const
	{
		return m_isRunning;
	}
	uint32_t SphFluid2d::GetTimeStep() const
	{
		return m_timeStep;
	}
	uint32_t SphFluid2d::GetParticleCount() const
	{
		return m_particleCount;
	}
	Bounds2d SphFluid2d::GetFluidBounds() const
	{
		return m_fluidBounds;
	}
	float SphFluid2d::GetEffectRadius() const
	{
		return m_effectRadius;
	}
	float SphFluid2d::GetVisualRadius() const
	{
		return m_visualRadius;
	}
	float SphFluid2d::GetMass() const
	{
		return m_mass;
	}
	float SphFluid2d::GetViscosity() const
	{
		return m_viscosity;
	}
	float SphFluid2d::GetCollisionDampening() const
	{
		return m_collisionDampening;
	}
	float SphFluid2d::GetTargetDensity() const
	{
		return m_targetDensity;
	}
	float SphFluid2d::GetPressureMultiplier() const
	{
		return m_pressureMultiplier;
	}
	float SphFluid2d::GetGravity() const
	{
		return m_gravity;
	}



	// Overrides:
	void SphFluid2d::Start()
	{

	}
	void SphFluid2d::Update()
	{
		if (EventSystem::KeyDown(SDLK_SPACE))
		{
			m_isRunning = !m_isRunning;
			if (m_isRunning)
				LOG_TRACE("Simulation running.");
			else
				LOG_TRACE("Simulation stopped.");
		}
		if (EventSystem::KeyDown(SDLK_DELETE))
		{
			m_isRunning = false;
			Reset();
			LOG_TRACE("Simulation stopped.");
		}

		Float4x4 localToWorld = GetTransform()->GetLocalToWorldMatrix();
		Graphics::DrawBounds(localToWorld, m_fluidBounds, 0.01f, Float4::white, false, false);
		for (uint32_t i = 0; i < m_particleCount; i++)
		{
			Float4x4 matrix = localToWorld * Float4x4::Translate(Float3(m_positions[i], 0)) * Float4x4::Scale(m_visualRadius);
			ShaderProperties* pShaderProperties = Graphics::DrawMesh(m_pQuad, m_pParticleMaterial, matrix, false, false);
			float t = (m_densities[i] - m_targetDensity) / m_targetDensity;
			float t0 = math::Clamp(t, 0.0f, 1.0f);
			float t1 = math::Clamp(t - 1.0f, 0.0f, 1.0f);
			Float4 colorA = t0 * Float4::white + (1.0f - t0) * Float4::blue;
			Float4 colorB = t0 * Float4::red + (1.0f - t0) * Float4::white;
			Float4 color = (t < 1.0f) ? colorA : colorB;
			pShaderProperties->SetValue("SurfaceProperties", "diffuseColor", color);
		}
	}
	const std::string SphFluid2d::ToString() const
	{
		return "SphFluid2d";
	}



	// Private methods:
	// Physics:
	float SphFluid2d::Density(int particleIndex)
	{
		float density = 0;
		for (int i = 0; i < m_particleCount; i++)
		{
			Float2 offset = m_positions[particleIndex] - m_positions[i];
			float r = offset.Length();
			if (r < m_effectRadius)
				density += m_mass * smoothingKernals::Poly6(r, m_effectRadius);
		}
		return density;
	}
	float Pressure(float density, float targetDensity, float pressureMultiplier)
	{
		float densityError = density - targetDensity;
		return densityError * pressureMultiplier;
	}
	Float2 SphFluid2d::PressureForceDensity(int particleIndex)
	{
		Float2 pressureForce = Float2::zero;
		float particlePressure = Pressure(m_densities[particleIndex], m_targetDensity, m_pressureMultiplier);
		for (int i = 0; i < m_particleCount; i++)
		{
			if (i == particleIndex)
				continue;

			Float2 offset = m_positions[particleIndex] - m_positions[i];
			float r = offset.Length();
			if (r < m_effectRadius)
			{
				Float2 dir = (r < 1e-8f) ? math::Random::UniformDirection2() : offset / r;
				float otherParticlePressure = Pressure(m_densities[i], m_targetDensity, m_pressureMultiplier);
				float sharedPressure = 0.5f * (particlePressure + otherParticlePressure);
				pressureForce += -m_mass * sharedPressure * smoothingKernals::DSpiky(r, dir, m_effectRadius) / m_densities[i];
			}
		}
		return pressureForce;
	}
	Float2 SphFluid2d::ViscosityForceDensity(int particleIndex)
	{
		Float2 viscosityForce = Float2::zero;
		for (int i = 0; i < m_particleCount; i++)
		{
			if (i == particleIndex)
				continue;

			Float2 offset = m_positions[particleIndex] - m_positions[i];
			float r = offset.Length();
			if (r < m_effectRadius)
			{
				Float2 velocityDiff = m_velocities[i] - m_velocities[particleIndex];
				viscosityForce += (m_mass * smoothingKernals::DDViscos(r, m_effectRadius) / m_densities[i]) * velocityDiff;
			}
		}
		return m_viscosity * viscosityForce;
	}
	Float2 SphFluid2d::GravityForceDensity(int particleIndex)
	{
		return m_densities[particleIndex] * Float2(0.0f, -m_gravity);
	}
	void SphFluid2d::BoundaryCollisions(Float2& position, Float2& velocity)
	{
		float epsilon = 1e-4f;
		Float2 min = m_fluidBounds.GetMin();
		Float2 max = m_fluidBounds.GetMax();

		if (position.x < min.x)
		{
			position.x = min.x + epsilon;
			velocity.x *= -m_collisionDampening;
		}
		if (position.x > max.x)
		{
			position.x = max.x - epsilon;
			velocity.x *= -m_collisionDampening;
		}
		if (position.y < min.y)
		{
			position.y = min.y + epsilon;
			velocity.y *= -m_collisionDampening;
		}
		if (position.y > max.y)
		{
			position.y = max.y - epsilon;
			velocity.y *= -m_collisionDampening;
		}
	}
}