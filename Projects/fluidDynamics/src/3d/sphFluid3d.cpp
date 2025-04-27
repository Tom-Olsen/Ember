#include "sphFluid3d.h"
#include "sphFluid3dEditorWindow.h"
#include "accessMasks.h"
#include "sphBitonicSort3d.h"

#include "hashGrid2d.h"
#include "stagingBuffer.h"
#include "vulkanContext.h"
#include "smoothingKernals.h"



namespace emberEngine
{
	// Constructor/Destructor:
	SphFluid3d::SphFluid3d()
	{
		// Load compute shaders:
		std::string directoryPath = (std::string)PROJECT_ROOT_PATH + "/src/shaders/bin";
		cs_pReset = std::make_unique<ComputeShader>("reset3d", directoryPath + "/reset3d.comp.spv");
		cs_pDensity = std::make_unique<ComputeShader>("density3d", directoryPath + "/density3d.comp.spv");
		cs_pNormalAndCurvature = std::make_unique<ComputeShader>("normalAndCurvature3d", directoryPath + "/normalAndCurvature3d.comp.spv");
		cs_pForceDensity = std::make_unique<ComputeShader>("forceDensity3d", directoryPath + "/forceDensity3d.comp.spv");
		cs_pRungeKutta2Step1 = std::make_unique<ComputeShader>("rungeKutta2Step1_3d", directoryPath + "/rungeKutta2Step1_3d.comp.spv");
		cs_pRungeKutta2Step2 = std::make_unique<ComputeShader>("rungeKutta2Step2_3d", directoryPath + "/rungeKutta2Step2_3d.comp.spv");
		cs_pBoundaryCollisions = std::make_unique<ComputeShader>("boundaryCollisions3d", directoryPath + "/boundaryCollisions3d.comp.spv");

		// Initialize shader properties:
		m_pResetProperties = std::make_unique<ShaderProperties>((Shader*)cs_pReset.get());
		for (int i = 0; i < 2; i++)
		{
			m_densityProperties[i] = std::make_unique<ShaderProperties>((Shader*)cs_pDensity.get());
			m_normalAndCurvatureProperties[i] = std::make_unique<ShaderProperties>((Shader*)cs_pNormalAndCurvature.get());
			m_forceDensityProperties[i] = std::make_unique<ShaderProperties>((Shader*)cs_pForceDensity.get());
		}
		m_pRungeKutta2Step1Properties = std::make_unique<ShaderProperties>((Shader*)cs_pRungeKutta2Step1.get());
		m_pRungeKutta2Step2Properties = std::make_unique<ShaderProperties>((Shader*)cs_pRungeKutta2Step2.get());
		m_pBoundaryCollisionsProperties = std::make_unique<ShaderProperties>((Shader*)cs_pBoundaryCollisions.get());

		// Material setup:
		m_pParticleMaterial = MaterialManager::GetMaterial("particleMaterial3d");
		m_pShaderProperties = std::make_unique<ShaderProperties>((Shader*)m_pParticleMaterial);

		// Settings:
		{
			// Management:
			m_isRunning = false;
			m_reset = true;
			m_timeScale = 4.0f;
			SetUseGridOptimization(true);
			m_timeStep = 0;
			pGpuSort = std::make_unique<SphBitonicSort3d>();
			SetParticleCount(10000);

			// Physics:
			SetEffectRadius(0.3f);
			SetMass(1.0f);
			SetViscosity(0.5f);
			SetSurfaceTension(0.07f);
			SetCollisionDampening(0.95f);
			SetTargetDensity(90.0f);
			SetPressureMultiplier(6.0f);
			SetGravity(0.5f);
			SetMaxVelocity(5.0f);

			// User Interaction/Boundaries:
			SetFluidBounds(Bounds(Float3::zero, Float3(16.0f, 9.0f, 0.01f)));

			// Visuals:
			SetColorMode(1);
			SetInitialDistributionRadius(8.0f);
			SetVisualRadius(0.15f);
		}

		// Editor window:
		editorWindow = std::make_unique<SphFluid3dEditorWindow>(this);
		Reset();
	}
	SphFluid3d::~SphFluid3d()
	{

	}

