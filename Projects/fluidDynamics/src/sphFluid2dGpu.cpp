#include "sphFluid2dGpu.h"
#include "sphFluid2dGpuEditorWindow.h"
#include "vulkanUtility.h"



namespace emberEngine
{// Constructor/Destructor:
	SphFluid2dGpu::SphFluid2dGpu()
	{
		// Load compute shaders and materials:
		std::string directoryPath = (std::string)PROJECT_ROOT_PATH + "/src/shaders/bin";
		cs_pReset = std::make_unique<ComputeShader>("reset", directoryPath + "/reset.comp.spv");
		cs_pDensity = std::make_unique<ComputeShader>("density", directoryPath + "/density.comp.spv");
		cs_pPressureForceDensity = std::make_unique<ComputeShader>("pressureForceDensity", directoryPath + "/pressureForceDensity.comp.spv");
		cs_pBoundaryCollisions = std::make_unique<ComputeShader>("boundaryCollisions", directoryPath + "/boundaryCollisions.comp.spv");
		m_pResetProperties = std::make_unique<ShaderProperties>((Shader*)cs_pReset.get());
		m_pDensityProperties = std::make_unique<ShaderProperties>((Shader*)cs_pDensity.get());
		m_pPressureForceDensityProperties = std::make_unique<ShaderProperties>((Shader*)cs_pPressureForceDensity.get());
		m_pBoundaryCollisionsProperties = std::make_unique<ShaderProperties>((Shader*)cs_pBoundaryCollisions.get());
		m_pParticleMaterial = MaterialManager::GetMaterial("particleMaterial");
		m_pShaderProperties = std::make_unique<ShaderProperties>((Shader*)m_pParticleMaterial);

		// Management:
		m_isRunning = false;
		m_timeScale = 2.0f;
		m_timeStep = 0;
		SetParticleCount(400);
		m_useGridOptimization = true;

		// Physics:
		SetEffectRadius(0.5f);
		SetMass(1.0f);
		m_viscosity = 0.5f;
		m_surfaceTension = 0.07f;
		SetCollisionDampening(0.95f);
		SetTargetDensity(15.0f);
		m_pressureMultiplier = 20.0f;
		m_gravity = 0.5f;
		m_maxVelocity = 5.0f;

		// User Interaction/Boundaries:
		SetAttractorRadius(3.0f);
		m_attractorStrength = 2.0f;
		SetFluidBounds(Bounds(Float3::zero, Float3(16.0f, 9.0f, 0.01f)));

		// Visuals:
		SetInitialDistributionRadius(6.0f);
		m_visualRadius = 0.25f;
		m_pParticleMesh = std::unique_ptr<Mesh>(MeshGenerator::UnitQuad()->Scale(m_visualRadius));

		// Editor window:
		editorWindow = std::make_unique<SphFluid2dGpuEditorWindow>(this);
	}
	SphFluid2dGpu::~SphFluid2dGpu()
	{

	}



