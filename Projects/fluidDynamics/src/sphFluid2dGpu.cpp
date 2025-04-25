#include "sphFluid2dGpu.h"
#include "sphFluid2dGpuEditorWindow.h"
#include "accessMasks.h"
#include "sph2dBitonicSort.h"

#include "hashGrid2d.h"
#include "stagingBuffer.h"
#include "vulkanContext.h"
#include "smoothingKernals.h"



namespace emberEngine
{
	// Constructor/Destructor:
	SphFluid2dGpu::SphFluid2dGpu()
	{
		// Load compute shaders:
		std::string directoryPath = (std::string)PROJECT_ROOT_PATH + "/src/shaders/bin";
		cs_pReset = std::make_unique<ComputeShader>("reset", directoryPath + "/reset.comp.spv");
		cs_pDensity = std::make_unique<ComputeShader>("density", directoryPath + "/density.comp.spv");
		cs_pForceDensity = std::make_unique<ComputeShader>("forceDensity", directoryPath + "/forceDensity.comp.spv");
		cs_pRungeKutta2Step1 = std::make_unique<ComputeShader>("rungeKutta2Step1", directoryPath + "/rungeKutta2Step1.comp.spv");
		cs_pRungeKutta2Step2 = std::make_unique<ComputeShader>("rungeKutta2Step2", directoryPath + "/rungeKutta2Step2.comp.spv");
		cs_pBoundaryCollisions = std::make_unique<ComputeShader>("boundaryCollisions", directoryPath + "/boundaryCollisions.comp.spv");
		
		// Initialize shader properties:
		m_pResetProperties = std::make_unique<ShaderProperties>((Shader*)cs_pReset.get());
		for (int i = 0; i < 2; i++)
		{
			m_densityProperties[i] = std::make_unique<ShaderProperties>((Shader*)cs_pDensity.get());
			m_forceDensityProperties[i] = std::make_unique<ShaderProperties>((Shader*)cs_pForceDensity.get());
		}
		m_pRungeKutta2Step1Properties = std::make_unique<ShaderProperties>((Shader*)cs_pRungeKutta2Step1.get());
		m_pRungeKutta2Step2Properties = std::make_unique<ShaderProperties>((Shader*)cs_pRungeKutta2Step2.get());
		m_pBoundaryCollisionsProperties = std::make_unique<ShaderProperties>((Shader*)cs_pBoundaryCollisions.get());

		// Material setup:
		m_pParticleMaterial = MaterialManager::GetMaterial("particleMaterial");
		m_pShaderProperties = std::make_unique<ShaderProperties>((Shader*)m_pParticleMaterial);

		//// Settings0:
		//{
		//	// Management:
		//	m_isRunning = false;
		//	m_reset = true;
		//	m_timeScale = 4.0f;
		//	SetUseGridOptimization(true);
		//	m_timeStep = 0;
		//	pGpuSort = std::make_unique<Sph2dBitonicSort>();
		//	SetParticleCount(2000);
		//
		//	// Physics:
		//	SetEffectRadius(0.5f);
		//	SetMass(1.0f);
		//	SetViscosity(0.5f);
		//	m_surfaceTension = 0.07f;
		//	SetCollisionDampening(0.95f);
		//	SetTargetDensity(15.0f);
		//	SetPressureMultiplier(10.0f);
		//	SetGravity(0.5f);
		//	SetMaxVelocity(5.0f);
		//
		//	// User Interaction/Boundaries:
		//	SetAttractorRadius(3.0f);
		//	SetAttractorStrength(2.0f);
		//	SetFluidBounds(Bounds(Float3::zero, Float3(16.0f, 9.0f, 0.01f)));
		//
		//	// Visuals:
		//	SetColorMode(0);
		//	SetInitialDistributionRadius(6.0f);
		//	SetVisualRadius(0.25f);
		//}
		// Settings1:
		{
			// Management:
			m_isRunning = false;
			m_reset = true;
			m_timeScale = 4.0f;
			SetUseGridOptimization(true);
			m_timeStep = 0;
			pGpuSort = std::make_unique<Sph2dBitonicSort>();
			SetParticleCount(5000);

			// Physics:
			SetEffectRadius(0.3f);
			SetMass(1.0f);
			SetViscosity(0.5f);
			m_surfaceTension = 0.07f;
			SetCollisionDampening(0.95f);
			SetTargetDensity(70.0f);
			SetPressureMultiplier(4.0f);
			SetGravity(0.5f);
			SetMaxVelocity(5.0f);

			// User Interaction/Boundaries:
			SetAttractorRadius(3.0f);
			SetAttractorStrength(2.0f);
			SetFluidBounds(Bounds(Float3::zero, Float3(16.0f, 9.0f, 0.01f)));

			// Visuals:
			SetColorMode(1);
			SetInitialDistributionRadius(6.0f);
			SetVisualRadius(0.15f);
		}

		// Editor window:
		editorWindow = std::make_unique<SphFluid2dGpuEditorWindow>(this);
		Reset();
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
			std::unique_ptr<StorageBuffer> pNewCellKeyBuffer = std::make_unique<StorageBuffer>((uint32_t)m_particleCount, (uint32_t)sizeof(int));
			std::swap(m_pCellKeyBuffer, pNewCellKeyBuffer);
			std::unique_ptr<StorageBuffer> pNewStartIndexBuffer = std::make_unique<StorageBuffer>((uint32_t)m_particleCount, (uint32_t)sizeof(int));
			std::swap(m_pStartIndexBuffer, pNewStartIndexBuffer);
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
		ResetFluid();
		pGpuSort->ComputeCellKeys(m_pCellKeyBuffer.get(), m_pPositionBuffer.get(), m_effectRadius);
		pGpuSort->Sort(m_pCellKeyBuffer.get(), m_pPositionBuffer.get(), m_pVelocityBuffer.get());
		pGpuSort->ComputeStartIndices(m_pCellKeyBuffer.get(), m_pStartIndexBuffer.get());
		ComputeDensity(m_pPositionBuffer.get(), m_densityProperties[0].get(), m_effectRadius);
	}
	void SphFluid2dGpu::FixedUpdate()
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
	void SphFluid2dGpu::TimeStepLeapFrog(float deltaT)
	{

	}
	void SphFluid2dGpu::TimeStepVelocityVerlet(float deltaT)
	{

	}
	void SphFluid2dGpu::TimeStepRungeKutta2(float deltaT)
	{
		static int frame = 0;
		m_pRungeKutta2Step1Properties->SetValue("Values", "deltaT", deltaT);
		m_pRungeKutta2Step2Properties->SetValue("Values", "deltaT", deltaT);

		// Update hash grid for fast nearest neighbor particle look up:
		float q = 3.0f / 4.0f;	// Runge-Kutta-2 first timestep fraction.
		float gridRadius = 2.0f * m_effectRadius + q * m_maxVelocity * deltaT;
		pGpuSort->ComputeCellKeys(m_pCellKeyBuffer.get(), m_pPositionBuffer.get(), gridRadius);
		pGpuSort->Sort(m_pCellKeyBuffer.get(), m_pPositionBuffer.get(), m_pVelocityBuffer.get());
		pGpuSort->ComputeStartIndices(m_pCellKeyBuffer.get(), m_pStartIndexBuffer.get());

		// First Runte-Kutta step:
		ComputeDensity(m_pPositionBuffer.get(), m_densityProperties[0].get(), gridRadius);
		ComputeForceDensity(m_pPositionBuffer.get(), m_pVelocityBuffer.get(), m_forceDensityProperties[0].get(), gridRadius);
		ComputeRungeKutta2Step1();

		// Second Runge-Kutta step:
		ComputeDensity(m_pTempPositionBuffer.get(), m_densityProperties[1].get(), gridRadius);
		ComputeForceDensity(m_pTempPositionBuffer.get(), m_pTempVelocityBuffer.get(), m_forceDensityProperties[1].get(), gridRadius);
		ComputeRungeKutta2Step2();

		// Resolve boundary collisions:
		ComputeBoundaryCollisions();
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
		if (m_useGridOptimization != useGridOptimization)
		{
			m_useGridOptimization = useGridOptimization;
			m_densityProperties[0]->SetValue("Values", "useGridOptimization", (int)m_useGridOptimization);
			m_densityProperties[1]->SetValue("Values", "useGridOptimization", (int)m_useGridOptimization);
			m_forceDensityProperties[0]->SetValue("Values", "useGridOptimization", (int)m_useGridOptimization);
			m_forceDensityProperties[1]->SetValue("Values", "useGridOptimization", (int)m_useGridOptimization);
		}
	}
	void SphFluid2dGpu::SetParticleCount(int particleCount)
	{
		particleCount = math::Max(1, particleCount);
		if (m_particleCount != particleCount)
		{
			m_particleCount = particleCount;
			m_threadCount = Uint3(m_particleCount, 1, 1);
			m_densityProperties[0]->SetValue("Values", "particleCount", m_particleCount);
			m_densityProperties[1]->SetValue("Values", "particleCount", m_particleCount);
			m_forceDensityProperties[0]->SetValue("Values", "particleCount", m_particleCount);
			m_forceDensityProperties[1]->SetValue("Values", "particleCount", m_particleCount);
			m_reset = true;
		}
	}
	void SphFluid2dGpu::SetEffectRadius(float effectRadius)
	{
		effectRadius = math::Max(1e-4f, effectRadius);
		if (m_effectRadius != effectRadius)
		{
			m_effectRadius = effectRadius;
			m_densityProperties[0]->SetValue("Values", "effectRadius", m_effectRadius);
			m_densityProperties[1]->SetValue("Values", "effectRadius", m_effectRadius);
			m_forceDensityProperties[0]->SetValue("Values", "effectRadius", m_effectRadius);
			m_forceDensityProperties[1]->SetValue("Values", "effectRadius", m_effectRadius);
		}
	}
	void SphFluid2dGpu::SetMass(float mass)
	{
		mass = math::Max(1e-4f, mass);
		if (m_mass != mass)
		{
			m_mass = mass;
			m_densityProperties[0]->SetValue("Values", "mass", m_mass);
			m_densityProperties[1]->SetValue("Values", "mass", m_mass);
			m_forceDensityProperties[0]->SetValue("Values", "mass", m_mass);
			m_forceDensityProperties[1]->SetValue("Values", "mass", m_mass);
		}
	}
	void SphFluid2dGpu::SetViscosity(float viscosity)
	{
		if (m_viscosity != viscosity)
		{
			m_viscosity = viscosity;
			m_forceDensityProperties[0]->SetValue("Values", "viscosity", m_viscosity);
			m_forceDensityProperties[1]->SetValue("Values", "viscosity", m_viscosity);
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
			m_pBoundaryCollisionsProperties->SetValue("Values", "collisionDampening", m_collisionDampening);
		}
	}
	void SphFluid2dGpu::SetTargetDensity(float targetDensity)
	{
		targetDensity = math::Max(1e-4f, targetDensity);
		if (m_targetDensity != targetDensity)
		{
			m_targetDensity = targetDensity;
			m_pShaderProperties->SetValue("Values", "targetDensity", m_targetDensity);
			m_forceDensityProperties[0]->SetValue("Values", "targetDensity", m_targetDensity);
			m_forceDensityProperties[1]->SetValue("Values", "targetDensity", m_targetDensity);
		}
	}
	void SphFluid2dGpu::SetPressureMultiplier(float pressureMultiplier)
	{
		if (m_pressureMultiplier != pressureMultiplier)
		{
			m_pressureMultiplier = pressureMultiplier;
			m_forceDensityProperties[0]->SetValue("Values", "pressureMultiplier", m_pressureMultiplier);
			m_forceDensityProperties[1]->SetValue("Values", "pressureMultiplier", m_pressureMultiplier);
		}
	}
	void SphFluid2dGpu::SetGravity(float gravity)
	{
		if (m_gravity != gravity)
		{
			m_gravity = gravity;
			m_forceDensityProperties[0]->SetValue("Values", "gravity", m_gravity);
			m_forceDensityProperties[1]->SetValue("Values", "gravity", m_gravity);
		}
	}
	void SphFluid2dGpu::SetMaxVelocity(float maxVelocity)
	{
		maxVelocity = math::Max(1e-4f, maxVelocity);
		if (m_maxVelocity != maxVelocity)
		{
			m_maxVelocity = maxVelocity;
			m_pRungeKutta2Step1Properties->SetValue("Values", "maxVelocity", m_maxVelocity);
			m_pRungeKutta2Step2Properties->SetValue("Values", "maxVelocity", m_maxVelocity);
			m_pShaderProperties->SetValue("Values", "maxVelocity", m_maxVelocity);
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
			m_forceDensityProperties[0]->SetValue("Values", "attractorRadius", m_attractorRadius);
			m_forceDensityProperties[1]->SetValue("Values", "attractorRadius", m_attractorRadius);
		}
	}
	void SphFluid2dGpu::SetAttractorStrength(float attractorStrength)
	{
		attractorStrength = math::Max(1e-4f, attractorStrength);
		if (m_attractorStrength != attractorStrength)
		{
			m_attractorStrength = attractorStrength;
			m_forceDensityProperties[0]->SetValue("Values", "attractorStrength", m_attractorStrength);
			m_forceDensityProperties[1]->SetValue("Values", "attractorStrength", m_attractorStrength);
		}
	}
	void SphFluid2dGpu::SetAttractorState(int attractorState)
	{
		if (m_attractorState != attractorState)
		{
			m_attractorState = attractorState;
			m_forceDensityProperties[0]->SetValue("Values", "attractorState", m_attractorState);
			m_forceDensityProperties[1]->SetValue("Values", "attractorState", m_attractorState);
		}
	}
	void SphFluid2dGpu::SetAttractorPoint(const Float2& attractorPoint)
	{
		if (m_attractorPoint != attractorPoint)
		{
			m_attractorPoint = attractorPoint;
			m_forceDensityProperties[0]->SetValue("Values", "attractorPoint", m_attractorPoint);
			m_forceDensityProperties[1]->SetValue("Values", "attractorPoint", m_attractorPoint);
		}
	}
	void SphFluid2dGpu::SetFluidBounds(const Bounds& bounds)
	{
		if (m_fluidBounds != bounds)
		{
			m_fluidBounds = bounds;
			m_pBoundaryCollisionsProperties->SetValue("Values", "min", m_fluidBounds.GetMin());
			m_pBoundaryCollisionsProperties->SetValue("Values", "max", m_fluidBounds.GetMax());
		}
	}
	void SphFluid2dGpu::SetColorMode(int colorMode)
	{
		colorMode = math::Clamp(colorMode, 0, 1);
		if (m_colorMode != colorMode)
		{
			m_colorMode = colorMode;
			m_pShaderProperties->SetValue("Values", "colorMode", m_colorMode);
		}
	}
	void SphFluid2dGpu::SetInitialDistributionRadius(float initialDistributionRadius)
	{
		initialDistributionRadius = math::Max(1e-4f, initialDistributionRadius);
		if (m_initialDistributionRadius != initialDistributionRadius)
		{
			m_initialDistributionRadius = initialDistributionRadius;
			m_pResetProperties->SetValue("Values", "initialDistributionRadius", m_initialDistributionRadius);
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

		m_pShaderProperties->SetStorageBuffer("positionBuffer", m_pPositionBuffer.get());
		m_pShaderProperties->SetStorageBuffer("velocityBuffer", m_pVelocityBuffer.get());
		m_pShaderProperties->SetStorageBuffer("densityBuffer", m_pDensityBuffer.get());
		Graphics::DrawInstanced(m_particleCount, m_pPositionBuffer.get(), m_pParticleMesh.get(), m_pParticleMaterial, m_pShaderProperties.get(), localToWorld, false, false);
	}
	const std::string SphFluid2dGpu::ToString() const
	{
		return "SphFluid2dGpu";
	}



	// Private methods:
	// Physics:
	void SphFluid2dGpu::ResetFluid()
	{
		m_pResetProperties->SetStorageBuffer("positionBuffer", m_pPositionBuffer.get());
		m_pResetProperties->SetStorageBuffer("velocityBuffer", m_pVelocityBuffer.get());
		m_pResetProperties->SetStorageBuffer("densityBuffer", m_pDensityBuffer.get());
		m_pResetProperties->SetStorageBuffer("forceDensityBuffer", m_pForceDensityBuffer.get());
		m_pResetProperties->SetStorageBuffer("kp1Buffer", m_pKp1Buffer.get());
		m_pResetProperties->SetStorageBuffer("kv1Buffer", m_pKv1Buffer.get());
		m_pResetProperties->SetStorageBuffer("kp2Buffer", m_pKp2Buffer.get());
		m_pResetProperties->SetStorageBuffer("kv2Buffer", m_pKv2Buffer.get());
		m_pResetProperties->SetStorageBuffer("tempPositionBuffer", m_pTempPositionBuffer.get());
		m_pResetProperties->SetStorageBuffer("tempVelocityBuffer", m_pTempVelocityBuffer.get());
		compute::PreRender::RecordComputeShader(cs_pReset.get(), m_pResetProperties.get(), m_threadCount);
		compute::PreRender::RecordBarrier(AccessMask::ComputeShader::shaderWrite, AccessMask::ComputeShader::shaderRead);
	}
	void SphFluid2dGpu::ComputeDensity(StorageBuffer* positionBuffer, ShaderProperties* pShaderProperties, float gridRadius)
	{
		pShaderProperties->SetStorageBuffer("cellKeyBuffer", m_pCellKeyBuffer.get());
		pShaderProperties->SetStorageBuffer("startIndexBuffer", m_pStartIndexBuffer.get());
		pShaderProperties->SetStorageBuffer("positionBuffer", positionBuffer);
		pShaderProperties->SetStorageBuffer("densityBuffer", m_pDensityBuffer.get());
		pShaderProperties->SetValue("Values", "gridRadius", gridRadius);
		compute::PreRender::RecordComputeShader(cs_pDensity.get(), pShaderProperties, m_threadCount);
		compute::PreRender::RecordBarrier(AccessMask::ComputeShader::shaderWrite, AccessMask::ComputeShader::shaderRead);
	}
	void SphFluid2dGpu::ComputeNormals()
	{

	}
	void SphFluid2dGpu::ComputeCurvature()
	{

	}
	void SphFluid2dGpu::ComputeForceDensity(StorageBuffer* positionBuffer, StorageBuffer* velocityBuffer, ShaderProperties* pShaderProperties, float gridRadius)
	{
		pShaderProperties->SetStorageBuffer("cellKeyBuffer", m_pCellKeyBuffer.get());
		pShaderProperties->SetStorageBuffer("startIndexBuffer", m_pStartIndexBuffer.get());
		pShaderProperties->SetStorageBuffer("positionBuffer", positionBuffer);
		pShaderProperties->SetStorageBuffer("velocityBuffer", velocityBuffer);
		pShaderProperties->SetStorageBuffer("densityBuffer", m_pDensityBuffer.get());
		pShaderProperties->SetStorageBuffer("forceDensityBuffer", m_pForceDensityBuffer.get());
		pShaderProperties->SetValue("Values", "gridRadius", gridRadius);
		compute::PreRender::RecordComputeShader(cs_pForceDensity.get(), pShaderProperties, m_threadCount);
		compute::PreRender::RecordBarrier(AccessMask::ComputeShader::shaderWrite, AccessMask::ComputeShader::shaderRead);
	}
	void SphFluid2dGpu::ComputeRungeKutta2Step1()
	{
		m_pRungeKutta2Step1Properties->SetStorageBuffer("forceDensityBuffer", m_pForceDensityBuffer.get());
		m_pRungeKutta2Step1Properties->SetStorageBuffer("densityBuffer", m_pDensityBuffer.get());
		m_pRungeKutta2Step1Properties->SetStorageBuffer("positionBuffer", m_pPositionBuffer.get());
		m_pRungeKutta2Step1Properties->SetStorageBuffer("velocityBuffer", m_pVelocityBuffer.get());
		m_pRungeKutta2Step1Properties->SetStorageBuffer("kp1Buffer", m_pKp1Buffer.get());
		m_pRungeKutta2Step1Properties->SetStorageBuffer("kv1Buffer", m_pKv1Buffer.get());
		m_pRungeKutta2Step1Properties->SetStorageBuffer("tempPositionBuffer", m_pTempPositionBuffer.get());
		m_pRungeKutta2Step1Properties->SetStorageBuffer("tempVelocityBuffer", m_pTempVelocityBuffer.get());
		compute::PreRender::RecordComputeShader(cs_pRungeKutta2Step1.get(), m_pRungeKutta2Step1Properties.get(), m_threadCount);
		compute::PreRender::RecordBarrier(AccessMask::ComputeShader::shaderWrite, AccessMask::ComputeShader::shaderRead);
	}
	void SphFluid2dGpu::ComputeRungeKutta2Step2()
	{
		m_pRungeKutta2Step2Properties->SetStorageBuffer("forceDensityBuffer", m_pForceDensityBuffer.get());
		m_pRungeKutta2Step2Properties->SetStorageBuffer("densityBuffer", m_pDensityBuffer.get());
		m_pRungeKutta2Step2Properties->SetStorageBuffer("kp1Buffer", m_pKp1Buffer.get());
		m_pRungeKutta2Step2Properties->SetStorageBuffer("kv1Buffer", m_pKv1Buffer.get());
		m_pRungeKutta2Step2Properties->SetStorageBuffer("tempPositionBuffer", m_pTempPositionBuffer.get());
		m_pRungeKutta2Step2Properties->SetStorageBuffer("tempVelocityBuffer", m_pTempVelocityBuffer.get());
		m_pRungeKutta2Step2Properties->SetStorageBuffer("kp2Buffer", m_pKp2Buffer.get());
		m_pRungeKutta2Step2Properties->SetStorageBuffer("kv2Buffer", m_pKv2Buffer.get());
		m_pRungeKutta2Step2Properties->SetStorageBuffer("positionBuffer", m_pPositionBuffer.get());
		m_pRungeKutta2Step2Properties->SetStorageBuffer("velocityBuffer", m_pVelocityBuffer.get());
		compute::PreRender::RecordComputeShader(cs_pRungeKutta2Step2.get(), m_pRungeKutta2Step2Properties.get(), m_threadCount);
		compute::PreRender::RecordBarrier(AccessMask::ComputeShader::shaderWrite, AccessMask::ComputeShader::shaderRead);
	}
	void SphFluid2dGpu::ComputeBoundaryCollisions()
	{
		m_pBoundaryCollisionsProperties->SetStorageBuffer("positionBuffer", m_pPositionBuffer.get());
		m_pBoundaryCollisionsProperties->SetStorageBuffer("velocityBuffer", m_pVelocityBuffer.get());
		compute::PreRender::RecordComputeShader(cs_pBoundaryCollisions.get(), m_pBoundaryCollisionsProperties.get(), m_threadCount);
		compute::PreRender::RecordBarrier(AccessMask::ComputeShader::shaderWrite, AccessMask::ComputeShader::shaderRead);
	}
}