	// Physics update:
	void SphFluid3d::Reset()
	{
		m_timeStep = 0;

		// Reallocate buffers:
		if (m_pPositionBuffer == nullptr || m_pPositionBuffer->GetCount() != m_particleCount)
		{
			std::unique_ptr<StorageBuffer> pNewCellKeyBuffer = std::make_unique<StorageBuffer>((uint32_t)m_particleCount, (uint32_t)sizeof(int));
			std::swap(m_pCellKeyBuffer, pNewCellKeyBuffer);
			std::unique_ptr<StorageBuffer> pNewStartIndexBuffer = std::make_unique<StorageBuffer>((uint32_t)m_particleCount, (uint32_t)sizeof(int));
			std::swap(m_pStartIndexBuffer, pNewStartIndexBuffer);
			std::unique_ptr<StorageBuffer> pNewPositionBuffer = std::make_unique<StorageBuffer>((uint32_t)m_particleCount, (uint32_t)sizeof(Float3));
			std::swap(m_pPositionBuffer, pNewPositionBuffer);
			std::unique_ptr<StorageBuffer> pNewVelocityBuffer = std::make_unique<StorageBuffer>((uint32_t)m_particleCount, (uint32_t)sizeof(Float3));
			std::swap(m_pVelocityBuffer, pNewVelocityBuffer);
			std::unique_ptr<StorageBuffer> pNewDensityBuffer = std::make_unique<StorageBuffer>((uint32_t)m_particleCount, (uint32_t)sizeof(float));
			std::swap(m_pDensityBuffer, pNewDensityBuffer);
			std::unique_ptr<StorageBuffer> pNewNormalBuffer = std::make_unique<StorageBuffer>((uint32_t)m_particleCount, (uint32_t)sizeof(Float3));
			std::swap(m_pNormalBuffer, pNewNormalBuffer);
			std::unique_ptr<StorageBuffer> pNewCurvatureBuffer = std::make_unique<StorageBuffer>((uint32_t)m_particleCount, (uint32_t)sizeof(float));
			std::swap(m_pCurvatureBuffer, pNewCurvatureBuffer);
			std::unique_ptr<StorageBuffer> pNewForceDensityBuffer = std::make_unique<StorageBuffer>((uint32_t)m_particleCount, (uint32_t)sizeof(Float3));
			std::swap(m_pForceDensityBuffer, pNewForceDensityBuffer);
			std::unique_ptr<StorageBuffer> pNewKp1Buffer = std::make_unique<StorageBuffer>((uint32_t)m_particleCount, (uint32_t)sizeof(Float3));
			std::swap(m_pKp1Buffer, pNewKp1Buffer);
			std::unique_ptr<StorageBuffer> pNewKv1Buffer = std::make_unique<StorageBuffer>((uint32_t)m_particleCount, (uint32_t)sizeof(Float3));
			std::swap(m_pKv1Buffer, pNewKv1Buffer);
			std::unique_ptr<StorageBuffer> pNewKp2Buffer = std::make_unique<StorageBuffer>((uint32_t)m_particleCount, (uint32_t)sizeof(Float3));
			std::swap(m_pKp2Buffer, pNewKp2Buffer);
			std::unique_ptr<StorageBuffer> pNewKv2Buffer = std::make_unique<StorageBuffer>((uint32_t)m_particleCount, (uint32_t)sizeof(Float3));
			std::swap(m_pKv2Buffer, pNewKv2Buffer);
			std::unique_ptr<StorageBuffer> pNewTempPositionBuffer = std::make_unique<StorageBuffer>((uint32_t)m_particleCount, (uint32_t)sizeof(Float3));
			std::swap(m_pTempPositionBuffer, pNewTempPositionBuffer);
			std::unique_ptr<StorageBuffer> pNewTempVelocityBuffer = std::make_unique<StorageBuffer>((uint32_t)m_particleCount, (uint32_t)sizeof(Float3));
			std::swap(m_pTempVelocityBuffer, pNewTempVelocityBuffer);
		}

		// Reset fluid to initial data:
		ResetFluid();
		pGpuSort->ComputeCellKeys(m_pCellKeyBuffer.get(), m_pPositionBuffer.get(), m_effectRadius);
		pGpuSort->Sort(m_pCellKeyBuffer.get(), m_pPositionBuffer.get(), m_pVelocityBuffer.get());
		pGpuSort->ComputeStartIndices(m_pCellKeyBuffer.get(), m_pStartIndexBuffer.get());
		ComputeDensity(m_pPositionBuffer.get(), m_densityProperties[0].get(), m_effectRadius);
		ComputeNormalAndCurvature(m_pPositionBuffer.get(), m_normalAndCurvatureProperties[0].get(), m_effectRadius);
	}
	void SphFluid3d::FixedUpdate()
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
	void SphFluid3d::TimeStepLeapFrog(float deltaT)
	{

	}
	void SphFluid3d::TimeStepVelocityVerlet(float deltaT)
	{

	}
	void SphFluid3d::TimeStepRungeKutta2(float deltaT)
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
		ComputeNormalAndCurvature(m_pPositionBuffer.get(), m_normalAndCurvatureProperties[0].get(), gridRadius);
		ComputeForceDensity(m_pPositionBuffer.get(), m_pVelocityBuffer.get(), m_forceDensityProperties[0].get(), gridRadius);
		ComputeRungeKutta2Step1();