	// Physics update:
	void SphFluid2dGpu::Reset()
	{
		m_timeStep = 0;

		// Reallocate buffers:
		if (m_pPositionBuffer == nullptr || m_pPositionBuffer->GetCount() != m_particleCount)
		{
			std::unique_ptr<StorageBuffer> pNewPositionBuffer = std::make_unique<StorageBuffer>(m_particleCount, sizeof(Float2));
			std::swap(m_pPositionBuffer, pNewPositionBuffer);
			std::unique_ptr<StorageBuffer> pNewVelocityBuffer = std::make_unique<StorageBuffer>(m_particleCount, sizeof(Float2));
			std::swap(m_pVelocityBuffer, pNewVelocityBuffer);
			std::unique_ptr<StorageBuffer> pNewDensityBuffer = std::make_unique<StorageBuffer>(m_particleCount, sizeof(float));
			std::swap(m_pDensityBuffer, pNewDensityBuffer);
			std::unique_ptr<StorageBuffer> pNewForceDensityBuffer = std::make_unique<StorageBuffer>(m_particleCount, sizeof(Float2));
			std::swap(m_pForceDensityBuffer, pNewForceDensityBuffer);
			std::unique_ptr<StorageBuffer> pNewKp1Buffer = std::make_unique<StorageBuffer>(m_particleCount, sizeof(Float2));
			std::swap(m_pKp1Buffer, pNewKp1Buffer);
			std::unique_ptr<StorageBuffer> pNewKp2Buffer = std::make_unique<StorageBuffer>(m_particleCount, sizeof(Float2));
			std::swap(m_pKp2Buffer, pNewKp2Buffer);
			std::unique_ptr<StorageBuffer> pNewKv1Buffer = std::make_unique<StorageBuffer>(m_particleCount, sizeof(Float2));
			std::swap(m_pKv1Buffer, pNewKv1Buffer);
			std::unique_ptr<StorageBuffer> pNewKv2Buffer = std::make_unique<StorageBuffer>(m_particleCount, sizeof(Float2));
			std::swap(m_pKv2Buffer, pNewKv2Buffer);
			std::unique_ptr<StorageBuffer> pNewTempPositionBuffer = std::make_unique<StorageBuffer>(m_particleCount, sizeof(Float2));
			std::swap(m_pTempPositionBuffer, pNewTempPositionBuffer);
			std::unique_ptr<StorageBuffer> pNewTempVelocityBuffer = std::make_unique<StorageBuffer>(m_particleCount, sizeof(Float2));
			std::swap(m_pTempVelocityBuffer, pNewTempVelocityBuffer);
		}

		// Reset fluid to initial data:
		DispatchResetKernal();
		Compute::Barrier(AccessMask::ComputeShader::shaderWrite, AccessMask::ComputeShader::shaderRead);
		DispatchDensityKernal(m_pPositionBuffer.get());
	}
	void SphFluid2dGpu::FixedUpdate()
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
	void SphFluid2dGpu::TimeStepLeapFrog(float deltaT)
	{

	}
	void SphFluid2dGpu::TimeStepVelocityVerlet(float deltaT)
	{

	}
	void SphFluid2dGpu::TimeStepRungeKutta2(float deltaT)
	{
		// Update grid for fast nearest neighbor particle look up:
		//m_pGrid->UpdateGrid(m_positions, m_effectRadius);
		//m_pGrid->ApplySort(m_velocities);

		// Compute densities:
		DispatchDensityKernal(m_pPositionBuffer.get());
		Compute::Barrier(AccessMask::ComputeShader::shaderWrite, AccessMask::ComputeShader::shaderRead);

		// Compute forces:
		DispatchPressureForceDensityKernal(m_pPositionBuffer.get());
		Compute::Barrier(AccessMask::ComputeShader::shaderWrite, AccessMask::ComputeShader::shaderRead);

		//// First Runte-Kutta step:
		//for (int i = 0; i < m_particleCount; i++)
		//{
		//	Float2 acceleration = m_forceDensities[i] / m_densities[i];
		//	m_kp1[i] = m_velocities[i] * deltaT;
		//	m_kv1[i] = acceleration * deltaT;
		//	m_tempPositions[i] = m_positions[i] + 0.5f * m_kp1[i];
		//	m_tempVelocities[i] = m_velocities[i] + 0.5f * m_kv1[i];
		//	float speed = m_tempVelocities[i].Length();
		//	if (speed > m_maxVelocity)
		//		m_tempVelocities[i] *= (m_maxVelocity / speed);
		//}
		DispatchRungeKutta2Step1Kernal();
		Compute::Barrier(AccessMask::ComputeShader::shaderWrite, AccessMask::ComputeShader::shaderRead);

		//// Update grid for fast nearest neighbor particle look up:
		//m_pGrid->UpdateGrid(m_tempPositions, m_effectRadius);
		//m_pGrid->ApplySort(m_tempVelocities);
		//m_pGrid->ApplySort(m_positions);
		//m_pGrid->ApplySort(m_velocities);
		//m_pGrid->ApplySort(m_kv1);

		// Compute intermediate densities:
		DispatchDensityKernal(m_pTempPositionBuffer.get());
		Compute::Barrier(AccessMask::ComputeShader::shaderWrite, AccessMask::ComputeShader::shaderRead);

		// Compute intermediate force densities:
		DispatchPressureForceDensityKernal(m_pTempPositionBuffer.get());
		Compute::Barrier(AccessMask::ComputeShader::shaderWrite, AccessMask::ComputeShader::shaderRead);

		//// Second Runge-Kutta step:
		//for (int i = 0; i < m_particleCount; i++)
		//{
		//	Float2 acceleration = m_forceDensities[i] / m_densities[i];
		//	m_positions[i] += (m_velocities[i] + 0.5f * m_kv1[i]) * deltaT;
		//	m_velocities[i] += acceleration * deltaT;
		//	float speed = m_velocities[i].Length();
		//	if (speed > m_maxVelocity)
		//		m_velocities[i] *= (m_maxVelocity / speed);
		//	BoundaryCollisions(m_positions[i], m_velocities[i]);
		//}
		DispatchRungeKutta2Step2Kernal();
		Compute::Barrier(AccessMask::ComputeShader::shaderWrite, AccessMask::ComputeShader::shaderRead);

		DispatchBoundaryCollisionsKernal();
	}



