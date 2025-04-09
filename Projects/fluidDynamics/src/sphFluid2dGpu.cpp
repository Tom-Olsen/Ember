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
		cs_pViscosityForceDensity = std::make_unique<ComputeShader>("viscosityForceDensity", directoryPath + "/viscosityForceDensity.comp.spv");
		cs_pExternalForceDensity = std::make_unique<ComputeShader>("externalForceDensity", directoryPath + "/externalForceDensity.comp.spv");
		cs_pGravityForceDensity = std::make_unique<ComputeShader>("gravityForceDensity", directoryPath + "/gravityForceDensity.comp.spv");
		cs_pRungeKutta2Step1 = std::make_unique<ComputeShader>("rungeKutta2Step1", directoryPath + "/rungeKutta2Step1.comp.spv");
		cs_pRungeKutta2Step2 = std::make_unique<ComputeShader>("rungeKutta2Step2", directoryPath + "/rungeKutta2Step2.comp.spv");
		cs_pBoundaryCollisions = std::make_unique<ComputeShader>("boundaryCollisions", directoryPath + "/boundaryCollisions.comp.spv");
		m_pResetProperties = std::make_unique<ShaderProperties>((Shader*)cs_pReset.get());
		for (int i = 0; i < 2; i++)
		{
			m_densityProperties[i] = std::make_unique<ShaderProperties>((Shader*)cs_pDensity.get());
			m_pressureForceDensityProperties[i] = std::make_unique<ShaderProperties>((Shader*)cs_pPressureForceDensity.get());
			m_viscosityForceDensityProperties[i] = std::make_unique<ShaderProperties>((Shader*)cs_pViscosityForceDensity.get());
			m_externalForceDensityProperties[i] = std::make_unique<ShaderProperties>((Shader*)cs_pExternalForceDensity.get());
		}
		m_pGravityForceDensityProperties = std::make_unique<ShaderProperties>((Shader*)cs_pGravityForceDensity.get());
		m_pRungeKutta2Step1Properties = std::make_unique<ShaderProperties>((Shader*)cs_pRungeKutta2Step1.get());
		m_pRungeKutta2Step2Properties = std::make_unique<ShaderProperties>((Shader*)cs_pRungeKutta2Step2.get());
		m_pBoundaryCollisionsProperties = std::make_unique<ShaderProperties>((Shader*)cs_pBoundaryCollisions.get());
		m_pParticleMaterial = MaterialManager::GetMaterial("particleMaterial");
		m_pShaderProperties = std::make_unique<ShaderProperties>((Shader*)m_pParticleMaterial);

		// Management:
		m_isRunning = false;
		m_reset = true;
		m_timeScale = 2.0f;
		m_useGridOptimization = true;
		m_timeStep = 0;
		SetParticleCount(2000);

		// Physics:
		SetEffectRadius(0.5f);
		SetMass(1.0f);
		SetViscosity(0.5f);
		m_surfaceTension = 0.07f;
		SetCollisionDampening(0.95f);
		SetTargetDensity(15.0f);
		SetPressureMultiplier(5.0f);
		SetGravity(0.5f);
		SetMaxVelocity(5.0f);

		// User Interaction/Boundaries:
		SetAttractorRadius(3.0f);
		SetAttractorStrength(2.0f);
		SetFluidBounds(Bounds(Float3::zero, Float3(16.0f, 9.0f, 0.01f)));

		// Visuals:
		SetColorMode(1);
		SetInitialDistributionRadius(6.0f);
		SetVisualRadius(0.25f);

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
			std::unique_ptr<StorageBuffer> pNewPositionBuffer = std::make_unique<StorageBuffer>((uint32_t)m_particleCount, (uint32_t)sizeof(Float2));
			std::swap(m_pPositionBuffer, pNewPositionBuffer);
			std::unique_ptr<StorageBuffer> pNewVelocityBuffer = std::make_unique<StorageBuffer>((uint32_t)m_particleCount, (uint32_t)sizeof(Float2));
			std::swap(m_pVelocityBuffer, pNewVelocityBuffer);
			std::unique_ptr<StorageBuffer> pNewDensityBuffer = std::make_unique<StorageBuffer>((uint32_t)m_particleCount, (uint32_t)sizeof(float));
			std::swap(m_pDensityBuffer, pNewDensityBuffer);
			std::unique_ptr<StorageBuffer> pNewForceDensityBuffer = std::make_unique<StorageBuffer>((uint32_t)m_particleCount, (uint32_t)sizeof(Float2));
			std::swap(m_pForceDensityBuffer, pNewForceDensityBuffer);
			std::unique_ptr<StorageBuffer> pNewKp1Buffer = std::make_unique<StorageBuffer>((uint32_t)m_particleCount, (uint32_t)sizeof(Float2));
			std::swap(m_pKp1Buffer, pNewKp1Buffer);
			std::unique_ptr<StorageBuffer> pNewKv1Buffer = std::make_unique<StorageBuffer>((uint32_t)m_particleCount, (uint32_t)sizeof(Float2));
			std::swap(m_pKv1Buffer, pNewKv1Buffer);
			std::unique_ptr<StorageBuffer> pNewKp2Buffer = std::make_unique<StorageBuffer>((uint32_t)m_particleCount, (uint32_t)sizeof(Float2));
			std::swap(m_pKp2Buffer, pNewKp2Buffer);
			std::unique_ptr<StorageBuffer> pNewKv2Buffer = std::make_unique<StorageBuffer>((uint32_t)m_particleCount, (uint32_t)sizeof(Float2));
			std::swap(m_pKv2Buffer, pNewKv2Buffer);
			std::unique_ptr<StorageBuffer> pNewTempPositionBuffer = std::make_unique<StorageBuffer>((uint32_t)m_particleCount, (uint32_t)sizeof(Float2));
			std::swap(m_pTempPositionBuffer, pNewTempPositionBuffer);
			std::unique_ptr<StorageBuffer> pNewTempVelocityBuffer = std::make_unique<StorageBuffer>((uint32_t)m_particleCount, (uint32_t)sizeof(Float2));
			std::swap(m_pTempVelocityBuffer, pNewTempVelocityBuffer);
		}

		// Reset fluid to initial data:
		DispatchResetKernal();
		Compute::Barrier(AccessMask::ComputeShader::shaderWrite, AccessMask::ComputeShader::shaderRead);
		DispatchDensityKernal(m_pPositionBuffer.get(), m_densityProperties[0].get());
		Compute::Barrier(AccessMask::ComputeShader::shaderWrite, AccessMask::ComputeShader::shaderRead);
	}
	void SphFluid2dGpu::FixedUpdate()
	{
		if (!m_isRunning || m_reset)
			return;

		// Do multiple iterations of deltaT<=dt if timeScale is bigger 1. Otherwise 1 iteration per FixedUpdate().
		float dt = 2.0f * Time::GetFixedDeltaTime();
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
		static int frame = 0;
		m_pRungeKutta2Step1Properties->SetValue("Values", "cb_deltaT", deltaT);
		m_pRungeKutta2Step2Properties->SetValue("Values", "cb_deltaT", deltaT);

		// Update grid for fast nearest neighbor particle look up:
		// ...

		// Compute densities:
		DispatchDensityKernal(m_pPositionBuffer.get(), m_densityProperties[0].get());

		// Compute force densities:
		DispatchPressureForceDensityKernal(m_pPositionBuffer.get(), m_pressureForceDensityProperties[0].get());
		DispatchViscosityForceDensityKernal(m_pPositionBuffer.get(), m_pVelocityBuffer.get(), m_viscosityForceDensityProperties[0].get());
		DispatchExternalForceDensityKernal(m_pPositionBuffer.get(), m_externalForceDensityProperties[0].get());
		DispatchGravityForceDensityKernal();

		// First Runte-Kutta step:
		DispatchRungeKutta2Step1Kernal();

		// Update grid for fast nearest neighbor particle look up:
		// ...
		
		// Compute intermediate densities:
		DispatchDensityKernal(m_pTempPositionBuffer.get(), m_densityProperties[1].get());

		// Compute intermediate force densities:
		DispatchPressureForceDensityKernal(m_pTempPositionBuffer.get(), m_pressureForceDensityProperties[1].get());
		DispatchViscosityForceDensityKernal(m_pTempPositionBuffer.get(), m_pTempVelocityBuffer.get(), m_viscosityForceDensityProperties[1].get());
		DispatchExternalForceDensityKernal(m_pTempPositionBuffer.get(), m_externalForceDensityProperties[1].get());
		DispatchGravityForceDensityKernal();
		
		// Second Runge-Kutta step:
		DispatchRungeKutta2Step2Kernal();

		// Resolve boundary collisions:
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
		particleCount = math::Max(1, particleCount);
		if (m_particleCount != particleCount)
		{
			m_particleCount = particleCount;
			m_threadCount = Uint3(m_particleCount, 1, 1);
			m_densityProperties[0]->SetValue("Values", "cb_particleCount", m_particleCount);
			m_densityProperties[1]->SetValue("Values", "cb_particleCount", m_particleCount);
			m_pressureForceDensityProperties[0]->SetValue("Values", "cb_particleCount", m_particleCount);
			m_pressureForceDensityProperties[1]->SetValue("Values", "cb_particleCount", m_particleCount);
			m_viscosityForceDensityProperties[0]->SetValue("Values", "cb_particleCount", m_particleCount);
			m_viscosityForceDensityProperties[1]->SetValue("Values", "cb_particleCount", m_particleCount);
			m_externalForceDensityProperties[0]->SetValue("Values", "cb_particleCount", m_particleCount);
			m_externalForceDensityProperties[1]->SetValue("Values", "cb_particleCount", m_particleCount);
			m_reset = true;
		}
	}
	void SphFluid2dGpu::SetEffectRadius(float effectRadius)
	{
		effectRadius = math::Max(1e-4f, effectRadius);
		if (m_effectRadius != effectRadius)
		{
			m_effectRadius = effectRadius;
			m_densityProperties[0]->SetValue("Values", "cb_effectRadius", m_effectRadius);
			m_densityProperties[1]->SetValue("Values", "cb_effectRadius", m_effectRadius);
			m_pressureForceDensityProperties[0]->SetValue("Values", "cb_effectRadius", m_effectRadius);
			m_pressureForceDensityProperties[1]->SetValue("Values", "cb_effectRadius", m_effectRadius);
			m_viscosityForceDensityProperties[0]->SetValue("Values", "cb_effectRadius", m_effectRadius);
			m_viscosityForceDensityProperties[1]->SetValue("Values", "cb_effectRadius", m_effectRadius);
		}
	}
	void SphFluid2dGpu::SetMass(float mass)
	{
		mass = math::Max(1e-4f, mass);
		if (m_mass != mass)
		{
			m_mass = mass;
			m_densityProperties[0]->SetValue("Values", "cb_mass", m_mass);
			m_densityProperties[1]->SetValue("Values", "cb_mass", m_mass);
			m_pressureForceDensityProperties[0]->SetValue("Values", "cb_mass", m_mass);
			m_pressureForceDensityProperties[1]->SetValue("Values", "cb_mass", m_mass);
			m_viscosityForceDensityProperties[0]->SetValue("Values", "cb_mass", m_mass);
			m_viscosityForceDensityProperties[1]->SetValue("Values", "cb_mass", m_mass);
		}
	}
	void SphFluid2dGpu::SetViscosity(float viscosity)
	{
		if (m_viscosity != viscosity)
		{
			m_viscosity = viscosity;
			m_viscosityForceDensityProperties[0]->SetValue("Values", "cb_viscosity", m_viscosity);
			m_viscosityForceDensityProperties[1]->SetValue("Values", "cb_viscosity", m_viscosity);
		}
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
		targetDensity = math::Max(1e-4f, targetDensity);
		if (m_targetDensity != targetDensity)
		{
			m_targetDensity = targetDensity;
			m_pShaderProperties->SetValue("Values", "cb_targetDensity", m_targetDensity);
			m_pressureForceDensityProperties[0]->SetValue("Values", "cb_targetDensity", m_targetDensity);
			m_pressureForceDensityProperties[1]->SetValue("Values", "cb_targetDensity", m_targetDensity);
			m_viscosityForceDensityProperties[0]->SetValue("Values", "cb_targetDensity", m_targetDensity);
			m_viscosityForceDensityProperties[1]->SetValue("Values", "cb_targetDensity", m_targetDensity);
		}
	}
	void SphFluid2dGpu::SetPressureMultiplier(float pressureMultiplier)
	{
		if (m_pressureMultiplier != pressureMultiplier)
		{
			m_pressureMultiplier = pressureMultiplier;
			m_pressureForceDensityProperties[0]->SetValue("Values", "cb_pressureMultiplier", m_pressureMultiplier);
			m_pressureForceDensityProperties[1]->SetValue("Values", "cb_pressureMultiplier", m_pressureMultiplier);
			m_viscosityForceDensityProperties[0]->SetValue("Values", "cb_pressureMultiplier", m_pressureMultiplier);
			m_viscosityForceDensityProperties[1]->SetValue("Values", "cb_pressureMultiplier", m_pressureMultiplier);
		}
	}
	void SphFluid2dGpu::SetGravity(float gravity)
	{
		if (m_gravity != gravity)
		{
			m_gravity = gravity;
			m_pGravityForceDensityProperties->SetValue("Values", "cb_gravity", m_gravity);
		}
	}
	void SphFluid2dGpu::SetMaxVelocity(float maxVelocity)
	{
		maxVelocity = math::Max(1e-4f, maxVelocity);
		if (m_maxVelocity != maxVelocity)
		{
			m_maxVelocity = maxVelocity;
			m_pRungeKutta2Step1Properties->SetValue("Values", "cb_maxVelocity", m_maxVelocity);
			m_pRungeKutta2Step2Properties->SetValue("Values", "cb_maxVelocity", m_maxVelocity);
			m_pShaderProperties->SetValue("Values", "cb_maxVelocity", m_maxVelocity);
		}
	}
	void SphFluid2dGpu::SetAttractorRadius(float attractorRadius)
	{
		attractorRadius = math::Max(1e-4f, attractorRadius);
		if (m_attractorRadius != attractorRadius)
		{
			std::unique_ptr<Mesh> pNewRingMesh = std::unique_ptr<Mesh>(MeshGenerator::ArcFlatUv(attractorRadius - 0.1f, attractorRadius + 0.1f, 360.0f, 100, "attractorRing"));
			std::swap(m_pRingMesh, pNewRingMesh);
			m_attractorRadius = attractorRadius;
			m_externalForceDensityProperties[0]->SetValue("Values", "cb_attractorRadius", m_attractorRadius);
			m_externalForceDensityProperties[1]->SetValue("Values", "cb_attractorRadius", m_attractorRadius);
		}
	}
	void SphFluid2dGpu::SetAttractorStrength(float attractorStrength)
	{
		attractorStrength = math::Max(1e-4f, attractorStrength);
		if (m_attractorStrength != attractorStrength)
		{
			m_attractorStrength = attractorStrength;
			m_externalForceDensityProperties[0]->SetValue("Values", "cb_attractorStrength", m_attractorStrength);
			m_externalForceDensityProperties[1]->SetValue("Values", "cb_attractorStrength", m_attractorStrength);
		}
	}
	void SphFluid2dGpu::SetAttractorState(int attractorState)
	{
		if (m_attractorState != attractorState)
		{
			m_attractorState = attractorState;
			m_externalForceDensityProperties[0]->SetValue("Values", "cb_attractorState", m_attractorState);
			m_externalForceDensityProperties[1]->SetValue("Values", "cb_attractorState", m_attractorState);
		}
	}
	void SphFluid2dGpu::SetAttractorPoint(const Float2& attractorPoint)
	{
		if (m_attractorPoint != attractorPoint)
		{
			m_attractorPoint = attractorPoint;
			m_externalForceDensityProperties[0]->SetValue("Values", "cb_attractorPoint", m_attractorPoint);
			m_externalForceDensityProperties[1]->SetValue("Values", "cb_attractorPoint", m_attractorPoint);
		}
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
	void SphFluid2dGpu::SetColorMode(int colorMode)
	{
		colorMode = math::Clamp(colorMode, 0, 1);
		if (m_colorMode != colorMode)
		{
			m_colorMode = colorMode;
			m_pShaderProperties->SetValue("Values", "cb_colorMode", m_colorMode);
		}
	}
	void SphFluid2dGpu::SetInitialDistributionRadius(float initialDistributionRadius)
	{
		initialDistributionRadius = math::Max(1e-4f, initialDistributionRadius);
		if (m_initialDistributionRadius != initialDistributionRadius)
		{
			m_initialDistributionRadius = initialDistributionRadius;
			m_pResetProperties->SetValue("Values", "cb_initialDistributionRadius", m_initialDistributionRadius);
			m_reset = true;
		}
	}
	void SphFluid2dGpu::SetVisualRadius(float visualRadius)
	{
		visualRadius = math::Max(0.01f, visualRadius);
		if (m_visualRadius != visualRadius)
		{
			m_visualRadius = visualRadius;
			std::unique_ptr<Mesh> pNewParticleMesh = std::unique_ptr<Mesh>(MeshGenerator::UnitQuad()->Scale(m_visualRadius));
			std::swap(m_pParticleMesh, pNewParticleMesh);
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
	int SphFluid2dGpu::GetColorMode() const
	{
		return m_colorMode;
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
			m_reset = true;
			LOG_TRACE("Simulation stopped and reset.");
		}

		// Reset:
		if (m_reset)
		{
			m_reset = false;
			Reset();
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
				SetAttractorPoint(Float2(hit.value()));
				ShaderProperties* shaderProperties = Graphics::DrawMesh(m_pRingMesh.get(), MaterialManager::GetMaterial("simpleUnlitMaterial"), hit.value(), Float3x3::identity, 1.0f, false, false);
				shaderProperties->SetValue("SurfaceProperties", "diffuseColor", Float4::red);
				if (EventSystem::MouseHeld(EventSystem::MouseButton::left))
					SetAttractorState(1);
				if (EventSystem::MouseHeld(EventSystem::MouseButton::right))
					SetAttractorState(-1);
			}
			else
				SetAttractorState(0);
		}
		else
			SetAttractorState(0);

		// Rendering:
		Float4x4 localToWorld = GetTransform()->GetLocalToWorldMatrix();
		Graphics::DrawBounds(localToWorld, m_fluidBounds, 0.01f, Float4::white, false, false);

		m_pShaderProperties->SetStorageBuffer("b_positions", m_pPositionBuffer.get());
		m_pShaderProperties->SetStorageBuffer("b_velocities", m_pVelocityBuffer.get());
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
		m_pResetProperties->SetStorageBuffer("b_densities", m_pDensityBuffer.get());
		m_pResetProperties->SetStorageBuffer("b_forceDensities", m_pForceDensityBuffer.get());
		m_pResetProperties->SetStorageBuffer("b_kp1", m_pKp1Buffer.get());
		m_pResetProperties->SetStorageBuffer("b_kv1", m_pKv1Buffer.get());
		m_pResetProperties->SetStorageBuffer("b_kp2", m_pKp2Buffer.get());
		m_pResetProperties->SetStorageBuffer("b_kv2", m_pKv2Buffer.get());
		m_pResetProperties->SetStorageBuffer("b_tempPositions", m_pTempPositionBuffer.get());
		m_pResetProperties->SetStorageBuffer("b_tempVelocities", m_pTempVelocityBuffer.get());
		Compute::Dispatch(cs_pReset.get(), m_pResetProperties.get(), m_threadCount);
		Compute::Barrier(AccessMask::ComputeShader::shaderWrite, AccessMask::ComputeShader::shaderRead);
	}
	void SphFluid2dGpu::DispatchDensityKernal(StorageBuffer* positionBuffer, ShaderProperties* pShaderProperties)
	{
		pShaderProperties->SetStorageBuffer("b_positions", positionBuffer);
		pShaderProperties->SetStorageBuffer("b_densities", m_pDensityBuffer.get());
		Compute::Dispatch(cs_pDensity.get(), pShaderProperties, m_threadCount);
		Compute::Barrier(AccessMask::ComputeShader::shaderWrite, AccessMask::ComputeShader::shaderRead);
	}
	void SphFluid2dGpu::DispatchNormalKernal()
	{

	}
	void SphFluid2dGpu::DispatchCurvatureKernal()
	{

	}
	void SphFluid2dGpu::DispatchPressureForceDensityKernal(StorageBuffer* positionBuffer, ShaderProperties* pShaderProperties)
	{
		pShaderProperties->SetStorageBuffer("b_positions", positionBuffer);
		pShaderProperties->SetStorageBuffer("b_densities", m_pDensityBuffer.get());
		pShaderProperties->SetStorageBuffer("b_forceDensities", m_pForceDensityBuffer.get());
		Compute::Dispatch(cs_pPressureForceDensity.get(), pShaderProperties, m_threadCount);
		Compute::Barrier(AccessMask::ComputeShader::shaderWrite, AccessMask::ComputeShader::shaderRead);
	}
	void SphFluid2dGpu::DispatchViscosityForceDensityKernal(StorageBuffer* positionBuffer, StorageBuffer* velocityBuffer, ShaderProperties* pShaderProperties)
	{
		pShaderProperties->SetStorageBuffer("b_positions", positionBuffer);
		pShaderProperties->SetStorageBuffer("b_velocities", velocityBuffer);
		pShaderProperties->SetStorageBuffer("b_densities", m_pDensityBuffer.get());
		pShaderProperties->SetStorageBuffer("b_forceDensities", m_pForceDensityBuffer.get());
		Compute::Dispatch(cs_pViscosityForceDensity.get(), pShaderProperties, m_threadCount);
		Compute::Barrier(AccessMask::ComputeShader::shaderWrite, AccessMask::ComputeShader::shaderRead);
	}
	void SphFluid2dGpu::DispatchSurfaceTensionForceDensityKernal()
	{

	}
	void SphFluid2dGpu::DispatchExternalForceDensityKernal(StorageBuffer* positionBuffer, ShaderProperties* pShaderProperties)
	{
		pShaderProperties->SetStorageBuffer("b_positions", positionBuffer);
		pShaderProperties->SetStorageBuffer("b_densities", m_pDensityBuffer.get());
		pShaderProperties->SetStorageBuffer("b_forceDensities", m_pForceDensityBuffer.get());
		Compute::Dispatch(cs_pExternalForceDensity.get(), pShaderProperties, m_threadCount);
		Compute::Barrier(AccessMask::ComputeShader::shaderWrite, AccessMask::ComputeShader::shaderRead);
	}
	void SphFluid2dGpu::DispatchGravityForceDensityKernal()
	{
		m_pGravityForceDensityProperties->SetStorageBuffer("b_densities", m_pDensityBuffer.get());
		m_pGravityForceDensityProperties->SetStorageBuffer("b_forceDensities", m_pForceDensityBuffer.get());
		Compute::Dispatch(cs_pGravityForceDensity.get(), m_pGravityForceDensityProperties.get(), m_threadCount);
		Compute::Barrier(AccessMask::ComputeShader::shaderWrite, AccessMask::ComputeShader::shaderRead);
	}
	void SphFluid2dGpu::DispatchRungeKutta2Step1Kernal()
	{
		m_pRungeKutta2Step1Properties->SetStorageBuffer("b_forceDensities", m_pForceDensityBuffer.get());
		m_pRungeKutta2Step1Properties->SetStorageBuffer("b_densities", m_pDensityBuffer.get());
		m_pRungeKutta2Step1Properties->SetStorageBuffer("b_positions", m_pPositionBuffer.get());
		m_pRungeKutta2Step1Properties->SetStorageBuffer("b_velocities", m_pVelocityBuffer.get());
		m_pRungeKutta2Step1Properties->SetStorageBuffer("b_kp1", m_pKp1Buffer.get());
		m_pRungeKutta2Step1Properties->SetStorageBuffer("b_kv1", m_pKv1Buffer.get());
		m_pRungeKutta2Step1Properties->SetStorageBuffer("b_tempPositions", m_pTempPositionBuffer.get());
		m_pRungeKutta2Step1Properties->SetStorageBuffer("b_tempVelocities", m_pTempVelocityBuffer.get());
		Compute::Dispatch(cs_pRungeKutta2Step1.get(), m_pRungeKutta2Step1Properties.get(), m_threadCount);
		Compute::Barrier(AccessMask::ComputeShader::shaderWrite, AccessMask::ComputeShader::shaderRead);
	}
	void SphFluid2dGpu::DispatchRungeKutta2Step2Kernal()
	{
		m_pRungeKutta2Step2Properties->SetStorageBuffer("b_forceDensities", m_pForceDensityBuffer.get());
		m_pRungeKutta2Step2Properties->SetStorageBuffer("b_densities", m_pDensityBuffer.get());
		m_pRungeKutta2Step2Properties->SetStorageBuffer("b_kp1", m_pKp1Buffer.get());
		m_pRungeKutta2Step2Properties->SetStorageBuffer("b_kv1", m_pKv1Buffer.get());
		m_pRungeKutta2Step2Properties->SetStorageBuffer("b_tempPositions", m_pTempPositionBuffer.get());
		m_pRungeKutta2Step2Properties->SetStorageBuffer("b_tempVelocities", m_pTempVelocityBuffer.get());
		m_pRungeKutta2Step2Properties->SetStorageBuffer("b_kp2", m_pKp2Buffer.get());
		m_pRungeKutta2Step2Properties->SetStorageBuffer("b_kv2", m_pKv2Buffer.get());
		m_pRungeKutta2Step2Properties->SetStorageBuffer("b_positions", m_pPositionBuffer.get());
		m_pRungeKutta2Step2Properties->SetStorageBuffer("b_velocities", m_pVelocityBuffer.get());
		Compute::Dispatch(cs_pRungeKutta2Step2.get(), m_pRungeKutta2Step2Properties.get(), m_threadCount);
		Compute::Barrier(AccessMask::ComputeShader::shaderWrite, AccessMask::ComputeShader::shaderRead);
	}
	void SphFluid2dGpu::DispatchBoundaryCollisionsKernal()
	{
		m_pBoundaryCollisionsProperties->SetStorageBuffer("b_positions", m_pPositionBuffer.get());
		m_pBoundaryCollisionsProperties->SetStorageBuffer("b_velocities", m_pVelocityBuffer.get());
		Compute::Dispatch(cs_pBoundaryCollisions.get(), m_pBoundaryCollisionsProperties.get(), m_threadCount);
		Compute::Barrier(AccessMask::ComputeShader::shaderWrite, AccessMask::ComputeShader::shaderRead);
	}
}