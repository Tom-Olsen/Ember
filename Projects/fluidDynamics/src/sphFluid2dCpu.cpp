#include "sphFluid2dCpu.h"
#include "hashGrid2d.h"
#include "smoothingKernals.h"
#include "sphFluid2dCpuEditorWindow.h"



namespace emberEngine
{
	const std::array<Int2, 9> SphFluid2dCpu::s_offsets = {
	Int2(-1, -1), Int2(-1, 0), Int2(-1, 1),
	Int2( 0, -1), Int2( 0, 0), Int2( 0, 1),
	Int2( 1, -1), Int2( 1, 0), Int2( 1, 1) };



	// Constructor/Destructor:
	SphFluid2dCpu::SphFluid2dCpu()
	{
		// Management:
		m_isRunning = false;
		m_timeScale = 2.0f;
		m_timeStep = 0;
		m_particleCount = 400;
		m_useGridOptimization = true;

		// Physics:
		m_effectRadius = 0.5f;
		m_mass = 1.0f;
		m_viscosity = 0.5f;
		m_surfaceTension = 0.07f;
		m_collisionDampening = 0.95f;
		m_targetDensity = 15.0f;
		m_pressureMultiplier = 20.0f;
		m_gravity = 0.5f;
		m_maxVelocity = 5.0f;

		// User Interaction/Boundaries:
		SetAttractorRadius(3.0f);
		m_attractorStrength = 2.0f;
		m_fluidBounds = Bounds(Float3::zero, Float3(16.0f, 9.0f, 0.01f));

		// Visuals:
		SetInitialDistributionRadius(6.0f);
		m_visualRadius = 0.25f;
		m_pParticleMesh = std::unique_ptr<Mesh>(MeshGenerator::UnitQuad()->Scale(m_visualRadius));
		m_pParticleMaterial = MaterialManager::GetMaterial("particleMaterial");

		// Editor Window:
		editorWindow = std::make_unique<SphFluid2dCpuEditorWindow>(this);
	}
	SphFluid2dCpu::~SphFluid2dCpu()
	{

	}