	// Setters:
	void SphFluid2dGpu::SetIsRunning(bool isRunning)
	{
		m_isRunning = isRunning;
	}
	void SphFluid2dGpu::SetTimeScale(float timeScale)
	{
		m_timeScale = timeScale;
	}
	void SphFluid2dGpu::SetUseGridOptimization(bool useGridOptimization)
	{
		m_useGridOptimization = useGridOptimization;
	}
	void SphFluid2dGpu::SetParticleCount(int particleCount)
	{
		if (m_particleCount != particleCount)
		{
			m_particleCount = particleCount;
			m_threadCount = Uint3(m_particleCount, 1, 1);
			m_pDensityProperties->SetValue("Values", "cb_particleCount", m_particleCount);
			m_pPressureForceDensityProperties->SetValue("Values", "cb_particleCount", m_particleCount);
			Reset();
		}
	}
	void SphFluid2dGpu::SetEffectRadius(float effectRadius)
	{
		if (m_effectRadius != effectRadius)
		{
			m_effectRadius = effectRadius;
			m_pDensityProperties->SetValue("Values", "cb_effectRadius", m_effectRadius);
			m_pPressureForceDensityProperties->SetValue("Values", "cb_effectRadius", m_effectRadius);
		}
	}
	void SphFluid2dGpu::SetMass(float mass)
	{
		if (m_mass != mass)
		{
			m_mass = mass;
			m_pDensityProperties->SetValue("Values", "cb_mass", m_mass);
			m_pPressureForceDensityProperties->SetValue("Values", "cb_mass", m_mass);
		}
	}
	void SphFluid2dGpu::SetViscosity(float viscosity)
	{
		m_viscosity = viscosity;
	}
	void SphFluid2dGpu::SetSurfaceTension(float surfaceTension)
	{
		m_surfaceTension = surfaceTension;
	}
	void SphFluid2dGpu::SetCollisionDampening(float collisionDampening)
	{
		if (m_collisionDampening != collisionDampening)
		{
			m_collisionDampening = collisionDampening;
			m_pBoundaryCollisionsProperties->SetValue("Values", "cb_collisionDampening", m_collisionDampening);
		}
	}
	void SphFluid2dGpu::SetTargetDensity(float targetDensity)
	{
		if (m_targetDensity != targetDensity)
		{
			m_targetDensity = targetDensity;
			m_pShaderProperties->SetValue("Values", "cb_targetDensity", m_targetDensity);
			m_pPressureForceDensityProperties->SetValue("Values", "cb_targetDensity", m_targetDensity);
		}
	}
	void SphFluid2dGpu::SetPressureMultiplier(float pressureMultiplier)
	{
		m_pressureMultiplier = pressureMultiplier;
		m_pPressureForceDensityProperties->SetValue("Values", "cb_pressureMultiplier", m_pressureMultiplier);
	}
	void SphFluid2dGpu::SetGravity(float gravity)
	{
		m_gravity = gravity;
	}
	void SphFluid2dGpu::SetMaxVelocity(float maxVelocity)
	{
		m_maxVelocity = maxVelocity;
	}
	void SphFluid2dGpu::SetAttractorRadius(float attractorRadius)
	{
		attractorRadius = math::Max(0.01f, attractorRadius);
		if (m_attractorRadius != attractorRadius)
		{
			std::unique_ptr<Mesh> pNewRingMesh = std::unique_ptr<Mesh>(MeshGenerator::ArcFlatUv(attractorRadius - 0.1f, attractorRadius + 0.1f, 360.0f, 100, "attractorRing"));
			std::swap(m_pRingMesh, pNewRingMesh);
			m_attractorRadius = attractorRadius;
		}
	}
	void SphFluid2dGpu::SetAttractorStrength(float attractorStrength)
	{
		attractorStrength = math::Max(0.01f, attractorStrength);
		m_attractorStrength = attractorStrength;
	}
	void SphFluid2dGpu::SetFluidBounds(const Bounds& bounds)
	{
		if (m_fluidBounds != bounds)
		{
			m_fluidBounds = bounds;
			m_pBoundaryCollisionsProperties->SetValue("Values", "cb_min", m_fluidBounds.GetMin());
			m_pBoundaryCollisionsProperties->SetValue("Values", "cb_max", m_fluidBounds.GetMax());
		}
	}
	void SphFluid2dGpu::SetInitialDistributionRadius(float initialDistributionRadius)
	{
		if (m_initialDistributionRadius != initialDistributionRadius)
		{
			m_initialDistributionRadius = initialDistributionRadius;
			m_pResetProperties->SetValue("Values", "cb_initialDistributionRadius", m_initialDistributionRadius);
			Reset();
		}
	}
	void SphFluid2dGpu::SetVisualRadius(float visualRadius)
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
	bool SphFluid2dGpu::GetIsRunning() const
	{
		return m_isRunning;
	}
	float SphFluid2dGpu::GetTimeScale() const
	{
		return m_timeScale;
	}
	bool SphFluid2dGpu::GetUseGridOptimization() const
	{
		return m_useGridOptimization;
	}
	uint32_t SphFluid2dGpu::GetTimeStep() const
	{
		return m_timeStep;
	}
	int SphFluid2dGpu::GetParticleCount() const
	{
		return m_particleCount;
	}
	float SphFluid2dGpu::GetEffectRadius() const
	{
		return m_effectRadius;
	}
	float SphFluid2dGpu::GetMass() const
	{
		return m_mass;
	}
	float SphFluid2dGpu::GetViscosity() const
	{
		return m_viscosity;
	}
	float SphFluid2dGpu::GetSurfaceTension() const
	{
		return m_surfaceTension;
	}
	float SphFluid2dGpu::GetCollisionDampening() const
	{
		return m_collisionDampening;
	}
	float SphFluid2dGpu::GetTargetDensity() const
	{
		return m_targetDensity;
	}
	float SphFluid2dGpu::GetPressureMultiplier() const
	{
		return m_pressureMultiplier;
	}
	float SphFluid2dGpu::GetGravity() const
	{
		return m_gravity;
	}
	float SphFluid2dGpu::GetMaxVelocity() const
	{
		return m_maxVelocity;
	}
	float SphFluid2dGpu::GetAttractorRadius() const
	{
		return m_attractorRadius;
	}
	float SphFluid2dGpu::GetAttractorStrength() const
	{
		return m_attractorStrength;
	}
	Bounds SphFluid2dGpu::GetFluidBounds() const
	{
		return m_fluidBounds;
	}
	float SphFluid2dGpu::GetInitialDistributionRadius() const
	{
		return m_initialDistributionRadius;
	}
	float SphFluid2dGpu::GetVisualRadius() const
	{
		return m_visualRadius;
	}



