#include "sphFluid2dCpu.h"
#include "hashGrid2d.h"
#include "smoothingKernals.h"
#include "sphFluid2dCpuEditorWindow.h"



namespace fluidDynamics
{
	const std::array<Int2, 9> SphFluid2dCpu::s_offsets = {
	Int2(-1, -1), Int2(-1, 0), Int2(-1, 1),
	Int2( 0, -1), Int2( 0, 0), Int2( 0, 1),
	Int2( 1, -1), Int2( 1, 0), Int2( 1, 1) };



	// Public methods:
	// Constructor/Destructor:
	SphFluid2dCpu::SphFluid2dCpu()
	{
		// Management:
		m_isRunning = false;
		m_timeScale = 2.0f;
		m_useGridOptimization = true;
		m_timeStep = 0;
		m_particleCount = 400;

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
		SetColorMode(1);
		SetInitialDistributionRadius(6.0f);
		SetVisualRadius(0.25f);
		m_particleMaterial = MaterialManager::GetMaterial("particleMaterial2d");

		// Editor Window:
		editorWindow = std::make_unique<emberEditor::SphFluid2dCpuEditorWindow>(this);
		Reset();
	}
	SphFluid2dCpu::~SphFluid2dCpu()
	{

	}



	// Physics update:
	void SphFluid2dCpu::Reset()
	{
		m_timeStep = 0;

		// Reallocate vectors:
		if (m_positions.size() != m_particleCount)
		{
			m_positions.resize(m_particleCount);
			m_velocities.resize(m_particleCount);
			m_densities.resize(m_particleCount);
			m_normals.resize(m_particleCount);
			m_curvatures.resize(m_particleCount);
			m_forceDensities.resize(m_particleCount);
			m_kp1.resize(m_particleCount);
			m_kv1.resize(m_particleCount);
			m_kp2.resize(m_particleCount);
			m_kv2.resize(m_particleCount);
			m_tempPositions.resize(m_particleCount);
			m_tempVelocities.resize(m_particleCount);
			m_pGrid = std::make_unique<HashGrid2d>(m_particleCount);
		}

		float phi = math::pi * (math::Sqrt(5.0f) - 1.0f);
		for (int i = 0; i < m_particleCount; i++)
		{
			float r = i / (m_particleCount - 1.0f) * m_initialDistributionRadius;
			float theta = phi * i;
			m_positions[i].x = math::Cos(theta) * r;
			m_positions[i].y = math::Sin(theta) * r;
			m_velocities[i].x = 0;
			m_velocities[i].y = 0;
			m_forceDensities[i] = 0;
			m_kp1[i] = 0;
			m_kv1[i] = 0;
			m_kp2[i] = 0;
			m_kv2[i] = 0;
			m_tempPositions[i] = 0;
			m_tempVelocities[i] = 0;
		}

		// Reset fluid to initial data:
		m_pGrid->UpdateGrid(m_positions, m_effectRadius);
		m_pGrid->ApplySort(m_positions);
		m_pGrid->ApplySort(m_velocities);
		for (int i = 0; i < m_particleCount; i++)
			m_densities[i] = Density(i, m_positions);
		//for (int i = 0; i < m_particleCount; i++)
		//	m_normals[i] = Normal(i, m_positions, m_densities);
		//for (int i = 0; i < m_particleCount; i++)
		//	m_curvatures[i] = Curvature(i, m_positions, m_normals, m_densities);

		m_reset = false;
	}
	void SphFluid2dCpu::FixedUpdate()
	{
		if (!m_isRunning || m_reset)
			return;

		// Do multiple iterations of deltaT<=dt if timeScale is bigger 1. Otherwise 1 iteration per FixedUpdate().
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
		m_pGrid->ApplySort(m_positions);
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
			m_forceDensities[i] = ForceDensity(i, m_positions, m_velocities, m_densities);

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
		m_pGrid->ApplySort(m_positions);
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
			m_forceDensities[i] = ForceDensity(i, m_positions, m_velocities, m_densities);

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
		constexpr float a1 = 1.0f / 3.0f;
		constexpr float a2 = 2.0f / 3.0f;
		constexpr float q  = 3.0f / 4.0f;

		// Update hash grid for fast nearest neighbor particle look up:
		m_pGrid->UpdateGrid(m_positions, m_effectRadius);
		m_pGrid->ApplySort(m_positions);
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
			m_forceDensities[i] = ForceDensity(i, m_positions, m_velocities, m_densities);

		// First Runte-Kutta step:
		for (int i = 0; i < m_particleCount; i++)
		{
			Float2 acceleration = m_forceDensities[i] / m_densities[i];
			m_kp1[i] = m_velocities[i];
			m_kv1[i] = acceleration;
			m_tempPositions[i] = m_positions[i] + q * m_kp1[i] * deltaT;
			m_tempVelocities[i] = m_velocities[i] + q * m_kv1[i] * deltaT;
			float speed = m_tempVelocities[i].Length();
			if (speed > m_maxVelocity)
				m_tempVelocities[i] *= (m_maxVelocity / speed);
		}

		// Update hash grid for fast nearest neighbor particle look up:
		m_pGrid->UpdateGrid(m_tempPositions, m_effectRadius);
		m_pGrid->ApplySort(m_tempPositions);
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
			m_forceDensities[i] = ForceDensity(i, m_tempPositions, m_tempVelocities, m_densities);

		// Second Runge-Kutta step:
		for (int i = 0; i < m_particleCount; i++)
		{
			Float2 acceleration = m_forceDensities[i] / m_densities[i];		// a(tempPos, tempVel)
			m_kp2[i] = m_tempVelocities[i];
			m_kv2[i] = acceleration;
			m_positions[i] += (a1 * m_kp1[i] + a2 * m_kp2[i]) * deltaT;
			m_velocities[i] += (a1 * m_kv1[i] + a2 * m_kv2[i]) * deltaT;
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
			m_reset = true;
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
			m_ringMesh = MeshGenerator::ArcFlatUv(attractorRadius - 0.1f, attractorRadius + 0.1f, 360.0f, 100, "attractorRing");
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
	void SphFluid2dCpu::SetColorMode(int colorMode)
	{
		colorMode = math::Clamp(colorMode, 0, 3);
		if (m_colorMode != colorMode)
			m_colorMode = colorMode;
	}
	void SphFluid2dCpu::SetInitialDistributionRadius(float initialDistributionRadius)
	{
		if (m_initialDistributionRadius != initialDistributionRadius)
		{
			m_initialDistributionRadius = initialDistributionRadius;
			m_reset = true;
		}
	}
	void SphFluid2dCpu::SetVisualRadius(float visualRadius)
	{
		visualRadius = math::Max(0.01f, visualRadius);
		if (m_visualRadius != visualRadius)
		{
			m_visualRadius = visualRadius;
			m_particleMesh = MeshGenerator::UnitQuad().Scale(m_visualRadius);
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
	int SphFluid2dCpu::GetColorMode() const
	{
		return m_colorMode;
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
		if (EventSystem::KeyDown(Input::Key::Space))
		{
			m_isRunning = !m_isRunning;
			if (m_isRunning)
				LOG_TRACE("Simulation running.");
			else
				LOG_TRACE("Simulation stopped.");
		}
		if (EventSystem::KeyDown(Input::Key::Delete))
		{
			m_isRunning = false;
			m_reset = true;
			LOG_TRACE("Simulation stopped and reset.");
		}

		// Reset:
		if (m_reset)
		{
			Reset();
			return;
		}

		// Mouse scrolling:
		float mouseScroll = EventSystem::MouseScrollY();
		if (mouseScroll != 0)
		{
			if (EventSystem::KeyDown(Input::Key::ShiftLeft))
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
		if (EventSystem::MouseHeld(Input::MouseButton::Left) ^ EventSystem::MouseHeld(Input::MouseButton::Right)) // exlusive or.
		{
			Ray ray = Scene::GetActiveScene()->GetActiveCamera()->GetClickRay();
			std::optional<Float3> hit = m_fluidBounds.IntersectRay(ray);
			if (hit.has_value())
			{
				m_attractorPoint = Float2(hit.value());
				ShaderProperties shaderProperties = Renderer::DrawMesh(m_ringMesh, MaterialManager::GetMaterial("simpleUnlitMaterial"), hit.value(), Float3x3::identity, 1.0f, false, false);
				shaderProperties.SetValue("SurfaceProperties", "diffuseColor", Float4::red);
				if (EventSystem::MouseHeld(Input::MouseButton::Left))
					m_attractorState = 1;
				if (EventSystem::MouseHeld(Input::MouseButton::Right))
					m_attractorState = -1;
			}
			else
				m_attractorState = 0;
		}
		else
			m_attractorState = 0;


		// Rendering:
		Float4x4 localToWorld = GetTransform()->GetLocalToWorldMatrix();
		Renderer::DrawBounds(localToWorld, m_fluidBounds, 0.01f, Float4::white, false, false);
		for (int i = 0; i < m_particleCount; i++)
		{
			Float4x4 matrix = localToWorld * Float4x4::Translate(Float3(m_positions[i]));
			ShaderProperties shaderProperties = Renderer::DrawMesh(m_particleMesh, m_particleMaterial, matrix, false, false);
		
			// Color by density:
			if (m_colorMode == 0)
			{
				float t = (m_densities[i] - m_targetDensity) / m_targetDensity;
				float t0 = math::Clamp(t, 0.0f, 1.0f);
				float t1 = math::Clamp(t - 1.0f, 0.0f, 1.0f);
				Float4 colorA = t0 * Float4::white + (1.0f - t0) * Float4::blue;
				Float4 colorB = t1 * Float4::red + (1.0f - t1) * Float4::white;
				Float4 color = (t < 1.0f) ? colorA : colorB;
				shaderProperties.SetValue("SurfaceProperties", "diffuseColor", color);
			}
    		// Color by velocity:
			else if (m_colorMode == 1)
			{
				float t = m_velocities[i].Length() / m_maxVelocity;
        		float t0 = 2.0f * t;
        		float t1 = 2.0f * t - 1.0f;
				Float4 colorA = t0 * Float4::white + (1.0f - t0) * Float4::blue;
				Float4 colorB = t1 * Float4::red + (1.0f - t1) * Float4::white;
				Float4 color = (t < 0.5f) ? colorA : colorB;
				shaderProperties.SetValue("SurfaceProperties", "diffuseColor", color);
			}
			// Color by normal:
			else if (m_colorMode == 2)
			{
				Float4 color = Float4(m_normals[i].x, m_normals[i].y, 0, 1);
				shaderProperties.SetValue("SurfaceProperties", "diffuseColor", color);
			}
			// Color by curvature:
			else if (m_colorMode == 3)
			{
        		Float4 color = (0.5f + m_curvatures[i]) * Float4(0, 0, 1, 1);
				shaderProperties.SetValue("SurfaceProperties", "diffuseColor", color);
			}
		}
	}



	// Private methods:
	// Physics:
	float SphFluid2dCpu::Density(int particleIndex, const std::vector<Float2>& positions)
	{
		float density = 0;
		Float2 particlePos = positions[particleIndex];
		if (m_useGridOptimization)
		{
			Int2 particleCell = m_pGrid->Cell(particlePos, m_effectRadius);
			for (const Int2& offset : s_offsets)
			{
				Int2 neighbourCell = particleCell + offset;
				const uint32_t cellKey = m_pGrid->GetCellKey(neighbourCell);
				uint32_t otherIndex = m_pGrid->GetStartIndex(neighbourCell);

				// Skip empty cells:
				if (otherIndex == uint32_t(-1) || otherIndex >= m_particleCount)
					continue;

				while (otherIndex < m_particleCount && m_pGrid->GetCellKey(otherIndex) == cellKey)
				{
					Float2 otherPos = positions[otherIndex];
					Float2 offset = particlePos - otherPos;
					float r = offset.Length();
					if (r < m_effectRadius)
						density += m_mass * smoothingKernals::Poly6(r, m_effectRadius);
					otherIndex++;
				}
			}
		}
		else
		{
			for (int i = 0; i < m_particleCount; i++)
			{
				Float2 offset = particlePos - positions[i];
				float r = offset.Length();
				if (r < m_effectRadius)
					density += m_mass * smoothingKernals::Poly6(r, m_effectRadius);
			}
		}
		return density;
	}
	Float2 SphFluid2dCpu::Normal(int particleIndex, const std::vector<Float2>& positions, const std::vector<float>& densities)
	{
		Float2 normal = Float2::zero;
		if (m_useGridOptimization)
		{
			Float2 particlePos = positions[particleIndex];
			Int2 particleCell = m_pGrid->Cell(particlePos, m_effectRadius);
			for (const Int2& offset : s_offsets)
			{
				Int2 neighbourCell = particleCell + offset;
				const uint32_t cellKey = m_pGrid->GetCellKey(neighbourCell);
				uint32_t otherIndex = m_pGrid->GetStartIndex(neighbourCell);

				// Skip empty cells:
				if (otherIndex == uint32_t(-1) || otherIndex >= m_particleCount)
					continue;

				while (otherIndex < m_particleCount && m_pGrid->GetCellKey(otherIndex) == cellKey)
				{
					if (otherIndex == particleIndex)
					{
						otherIndex++;
						continue;
					}

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
		}
		else
		{
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
		}
		return normal;
	}
	float SphFluid2dCpu::Curvature(int particleIndex, const std::vector<Float2>& positions, const std::vector<Float2>& normals, const std::vector<float>& densities)
	{
		float curvature = 0.0f;
		Float2 particlePos = positions[particleIndex];
		if (m_useGridOptimization)
		{
			Int2 particleCell = m_pGrid->Cell(particlePos, m_effectRadius);
			for (const Int2& offset : s_offsets)
			{
				Int2 neighbourCell = particleCell + offset;
				const uint32_t cellKey = m_pGrid->GetCellKey(neighbourCell);
				uint32_t otherIndex = m_pGrid->GetStartIndex(neighbourCell);

				// Skip empty cells:
				if (otherIndex == uint32_t(-1) || otherIndex >= m_particleCount)
					continue;

				while (otherIndex < m_particleCount && m_pGrid->GetCellKey(otherIndex) == cellKey)
				{
					if (otherIndex == particleIndex)
					{
						otherIndex++;
						continue;
					}

					Float2 otherPos = positions[otherIndex];
					Float2 offset = particlePos - otherPos;
					float r = offset.Length();
					if (r < m_effectRadius && r > 1e-8f)
						curvature -= m_mass / densities[otherIndex] * smoothingKernals::DDSpiky(r, m_effectRadius) / normals[otherIndex].Length();
					otherIndex++;
				}
			}
		}
		else
		{
			for (int i = 0; i < m_particleCount; i++)
			{
				if (i == particleIndex)
					continue;

				Float2 offset = particlePos - positions[i];
				float r = offset.Length();
				if (r < m_effectRadius && r > 1e-8f)
					curvature -= m_mass / densities[i] * smoothingKernals::DDSpiky(r, m_effectRadius) / normals[i].Length();
			}
		}
		return curvature;
	}
	float Pressure(float density, float targetDensity, float pressureMultiplier)
	{
		float densityError = density - targetDensity;
		return densityError * pressureMultiplier;
		//return pressureMultiplier * (math::Pow(density / targetDensity, 7.0f) - 1.0f);
	}
	Float2 SphFluid2dCpu::ForceDensity(int particleIndex, const std::vector<Float2>& positions, const std::vector<Float2>& velocities, const std::vector<float>& densities)
	{
		Float2 forceDensity = Float2::zero;
		float particlePressure = Pressure(densities[particleIndex], m_targetDensity, m_pressureMultiplier);
		Float2 particlePos = positions[particleIndex];
		Float2 particleVel = velocities[particleIndex];

		// Internal interactions (particle-particle):
		if (m_useGridOptimization)
		{
			Int2 particleCell = m_pGrid->Cell(particlePos, m_effectRadius);
			for (const Int2& offset : s_offsets)
			{
				Int2 neighbourCell = particleCell + offset;
				const uint32_t cellKey = m_pGrid->GetCellKey(neighbourCell);
				uint32_t otherIndex = m_pGrid->GetStartIndex(neighbourCell);

				// Skip empty cells:
				if (otherIndex == uint32_t(-1) || otherIndex >= m_particleCount)
					continue;

				while (otherIndex < m_particleCount && m_pGrid->GetCellKey(otherIndex) == cellKey)
				{
					if (otherIndex == particleIndex)
					{
						otherIndex++;
						continue;
					}

					Float2 otherPos = positions[otherIndex];
					Float2 otherVel = velocities[otherIndex];
					Float2 offset = particlePos - otherPos;
					float r = offset.Length();
					if (r < m_effectRadius)
					{
						// Pressure force density:
						Float2 dir = (r < 1e-8f) ? math::Random::UniformDirection2() : offset / r;
						float otherParticlePressure = Pressure(densities[otherIndex], m_targetDensity, m_pressureMultiplier);
						float sharedPressure = 0.5f * (particlePressure + otherParticlePressure);
						forceDensity += -m_mass * sharedPressure * smoothingKernals::DSpiky(r, dir, m_effectRadius) / densities[otherIndex];

						// Viscosity force density:
						Float2 velocityDiff = otherVel - particleVel;
						forceDensity += (m_mass * smoothingKernals::DDViscos(r, m_effectRadius) / densities[otherIndex]) * velocityDiff;
					}
					otherIndex++;
				}
			}
		}
		else
		{
			for (int i = 0; i < m_particleCount; i++)
			{
				if (i == particleIndex)
					continue;

				Float2 offset = particlePos - positions[i];
				float r = offset.Length();
				if (r < m_effectRadius)
				{
					// Pressure force density:
					Float2 dir = (r < 1e-8f) ? math::Random::UniformDirection2() : offset / r;
					float otherParticlePressure = Pressure(densities[i], m_targetDensity, m_pressureMultiplier);
					float sharedPressure = 0.5f * (particlePressure + otherParticlePressure);
					forceDensity += -m_mass * sharedPressure * smoothingKernals::DSpiky(r, dir, m_effectRadius) / densities[i];

					// Viscosity force density:
					Float2 velocityDiff = velocities[i] - particleVel;
					forceDensity += (m_mass * smoothingKernals::DDViscos(r, m_effectRadius) / densities[i]) * velocityDiff;
				}
			}
		}

		// External interactions:
		{
			// Gravity force density:
			forceDensity += densities[particleIndex] * Float2(0.0f, -m_gravity);

			// External force density:
			Float2 offset = m_attractorPoint - positions[particleIndex];
			float r = offset.Length();
			if (r < m_attractorRadius && r > 1e-8f)
			{
				r /= m_attractorRadius;
				forceDensity += m_attractorState * m_attractorStrength * densities[particleIndex] * offset * r * r;
			}
		}
		return forceDensity;
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