	// Physics update:
	void SphFluid2dCpu::Reset()
	{
		m_timeStep = 0;
		m_positions.resize(m_particleCount);
		m_velocities.resize(m_particleCount);
		m_densities.resize(m_particleCount);
		m_normals.resize(m_particleCount);
		m_curvatures.resize(m_particleCount);
		m_forceDensities.resize(m_particleCount);
		m_kp1.resize(m_particleCount);
		m_kp2.resize(m_particleCount);
		m_kv1.resize(m_particleCount);
		m_kv2.resize(m_particleCount);
		m_tempPositions.resize(m_particleCount);
		m_tempVelocities.resize(m_particleCount);
		m_pGrid = std::make_unique<HashGrid2d>(m_particleCount);

		float phi = math::pi * (math::Sqrt(5.0f) - 1.0f);
		float radius = 0.4f * math::Min(m_fluidBounds.GetSize().x, m_fluidBounds.GetSize().y);
		for (int i = 0; i < m_particleCount; i++)
		{
			float r = i / (m_particleCount - 1.0f) * radius;
			float theta = phi * i;
			m_positions[i].x = math::Cos(theta) * r;
			m_positions[i].y = math::Sin(theta) * r;
			m_velocities[i].x = 0;
			m_velocities[i].y = 0;
			m_forceDensities[i] = 0;
			m_kp1[i] = 0;
			m_kp2[i] = 0;
			m_kv1[i] = 0;
			m_kv2[i] = 0;
			m_tempPositions[i] = 0;
			m_tempVelocities[i] = 0;
		}

		// Compute densities:
		m_pGrid->UpdateGrid(m_positions, m_effectRadius);
		m_pGrid->ApplySort(m_velocities);
		for (int i = 0; i < m_particleCount; i++)
			m_densities[i] = Density(i, m_positions);
		//// Compute normals:
		//for (int i = 0; i < m_particleCount; i++)
		//	m_normals[i] = Normal(i, m_positions, m_densities);
		//// Compute curvature:
		//for (int i = 0; i < m_particleCount; i++)
		//	m_curvatures[i] = Curvature(i, m_positions, m_normals, m_densities);
	}
	void SphFluid2dCpu::FixedUpdate()
	{
		if (!m_isRunning)
			return;

		// Do multiple iterations of deltaT<=Time::GetFixedDeltaTime() if timeScale is bigger 1. Otherwise 1 iteration per FixedUpdate().
		float dt = Time::GetFixedDeltaTime();
		float timeStep = m_timeScale * dt;
		float restTime = timeStep;
		while (restTime > 0.0f)
		{
			float deltaT = math::Min(dt, restTime);
			TimeStepRungeKutta2(deltaT);
			restTime -= deltaT;
		}
		m_timeStep++;
	}
	void SphFluid2dCpu::TimeStepLeapFrog(float deltaT)
	{
		// Update grid for fast nearest neighbor particle look up:
		m_pGrid->UpdateGrid(m_positions, m_effectRadius);
		m_pGrid->ApplySort(m_velocities);

		// Compute densities:
		for (int i = 0; i < m_particleCount; i++)
			m_densities[i] = Density(i, m_positions);
		//// Compute normals:
		//for (int i = 0; i < m_particleCount; i++)
		//	m_normals[i] = Normal(i, m_positions, m_densities);
		//// Compute curvature:
		//for (int i = 0; i < m_particleCount; i++)
		//	m_curvatures[i] = Curvature(i, m_positions, m_normals, m_densities);

		// Compute force densities:
		for (int i = 0; i < m_particleCount; i++)
		{
			m_forceDensities[i] = PressureForceDensity(i, m_positions, m_densities);
			m_forceDensities[i] += ViscosityForceDensity(i, m_positions, m_velocities, m_densities);
			//m_forceDensities[i] += SurfaceTensionForceDensity(i, m_normals, m_curvatures);
			m_forceDensities[i] += GravityForceDensity(i, m_densities);
			m_forceDensities[i] += ExternalForceDensity(i, m_positions, m_densities);
		}

		// Step:
		if (m_timeStep == 0)
		{
			for (int i = 0; i < m_particleCount; i++)
			{
				Float2 acceleration = m_forceDensities[i] / m_densities[i];
				m_velocities[i] += 0.5f * acceleration * deltaT;
			}
		}
		for (int i = 0; i < m_particleCount; i++)
		{
			Float2 acceleration = m_forceDensities[i] / m_densities[i];
			m_positions[i] += m_velocities[i] * deltaT;
			m_velocities[i] += acceleration * deltaT;
			BoundaryCollisions(m_positions[i], m_velocities[i]);
		}
	}
	void SphFluid2dCpu::TimeStepVelocityVerlet(float deltaT)
	{
		// Update grid for fast nearest neighbor particle look up:
		m_pGrid->UpdateGrid(m_positions, m_effectRadius);
		m_pGrid->ApplySort(m_velocities);

		// Compute densities:
		for (int i = 0; i < m_particleCount; i++)
			m_densities[i] = Density(i, m_positions);
		//// Compute normals:
		//for (int i = 0; i < m_particleCount; i++)
		//	m_normals[i] = Normal(i, m_positions, m_densities);
		//// Compute curvature:
		//for (int i = 0; i < m_particleCount; i++)
		//	m_curvatures[i] = Curvature(i, m_positions, m_normals, m_densities);

		// Compute force densities:
		for (int i = 0; i < m_particleCount; i++)
		{
			m_forceDensities[i] = PressureForceDensity(i, m_positions, m_densities);
			m_forceDensities[i] += ViscosityForceDensity(i, m_positions, m_velocities, m_densities);
			//m_forceDensities[i] += SurfaceTensionForceDensity(i, m_normals, m_curvatures);
			m_forceDensities[i] += GravityForceDensity(i, m_densities);
			m_forceDensities[i] += ExternalForceDensity(i, m_positions, m_densities);
		}

		// Step:
		for (int i = 0; i < m_particleCount; i++)
		{
			Float2 acceleration = m_forceDensities[i] / m_densities[i];
			Float2 velocityHalfStep = m_velocities[i] + 0.5f * acceleration * deltaT;
			m_positions[i] += velocityHalfStep * deltaT;
			m_velocities[i] = velocityHalfStep + 0.5f * acceleration * deltaT;
			BoundaryCollisions(m_positions[i], m_velocities[i]);
		}
	}
	void SphFluid2dCpu::TimeStepRungeKutta2(float deltaT)
	{
		// Update grid for fast nearest neighbor particle look up:
		m_pGrid->UpdateGrid(m_positions, m_effectRadius);
		m_pGrid->ApplySort(m_velocities);

		// Compute densities:
		for (int i = 0; i < m_particleCount; i++)
			m_densities[i] = Density(i, m_positions);
		//// Compute normals:
		//for (int i = 0; i < m_particleCount; i++)
		//	m_normals[i] = Normal(i, m_positions, m_densities);
		//// Compute curvature:
		//for (int i = 0; i < m_particleCount; i++)
		//	m_curvatures[i] = Curvature(i, m_positions, m_normals, m_densities);

		// Compute force densities:
		for (int i = 0; i < m_particleCount; i++)
		{
			m_forceDensities[i] = PressureForceDensity(i, m_positions, m_densities);
			m_forceDensities[i] += ViscosityForceDensity(i, m_positions, m_velocities, m_densities);
			//m_forceDensities[i] += SurfaceTensionForceDensity(i, m_normals, m_curvatures);
			m_forceDensities[i] += GravityForceDensity(i, m_densities);
			m_forceDensities[i] += ExternalForceDensity(i, m_positions, m_densities);
		}

		// First Runte-Kutta step:
		for (int i = 0; i < m_particleCount; i++)
		{
			Float2 acceleration = m_forceDensities[i] / m_densities[i];
			m_kp1[i] = m_velocities[i] * deltaT;
			m_kv1[i] = acceleration * deltaT;
			m_tempPositions[i] = m_positions[i] + 0.5f * m_kp1[i];
			m_tempVelocities[i] = m_velocities[i] + 0.5f * m_kv1[i];
			float speed = m_tempVelocities[i].Length();
			if (speed > m_maxVelocity)
				m_tempVelocities[i] *= (m_maxVelocity / speed);
		}

		// Update grid for fast nearest neighbor particle look up:
		m_pGrid->UpdateGrid(m_tempPositions, m_effectRadius);
		m_pGrid->ApplySort(m_tempVelocities);
		m_pGrid->ApplySort(m_positions);
		m_pGrid->ApplySort(m_velocities);
		m_pGrid->ApplySort(m_kv1);

		// Compute intermediate densities:
		for (int i = 0; i < m_particleCount; i++)
			m_densities[i] = Density(i, m_tempPositions);
		//// Compute intermediate normals:
		//for (int i = 0; i < m_particleCount; i++)
		//	m_normals[i] = Normal(i, m_tempPositions, m_densities);
		//// Compute intermediate curvature:
		//for (int i = 0; i < m_particleCount; i++)
		//	m_curvatures[i] = Curvature(i, m_tempPositions, m_normals, m_densities);

		// Compute intermediate force densities:
		for (int i = 0; i < m_particleCount; i++)
		{
			m_forceDensities[i] = PressureForceDensity(i, m_tempPositions, m_densities);
			m_forceDensities[i] += ViscosityForceDensity(i, m_tempPositions, m_tempVelocities, m_densities);
			//m_forceDensities[i] += SurfaceTensionForceDensity(i, m_normals, m_curvatures);
			m_forceDensities[i] += GravityForceDensity(i, m_densities);
			m_forceDensities[i] += ExternalForceDensity(i, m_tempPositions, m_densities);
		}

		// Second Runge-Kutta step:
		for (int i = 0; i < m_particleCount; i++)
		{
			Float2 acceleration = m_forceDensities[i] / m_densities[i];
			m_kp2[i] = (m_velocities[i] + 0.5f * m_kv1[i]) * deltaT;
			m_kv2[i] = acceleration * deltaT;
			m_positions[i] += m_kp2[i];
			m_velocities[i] += m_kv2[i];
			float speed = m_velocities[i].Length();
			if (speed > m_maxVelocity)
				m_velocities[i] *= (m_maxVelocity / speed);
			BoundaryCollisions(m_positions[i], m_velocities[i]);
		}
	}