		// Second Runge-Kutta step:
		ComputeDensity(m_pTempPositionBuffer.get(), m_densityProperties[1].get(), gridRadius);
		ComputeNormalAndCurvature(m_pTempPositionBuffer.get(), m_normalAndCurvatureProperties[1].get(), gridRadius);
		ComputeForceDensity(m_pTempPositionBuffer.get(), m_pTempVelocityBuffer.get(), m_forceDensityProperties[1].get(), gridRadius);
		ComputeRungeKutta2Step2();

		// Resolve boundary collisions:
		ComputeBoundaryCollisions();
	}



	// Setters:
	void SphFluid3d::SetIsRunning(bool isRunning)
	{
		m_isRunning = isRunning;
	}
	void SphFluid3d::SetTimeScale(float timeScale)
	{
		m_timeScale = timeScale;
	}
	void SphFluid3d::SetUseGridOptimization(bool useGridOptimization)
	{
		if (m_useGridOptimization != useGridOptimization)
		{
			m_useGridOptimization = useGridOptimization;
			m_densityProperties[0]->SetValue("Values", "useGridOptimization", (int)m_useGridOptimization);
			m_densityProperties[1]->SetValue("Values", "useGridOptimization", (int)m_useGridOptimization);
			m_normalAndCurvatureProperties[0]->SetValue("Values", "useGridOptimization", (int)m_useGridOptimization);
			m_normalAndCurvatureProperties[1]->SetValue("Values", "useGridOptimization", (int)m_useGridOptimization);
			m_forceDensityProperties[0]->SetValue("Values", "useGridOptimization", (int)m_useGridOptimization);
			m_forceDensityProperties[1]->SetValue("Values", "useGridOptimization", (int)m_useGridOptimization);
		}
	}
	void SphFluid3d::SetParticleCount(int particleCount)
	{
		particleCount = math::Max(1, particleCount);
		if (m_particleCount != particleCount)
		{
			m_particleCount = particleCount;
			m_threadCount = Uint3(m_particleCount, 1, 1);
			m_densityProperties[0]->SetValue("Values", "particleCount", m_particleCount);
			m_densityProperties[1]->SetValue("Values", "particleCount", m_particleCount);
			m_normalAndCurvatureProperties[0]->SetValue("Values", "particleCount", m_particleCount);
			m_normalAndCurvatureProperties[1]->SetValue("Values", "particleCount", m_particleCount);
			m_forceDensityProperties[0]->SetValue("Values", "particleCount", m_particleCount);
			m_forceDensityProperties[1]->SetValue("Values", "particleCount", m_particleCount);
			m_reset = true;
		}
	}
	void SphFluid3d::SetEffectRadius(float effectRadius)
	{
		effectRadius = math::Max(1e-4f, effectRadius);
		if (m_effectRadius != effectRadius)
		{
			m_effectRadius = effectRadius;
			m_densityProperties[0]->SetValue("Values", "effectRadius", m_effectRadius);
			m_densityProperties[1]->SetValue("Values", "effectRadius", m_effectRadius);
			m_normalAndCurvatureProperties[0]->SetValue("Values", "effectRadius", m_effectRadius);
			m_normalAndCurvatureProperties[1]->SetValue("Values", "effectRadius", m_effectRadius);
			m_forceDensityProperties[0]->SetValue("Values", "effectRadius", m_effectRadius);
			m_forceDensityProperties[1]->SetValue("Values", "effectRadius", m_effectRadius);
		}
	}
	void SphFluid3d::SetMass(float mass)
	{
		mass = math::Max(1e-4f, mass);
		if (m_mass != mass)
		{
			m_mass = mass;
			m_densityProperties[0]->SetValue("Values", "mass", m_mass);
			m_densityProperties[1]->SetValue("Values", "mass", m_mass);
			m_normalAndCurvatureProperties[0]->SetValue("Values", "mass", m_mass);
			m_normalAndCurvatureProperties[1]->SetValue("Values", "mass", m_mass);
			m_forceDensityProperties[0]->SetValue("Values", "mass", m_mass);
			m_forceDensityProperties[1]->SetValue("Values", "mass", m_mass);
		}
	}
	void SphFluid3d::SetViscosity(float viscosity)
	{
		if (m_viscosity != viscosity)
		{
			m_viscosity = viscosity;
			m_forceDensityProperties[0]->SetValue("Values", "viscosity", m_viscosity);
			m_forceDensityProperties[1]->SetValue("Values", "viscosity", m_viscosity);
		}
	}
	void SphFluid3d::SetSurfaceTension(float surfaceTension)
	{
		if (m_surfaceTension != surfaceTension)
		{
			m_surfaceTension = surfaceTension;
			m_forceDensityProperties[0]->SetValue("Values", "surfaceTension", m_surfaceTension);
			m_forceDensityProperties[1]->SetValue("Values", "surfaceTension", m_surfaceTension);
		}
	}
	void SphFluid3d::SetCollisionDampening(float collisionDampening)
	{
		if (m_collisionDampening != collisionDampening)
		{
			m_collisionDampening = collisionDampening;
			m_pBoundaryCollisionsProperties->SetValue("Values", "collisionDampening", m_collisionDampening);
		}
	}
	void SphFluid3d::SetTargetDensity(float targetDensity)
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
	void SphFluid3d::SetPressureMultiplier(float pressureMultiplier)
	{
		if (m_pressureMultiplier != pressureMultiplier)
		{
			m_pressureMultiplier = pressureMultiplier;
			m_forceDensityProperties[0]->SetValue("Values", "pressureMultiplier", m_pressureMultiplier);
			m_forceDensityProperties[1]->SetValue("Values", "pressureMultiplier", m_pressureMultiplier);
		}
	}
	void SphFluid3d::SetGravity(float gravity)
	{
		if (m_gravity != gravity)
		{
			m_gravity = gravity;
			m_forceDensityProperties[0]->SetValue("Values", "gravity", m_gravity);
			m_forceDensityProperties[1]->SetValue("Values", "gravity", m_gravity);
		}
	}
	void SphFluid3d::SetMaxVelocity(float maxVelocity)
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
	void SphFluid3d::SetFluidBounds(const Bounds& bounds)
	{
		if (m_fluidBounds != bounds)
		{
			m_fluidBounds = bounds;
			m_pBoundaryCollisionsProperties->SetValue("Values", "min", m_fluidBounds.GetMin());
			m_pBoundaryCollisionsProperties->SetValue("Values", "max", m_fluidBounds.GetMax());
		}
	}
	void SphFluid3d::SetColorMode(int colorMode)
	{
		colorMode = math::Clamp(colorMode, 0, 3);
		if (m_colorMode != colorMode)
		{
			m_colorMode = colorMode;
			m_pShaderProperties->SetValue("Values", "colorMode", m_colorMode);
		}
	}
	void SphFluid3d::SetInitialDistributionRadius(float initialDistributionRadius)
	{
		initialDistributionRadius = math::Max(1e-4f, initialDistributionRadius);
		if (m_initialDistributionRadius != initialDistributionRadius)
		{
			m_initialDistributionRadius = initialDistributionRadius;
			m_pResetProperties->SetValue("Values", "initialDistributionRadius", m_initialDistributionRadius);
			m_reset = true;
		}
	}
	void SphFluid3d::SetVisualRadius(float visualRadius)
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
	bool SphFluid3d::GetIsRunning() const
	{
		return m_isRunning;
	}
	float SphFluid3d::GetTimeScale() const
	{
		return m_timeScale;
	}
	bool SphFluid3d::GetUseGridOptimization() const
	{
		return m_useGridOptimization;
	}
	uint32_t SphFluid3d::GetTimeStep() const
	{
		return m_timeStep;
	}
	int SphFluid3d::GetParticleCount() const
	{
		return m_particleCount;
	}
	float SphFluid3d::GetEffectRadius() const
	{
		return m_effectRadius;
	}
	float SphFluid3d::GetMass() const
	{
		return m_mass;
	}
	float SphFluid3d::GetViscosity() const
	{
		return m_viscosity;
	}
	float SphFluid3d::GetSurfaceTension() const
	{
		return m_surfaceTension;
	}
	float SphFluid3d::GetCollisionDampening() const
	{
		return m_collisionDampening;
	}
	float SphFluid3d::GetTargetDensity() const
	{
		return m_targetDensity;
	}
	float SphFluid3d::GetPressureMultiplier() const
	{
		return m_pressureMultiplier;
	}
	float SphFluid3d::GetGravity() const
	{
		return m_gravity;
	}
	float SphFluid3d::GetMaxVelocity() const
	{
		return m_maxVelocity;
	}
	Bounds SphFluid3d::GetFluidBounds() const
	{
		return m_fluidBounds;
	}
	int SphFluid3d::GetColorMode() const
	{
		return m_colorMode;
	}
	float SphFluid3d::GetInitialDistributionRadius() const
	{
		return m_initialDistributionRadius;
	}
	float SphFluid3d::GetVisualRadius() const
	{
		return m_visualRadius;
	}



	// Overrides:
	void SphFluid3d::Update()
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

		// Rendering:
		Float4x4 localToWorld = GetTransform()->GetLocalToWorldMatrix();
		Graphics::DrawBounds(localToWorld, m_fluidBounds, 0.01f, Float4::white, false, false);

		m_pShaderProperties->SetStorageBuffer("positionBuffer", m_pPositionBuffer.get());
		m_pShaderProperties->SetStorageBuffer("velocityBuffer", m_pVelocityBuffer.get());
		m_pShaderProperties->SetStorageBuffer("densityBuffer", m_pDensityBuffer.get());
		m_pShaderProperties->SetStorageBuffer("normalBuffer", m_pNormalBuffer.get());
		m_pShaderProperties->SetStorageBuffer("curvatureBuffer", m_pCurvatureBuffer.get());
		Graphics::DrawInstanced(m_particleCount, m_pPositionBuffer.get(), m_pParticleMesh.get(), m_pParticleMaterial, m_pShaderProperties.get(), localToWorld, false, false);
	}
	const std::string SphFluid3d::ToString() const
	{
		return "SphFluid3d";
	}



	// Private methods:
	// Physics:
	void SphFluid3d::ResetFluid()
	{
		m_pResetProperties->SetStorageBuffer("positionBuffer", m_pPositionBuffer.get());
		m_pResetProperties->SetStorageBuffer("velocityBuffer", m_pVelocityBuffer.get());
		m_pResetProperties->SetStorageBuffer("densityBuffer", m_pDensityBuffer.get());
		m_pResetProperties->SetStorageBuffer("normalBuffer", m_pNormalBuffer.get());
		m_pResetProperties->SetStorageBuffer("curvatureBuffer", m_pCurvatureBuffer.get());
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
	void SphFluid3d::ComputeDensity(StorageBuffer* positionBuffer, ShaderProperties* pShaderProperties, float gridRadius)
	{
		pShaderProperties->SetStorageBuffer("cellKeyBuffer", m_pCellKeyBuffer.get());
		pShaderProperties->SetStorageBuffer("startIndexBuffer", m_pStartIndexBuffer.get());
		pShaderProperties->SetStorageBuffer("positionBuffer", positionBuffer);
		pShaderProperties->SetStorageBuffer("densityBuffer", m_pDensityBuffer.get());
		pShaderProperties->SetValue("Values", "gridRadius", gridRadius);
		compute::PreRender::RecordComputeShader(cs_pDensity.get(), pShaderProperties, m_threadCount);
		compute::PreRender::RecordBarrier(AccessMask::ComputeShader::shaderWrite, AccessMask::ComputeShader::shaderRead);
	}
	void SphFluid3d::ComputeNormalAndCurvature(StorageBuffer* positionBuffer, ShaderProperties* pShaderProperties, float gridRadius)
	{
		pShaderProperties->SetStorageBuffer("cellKeyBuffer", m_pCellKeyBuffer.get());
		pShaderProperties->SetStorageBuffer("startIndexBuffer", m_pStartIndexBuffer.get());
		pShaderProperties->SetStorageBuffer("positionBuffer", positionBuffer);
		pShaderProperties->SetStorageBuffer("densityBuffer", m_pDensityBuffer.get());
		pShaderProperties->SetStorageBuffer("normalBuffer", m_pNormalBuffer.get());
		pShaderProperties->SetStorageBuffer("curvatureBuffer", m_pCurvatureBuffer.get());
		pShaderProperties->SetValue("Values", "gridRadius", gridRadius);
		compute::PreRender::RecordComputeShader(cs_pNormalAndCurvature.get(), pShaderProperties, m_threadCount);
		compute::PreRender::RecordBarrier(AccessMask::ComputeShader::shaderWrite, AccessMask::ComputeShader::shaderRead);
	}
	void SphFluid3d::ComputeCurvature()
	{

	}
	void SphFluid3d::ComputeForceDensity(StorageBuffer* positionBuffer, StorageBuffer* velocityBuffer, ShaderProperties* pShaderProperties, float gridRadius)
	{
		pShaderProperties->SetStorageBuffer("cellKeyBuffer", m_pCellKeyBuffer.get());
		pShaderProperties->SetStorageBuffer("startIndexBuffer", m_pStartIndexBuffer.get());
		pShaderProperties->SetStorageBuffer("positionBuffer", positionBuffer);
		pShaderProperties->SetStorageBuffer("velocityBuffer", velocityBuffer);
		pShaderProperties->SetStorageBuffer("densityBuffer", m_pDensityBuffer.get());
		pShaderProperties->SetStorageBuffer("normalBuffer", m_pNormalBuffer.get());
		pShaderProperties->SetStorageBuffer("curvatureBuffer", m_pCurvatureBuffer.get());
		pShaderProperties->SetStorageBuffer("forceDensityBuffer", m_pForceDensityBuffer.get());
		pShaderProperties->SetValue("Values", "gridRadius", gridRadius);
		compute::PreRender::RecordComputeShader(cs_pForceDensity.get(), pShaderProperties, m_threadCount);
		compute::PreRender::RecordBarrier(AccessMask::ComputeShader::shaderWrite, AccessMask::ComputeShader::shaderRead);
	}
	void SphFluid3d::ComputeRungeKutta2Step1()
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
	void SphFluid3d::ComputeRungeKutta2Step2()
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
	void SphFluid3d::ComputeBoundaryCollisions()
	{
		m_pBoundaryCollisionsProperties->SetStorageBuffer("positionBuffer", m_pPositionBuffer.get());
		m_pBoundaryCollisionsProperties->SetStorageBuffer("velocityBuffer", m_pVelocityBuffer.get());
		compute::PreRender::RecordComputeShader(cs_pBoundaryCollisions.get(), m_pBoundaryCollisionsProperties.get(), m_threadCount);
		compute::PreRender::RecordBarrier(AccessMask::ComputeShader::shaderWrite, AccessMask::ComputeShader::shaderRead);
	}
}