	// Overrides:
	void SphFluid2dGpu::Start()
	{
		Reset();
	}
	void SphFluid2dGpu::Update()
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

		m_pShaderProperties->SetStorageBuffer("b_positions", m_pPositionBuffer.get());
		m_pShaderProperties->SetStorageBuffer("b_densities", m_pDensityBuffer.get());
		Graphics::DrawInstanced(m_particleCount, m_pPositionBuffer.get(), m_pParticleMesh.get(), m_pParticleMaterial, m_pShaderProperties.get(), localToWorld, false, false);
	}
	const std::string SphFluid2dGpu::ToString() const
	{
		return "SphFluid2dGpu";
	}



	// Private methods:
	// Physics:
	void SphFluid2dGpu::DispatchResetKernal()
	{
		m_pResetProperties->SetStorageBuffer("b_positions", m_pPositionBuffer.get());
		m_pResetProperties->SetStorageBuffer("b_velocities", m_pVelocityBuffer.get());
		Compute::Dispatch(cs_pReset.get(), m_pResetProperties.get(), m_threadCount);
	}
	void SphFluid2dGpu::DispatchDensityKernal(StorageBuffer* positionsBuffer)
	{
		m_pDensityProperties->SetStorageBuffer("b_positions", positionsBuffer);
		m_pDensityProperties->SetStorageBuffer("b_densities", m_pDensityBuffer.get());
		Compute::Dispatch(cs_pDensity.get(), m_pDensityProperties.get(), m_threadCount);
	}
	void SphFluid2dGpu::DispatchNormalKernal()
	{

	}
	void SphFluid2dGpu::DispatchCurvatureKernal()
	{

	}
	void SphFluid2dGpu::DispatchPressureForceDensityKernal(StorageBuffer* positionsBuffer)
	{
		m_pPressureForceDensityProperties->SetStorageBuffer("b_positions", positionsBuffer);
		m_pPressureForceDensityProperties->SetStorageBuffer("b_densities", m_pDensityBuffer.get());
		m_pPressureForceDensityProperties->SetStorageBuffer("b_forceDensities", m_pForceDensityBuffer.get());
		Compute::Dispatch(cs_pPressureForceDensity.get(), m_pPressureForceDensityProperties.get(), m_threadCount);
	}
	void SphFluid2dGpu::DispatchViscosityForceDensityKernal()
	{

	}
	void SphFluid2dGpu::DispatchSurfaceTensionForceDensityKernal()
	{

	}
	void SphFluid2dGpu::DispatchGravityForceDensityKernal()
	{

	}
	void SphFluid2dGpu::DispatchExternalForceDensityKernal()
	{

	}
	void SphFluid2dGpu::DispatchRungeKutta2Step1Kernal()
	{

	}
	void SphFluid2dGpu::DispatchRungeKutta2Step2Kernal()
	{

	}
	void SphFluid2dGpu::DispatchBoundaryCollisionsKernal()
	{
		m_pBoundaryCollisionsProperties->SetStorageBuffer("b_positions", m_pPositionBuffer.get());
		m_pBoundaryCollisionsProperties->SetStorageBuffer("b_velocities", m_pVelocityBuffer.get());
		Compute::Dispatch(cs_pBoundaryCollisions.get(), m_pBoundaryCollisionsProperties.get(), m_threadCount);
	}
}