	// Setters:
	void SphFluid2dCpu::SetIsRunning(bool isRunning)
	{
		m_isRunning = isRunning;
	}
	void SphFluid2dCpu::SetTimeScale(float timeScale)
	{
		m_timeScale = timeScale;
	}
	void SphFluid2dCpu::SetUseGridOptimization(bool useGridOptimization)
	{
		m_useGridOptimization = useGridOptimization;
	}
	void SphFluid2dCpu::SetParticleCount(int particleCount)
	{
		if (m_particleCount != particleCount)
		{
			m_particleCount = particleCount;
			Reset();
		}
	}
	void SphFluid2dCpu::SetEffectRadius(float effectRadius)
	{
		m_effectRadius = effectRadius;
	}
	void SphFluid2dCpu::SetMass(float mass)
	{
		m_mass = mass;
	}
	void SphFluid2dCpu::SetViscosity(float viscosity)
	{
		m_viscosity = viscosity;
	}
	void SphFluid2dCpu::SetSurfaceTension(float surfaceTension)
	{
		m_surfaceTension = surfaceTension;
	}
	void SphFluid2dCpu::SetCollisionDampening(float collisionDampening)
	{
		m_collisionDampening = collisionDampening;
	}
	void SphFluid2dCpu::SetTargetDensity(float targetDensity)
	{
		m_targetDensity = targetDensity;
	}
	void SphFluid2dCpu::SetPressureMultiplier(float pressureMultiplier)
	{
		m_pressureMultiplier = pressureMultiplier;
	}
	void SphFluid2dCpu::SetGravity(float gravity)
	{
		m_gravity = gravity;
	}
	void SphFluid2dCpu::SetMaxVelocity(float maxVelocity)
	{
		m_maxVelocity = maxVelocity;
	}
	void SphFluid2dCpu::SetAttractorRadius(float attractorRadius)
	{
		attractorRadius = math::Max(0.01f, attractorRadius);
		if (m_attractorRadius != attractorRadius)
		{
			std::unique_ptr<Mesh> pNewRingMesh = std::unique_ptr<Mesh>(MeshGenerator::ArcFlatUv(attractorRadius - 0.1f, attractorRadius + 0.1f, 360.0f, 100, "attractorRing"));
			std::swap(m_pRingMesh, pNewRingMesh);
			m_attractorRadius = attractorRadius;
		}
	}
	void SphFluid2dCpu::SetAttractorStrength(float attractorStrength)
	{
		attractorStrength = math::Max(0.01f, attractorStrength);
		m_attractorStrength = attractorStrength;
	}
	void SphFluid2dCpu::SetFluidBounds(const Bounds& bounds)
	{
		m_fluidBounds = bounds;
	}
	void SphFluid2dCpu::SetInitialDistributionRadius(float initialDistributionRadius)
	{
		if (m_initialDistributionRadius != initialDistributionRadius)
		{
			m_initialDistributionRadius = initialDistributionRadius;
			Reset();
		}
	}
	void SphFluid2dCpu::SetVisualRadius(float visualRadius)
	{
		visualRadius = math::Max(0.01f, visualRadius);
		if (m_visualRadius != visualRadius)
		{
			std::unique_ptr<Mesh> pNewParticleMesh = std::unique_ptr<Mesh>(MeshGenerator::UnitQuad()->Scale(visualRadius));
			std::swap(m_pParticleMesh, pNewParticleMesh);
			m_visualRadius = visualRadius;
		}
	}



	// Getters:
	bool SphFluid2dCpu::GetIsRunning() const
	{
		return m_isRunning;
	}
	float SphFluid2dCpu::GetTimeScale() const
	{
		return m_timeScale;
	}
	bool SphFluid2dCpu::GetUseGridOptimization() const
	{
		return m_useGridOptimization;
	}
	uint32_t SphFluid2dCpu::GetTimeStep() const
	{
		return m_timeStep;
	}
	int SphFluid2dCpu::GetParticleCount() const
	{
		return m_particleCount;
	}
	float SphFluid2dCpu::GetEffectRadius() const
	{
		return m_effectRadius;
	}
	float SphFluid2dCpu::GetMass() const
	{
		return m_mass;
	}
	float SphFluid2dCpu::GetViscosity() const
	{
		return m_viscosity;
	}
	float SphFluid2dCpu::GetSurfaceTension() const
	{
		return m_surfaceTension;
	}
	float SphFluid2dCpu::GetCollisionDampening() const
	{
		return m_collisionDampening;
	}
	float SphFluid2dCpu::GetTargetDensity() const
	{
		return m_targetDensity;
	}
	float SphFluid2dCpu::GetPressureMultiplier() const
	{
		return m_pressureMultiplier;
	}
	float SphFluid2dCpu::GetGravity() const
	{
		return m_gravity;
	}
	float SphFluid2dCpu::GetMaxVelocity() const
	{
		return m_maxVelocity;
	}
	float SphFluid2dCpu::GetAttractorRadius() const
	{
		return m_attractorRadius;
	}
	float SphFluid2dCpu::GetAttractorStrength() const
	{
		return m_attractorStrength;
	}
	Bounds SphFluid2dCpu::GetFluidBounds() const
	{
		return m_fluidBounds;
	}
	float SphFluid2dCpu::GetInitialDistributionRadius() const
	{
		return m_initialDistributionRadius;
	}
	float SphFluid2dCpu::GetVisualRadius() const
	{
		return m_visualRadius;
	}


	// Overrides:
	void SphFluid2dCpu::Start()
	{
		Reset();
	}
	void SphFluid2dCpu::Update()
	{
		// Keyboard interactions:
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
			LOG_TRACE("Simulation stopped and reset.");
		}

		// Mouse scrolling:
		float mouseScroll = EventSystem::MouseScrollY();
		if (mouseScroll != 0)
		{
			if (EventSystem::KeyDownOrHeld(SDLK_LSHIFT))
			{
				float zoomFactor = 1.0f + 0.1f * mouseScroll;
				SetAttractorStrength(zoomFactor * m_attractorStrength);
			}
			else
			{
				float zoomFactor = 1.0f + 0.1f * mouseScroll;
				SetAttractorRadius(zoomFactor * m_attractorRadius);
			}
		}

		// Mouse left and right click:
		if (EventSystem::MouseHeld(EventSystem::MouseButton::left) ^ EventSystem::MouseHeld(EventSystem::MouseButton::right)) // exlusive or
		{
			Ray ray = Ray::CameraRay(EventSystem::MousePos01());
			std::optional<Float3> hit = m_fluidBounds.IntersectRay(ray);
			if (hit.has_value())
			{
				m_attractorPoint = Float2(hit.value());
				ShaderProperties* shaderProperties = Graphics::DrawMesh(m_pRingMesh.get(), MaterialManager::GetMaterial("simpleUnlitMaterial"), hit.value(), Float3x3::identity, 1.0f, false, false);
				shaderProperties->SetValue("SurfaceProperties", "diffuseColor", Float4::red);
				if (EventSystem::MouseHeld(EventSystem::MouseButton::left))
					m_attractorState = 1;
				if (EventSystem::MouseHeld(EventSystem::MouseButton::right))
					m_attractorState = -1;
			}
			else
				m_attractorState = 0;
		}
		else
			m_attractorState = 0;


		// Rendering:
		Float4x4 localToWorld = GetTransform()->GetLocalToWorldMatrix();
		Graphics::DrawBounds(localToWorld, m_fluidBounds, 0.01f, Float4::white, false, false);
		for (int i = 0; i < m_particleCount; i++)
		{
			Float4x4 matrix = localToWorld * Float4x4::Translate(Float3(m_positions[i]));
			ShaderProperties* pShaderProperties = Graphics::DrawMesh(m_pParticleMesh.get(), m_pParticleMaterial, matrix, false, false);

			// Color by density:
			{
				float t = (m_densities[i] - m_targetDensity) / m_targetDensity;
				float t0 = math::Clamp(t, 0.0f, 1.0f);
				float t1 = math::Clamp(t - 1.0f, 0.0f, 1.0f);
				Float4 colorA = t0 * Float4::white + (1.0f - t0) * Float4::blue;
				Float4 colorB = t1 * Float4::red + (1.0f - t1) * Float4::white;
				Float4 color = (t < 1.0f) ? colorA : colorB;
				pShaderProperties->SetValue("SurfaceProperties", "diffuseColor", color);
			}

			//// Color by normal length:
			//{
			//	float length = m_normals[i].Length();
			//	Float4 color = Float4::white * (1.0f - length) + Float4::red * length;
			//	pShaderProperties->SetValue("SurfaceProperties", "diffuseColor", color);
			//}

			//// Color by curvature length:
			//{
			//	float t = m_curvatures[i];
			//	float t0 = math::Clamp(t, 0.0f, 1.0f);
			//	float t1 = math::Clamp(t - 1.0f, 0.0f, 1.0f);
			//	Float4 colorA = t0 * Float4::white + (1.0f - t0) * Float4::blue;
			//	Float4 colorB = t1 * Float4::red + (1.0f - t1) * Float4::white;
			//	Float4 color = (t < 1.0f) ? colorA : colorB;
			//	pShaderProperties->SetValue("SurfaceProperties", "diffuseColor", color);
			//}
		}
	}
	const std::string SphFluid2dCpu::ToString() const
	{
		return "SphFluid2dCpu";
	}



	// Private methods:
	// Physics:
	float SphFluid2dCpu::Density(int particleIndex, const std::vector<Float2>& positions)
	{
		if (m_useGridOptimization)
		{
			Float2 particlePos = positions[particleIndex];
			Int2 particleCell = m_pGrid->Cell(particlePos, m_effectRadius);

			float density = 0;
			for (int i = 0; i < 9; i++)
			{
				uint32_t neighbourCellHash = m_pGrid->CellHash(particleCell + s_offsets[i]);
				uint32_t neighbourCellKey = m_pGrid->CellKey(neighbourCellHash);
				uint32_t otherIndex = m_pGrid->GetStartIndex(neighbourCellKey);

				while (otherIndex < m_particleCount) // at most as many iterations as there are particles.
				{
					uint32_t otherCellKey = m_pGrid->GetCellKey(otherIndex);
					if (otherCellKey != neighbourCellKey)	// found first particle that is in a different cell => done.
						break;

					Float2 otherPos = positions[otherIndex];
					Float2 offset = particlePos - otherPos;
					float r = offset.Length();
					if (r < m_effectRadius)
						density += m_mass * smoothingKernals::Poly6(r, m_effectRadius);

					otherIndex++;
				}
			}
			return density;
		}
		else
		{
			float density = 0;
			for (int i = 0; i < m_particleCount; i++)
			{
				Float2 offset = positions[particleIndex] - positions[i];
				float r = offset.Length();
				if (r < m_effectRadius)
					density += m_mass * smoothingKernals::Poly6(r, m_effectRadius);
			}
			return density;
		}
	}
	Float2 SphFluid2dCpu::Normal(int particleIndex, const std::vector<Float2>& positions, const std::vector<float>& densities)
	{
		if (m_useGridOptimization)
		{
			Float2 particlePos = positions[particleIndex];
			Int2 particleCell = m_pGrid->Cell(particlePos, m_effectRadius);

			Float2 normal = Float2::zero;
			for (int i = 0; i < 9; i++)
			{
				uint32_t neighbourCellHash = m_pGrid->CellHash(particleCell + s_offsets[i]);
				uint32_t neighbourCellKey = m_pGrid->CellKey(neighbourCellHash);
				uint32_t otherIndex = m_pGrid->GetStartIndex(neighbourCellKey);

				while (otherIndex < m_particleCount) // at most as many iterations as there are particles.
				{
					if (otherIndex == particleIndex)
					{
						otherIndex++;
						continue;
					}

					uint32_t otherCellKey = m_pGrid->GetCellKey(otherIndex);
					if (otherCellKey != neighbourCellKey)	// found first particle that is in a different cell => done.
						break;

					Float2 otherPos = positions[otherIndex];
					Float2 offset = particlePos - otherPos;
					float r = offset.Length();
					if (r < m_effectRadius && r > 1e-8f)
					{
						Float2 dir = offset / r;
						normal += m_mass / densities[otherIndex] * smoothingKernals::DSpiky(r, dir, m_effectRadius);
					}

					otherIndex++;
				}
			}
			return normal;
		}
		else
		{
			Float2 normal= Float2::zero;
			for (int i = 0; i < m_particleCount; i++)
			{
				if (i == particleIndex)
					continue;

				Float2 offset = positions[particleIndex] - positions[i];
				float r = offset.Length();
				if (r < m_effectRadius && r > 1e-8f)
				{
					Float2 dir = offset / r;
					normal += m_mass / densities[i] * smoothingKernals::DSpiky(r, dir, m_effectRadius);
				}
			}
			return normal;
		}
	}
	float SphFluid2dCpu::Curvature(int particleIndex, const std::vector<Float2>& positions, const std::vector<Float2>& normals, const std::vector<float>& densities)
	{
		if (m_useGridOptimization)
		{
			Float2 particlePos = positions[particleIndex];
			Int2 particleCell = m_pGrid->Cell(particlePos, m_effectRadius);

			float curvature = 0.0f;
			for (int i = 0; i < 9; i++)
			{
				uint32_t neighbourCellHash = m_pGrid->CellHash(particleCell + s_offsets[i]);
				uint32_t neighbourCellKey = m_pGrid->CellKey(neighbourCellHash);
				uint32_t otherIndex = m_pGrid->GetStartIndex(neighbourCellKey);

				while (otherIndex < m_particleCount) // at most as many iterations as there are particles.
				{
					if (otherIndex == particleIndex)
					{
						otherIndex++;
						continue;
					}

					uint32_t otherCellKey = m_pGrid->GetCellKey(otherIndex);
					if (otherCellKey != neighbourCellKey)	// found first particle that is in a different cell => done.
						break;

					Float2 otherPos = positions[otherIndex];
					Float2 offset = particlePos - otherPos;
					float r = offset.Length();
					if (r < m_effectRadius && r > 1e-8f)
						curvature -= m_mass / densities[otherIndex] * smoothingKernals::DDSpiky(r, m_effectRadius) / normals[otherIndex].Length();

					otherIndex++;
				}
			}
			return curvature;
		}
		else
		{
			float curvature = 0.0f;
			for (int i = 0; i < m_particleCount; i++)
			{
				if (i == particleIndex)
					continue;

				Float2 offset = positions[particleIndex] - positions[i];
				float r = offset.Length();
				if (r < m_effectRadius && r > 1e-8f)
					curvature -= m_mass / densities[i] * smoothingKernals::DDSpiky(r, m_effectRadius) / normals[i].Length();
			}
			return curvature;
		}
	}
	float Pressure(float density, float targetDensity, float pressureMultiplier)
	{
		float densityError = density - targetDensity;
		return densityError * pressureMultiplier;
		//return pressureMultiplier * (math::Pow(density / targetDensity, 7.0f) - 1.0f);
	}
	Float2 SphFluid2dCpu::PressureForceDensity(int particleIndex, const std::vector<Float2>& positions, const std::vector<float>& densities)
	{
		if (m_useGridOptimization)
		{
			float particlePressure = Pressure(densities[particleIndex], m_targetDensity, m_pressureMultiplier);
			Float2 particlePos = positions[particleIndex];
			Int2 particleCell = m_pGrid->Cell(particlePos, m_effectRadius);

			Float2 pressureForce = Float2::zero;
			for (int i = 0; i < 9; i++)
			{
				uint32_t neighbourCellHash = m_pGrid->CellHash(particleCell + s_offsets[i]);
				uint32_t neighbourCellKey = m_pGrid->CellKey(neighbourCellHash);
				uint32_t otherIndex = m_pGrid->GetStartIndex(neighbourCellKey);

				while (otherIndex < m_particleCount) // at most as many iterations as there are particles.
				{
					if (otherIndex == particleIndex)
					{
						otherIndex++;
						continue;
					}

					uint32_t otherCellKey = m_pGrid->GetCellKey(otherIndex);
					if (otherCellKey != neighbourCellKey)	// found first particle that is in a different cell => done.
						break;

					Float2 otherPos = positions[otherIndex];
					Float2 offset = particlePos - otherPos;
					float r = offset.Length();
					if (r < m_effectRadius)
					{
						Float2 dir = (r < 1e-8f) ? math::Random::UniformDirection2() : offset / r;
						float otherParticlePressure = Pressure(densities[otherIndex], m_targetDensity, m_pressureMultiplier);
						float sharedPressure = 0.5f * (particlePressure + otherParticlePressure);
						pressureForce += -m_mass * sharedPressure * smoothingKernals::DSpiky(r, dir, m_effectRadius) / densities[otherIndex];
					}

					otherIndex++;
				}
			}
			return pressureForce;
		}
		else
		{
			Float2 pressureForce = Float2::zero;
			float particlePressure = Pressure(densities[particleIndex], m_targetDensity, m_pressureMultiplier);
			for (int i = 0; i < m_particleCount; i++)
			{
				if (i == particleIndex)
					continue;
			
				Float2 offset = positions[particleIndex] - positions[i];
				float r = offset.Length();
				if (r < m_effectRadius)
				{
					Float2 dir = (r < 1e-8f) ? math::Random::UniformDirection2() : offset / r;
					float otherParticlePressure = Pressure(densities[i], m_targetDensity, m_pressureMultiplier);
					float sharedPressure = 0.5f * (particlePressure + otherParticlePressure);
					pressureForce += -m_mass * sharedPressure * smoothingKernals::DSpiky(r, dir, m_effectRadius) / densities[i];
				}
			}
			return pressureForce;
		}
	}
	Float2 SphFluid2dCpu::ViscosityForceDensity(int particleIndex, const std::vector<Float2>& positions, const std::vector<Float2>& velocities, const std::vector<float>& densities)
	{
		if (m_useGridOptimization)
		{
			Float2 particlePos = positions[particleIndex];
			Float2 particleVel = velocities[particleIndex];
			Int2 particleCell = m_pGrid->Cell(particlePos, m_effectRadius);

			Float2 viscosityForce = Float2::zero;
			for (int i = 0; i < 9; i++)
			{
				uint32_t neighbourCellHash = m_pGrid->CellHash(particleCell + s_offsets[i]);
				uint32_t neighbourCellKey = m_pGrid->CellKey(neighbourCellHash);
				uint32_t otherIndex = m_pGrid->GetStartIndex(neighbourCellKey);

				while (otherIndex < m_particleCount) // at most as many iterations as there are particles.
				{
					if (otherIndex == particleIndex)
					{
						otherIndex++;
						continue;
					}

					uint32_t otherCellKey = m_pGrid->GetCellKey(otherIndex);
					if (otherCellKey != neighbourCellKey)	// found first particle that is in a different cell => done.
						break;

					Float2 otherPos = positions[otherIndex];
					Float2 otherVel = velocities[otherIndex];
					Float2 offset = particlePos - otherPos;
					float r = offset.Length();
					if (r < m_effectRadius)
					{
						Float2 velocityDiff = otherVel - particleVel;
						viscosityForce += (m_mass * smoothingKernals::DDViscos(r, m_effectRadius) / densities[otherIndex]) * velocityDiff;
					}

					otherIndex++;
				}
			}
			return m_viscosity * viscosityForce;
		}
		else
		{
			Float2 viscosityForce = Float2::zero;
			for (int i = 0; i < m_particleCount; i++)
			{
				if (i == particleIndex)
					continue;
			
				Float2 offset = positions[particleIndex] - positions[i];
				float r = offset.Length();
				if (r < m_effectRadius)
				{
					Float2 velocityDiff = velocities[i] - velocities[particleIndex];
					viscosityForce += (m_mass * smoothingKernals::DDViscos(r, m_effectRadius) / densities[i]) * velocityDiff;
				}
			}
			return m_viscosity * viscosityForce;
		}
	}
	Float2 SphFluid2dCpu::SurfaceTensionForceDensity(int particleIndex, const std::vector<Float2>& normals, const std::vector<float>& curvatures)
	{
		Float2 normal = normals[particleIndex];
		float normalLength = normal.Length();
		//return (normalLength > 0.1f) ? -m_surfaceTension * normal / normalLength : Float2::zero;
		return (normalLength > 0.1f) ? m_surfaceTension * curvatures[particleIndex] * normal / normalLength : Float2::zero;
	}
	Float2 SphFluid2dCpu::GravityForceDensity(int particleIndex, const std::vector<float>& densities)
	{
		return densities[particleIndex] * Float2(0.0f, -m_gravity);
	}
	Float2 SphFluid2dCpu::ExternalForceDensity(int particleIndex, const std::vector<Float2>& positions, const std::vector<float>& densities)
	{
		if (m_attractorState != 0)
		{
			Float2 offset = m_attractorPoint - positions[particleIndex];
			float r = offset.Length();
			if (r < m_attractorRadius && r > 1e-8f)
				return m_attractorState * m_attractorStrength * densities[particleIndex] * offset * r * r;
		}
		return 0.0f;
	}
	void SphFluid2dCpu::BoundaryCollisions(Float2& position, Float2& velocity)
	{
		float epsilon = 1e-4f;
		Float3 min = m_fluidBounds.GetMin();
		Float3 max = m_fluidBounds.GetMax();

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