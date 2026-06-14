#include "sphFluid3dGpu.h"
#include "logger.h"
#include "sphFluid3dGpuEditorWindow.h"



namespace fluidDynamics
{
	// Public methods:
	// Constructor/Destructor:
	SphFluid3dGpu::SphFluid3dGpu()
	{
		// Material setup:
		m_particleMaterial = MaterialManager::GetMaterial("particleMaterial3d");
		m_shaderProperties = ShaderProperties(m_particleMaterial);

        m_forceSetters = true;
		{
			// Management:
			SetTimeScale(1.0f);
			SetParticleCount(100000);

			// Settings:
			SetUseHashGridOptimization(true);
			SetEffectRadius(1.0f);
			SetMass(1.0f);
			SetViscosity(2.0f);
			SetSurfaceTension(1.0f);
			SetCollisionDampening(0.95f);
			SetTargetDensity(12.5f);
			SetPressureMultiplier(300.0f);
			SetGravity(5.0f);
			SetMaxVelocity(5.0f);
			SetFluidBounds(Bounds(Float3::zero, Float3(16.0f, 9.0f, 9.0f)));

			// User Interaction:
			SetAttractorRadius(6.0f);
			SetAttractorStrength(10.0f);
			SetAttractorState(0);

			// Visuals:
			SetColorMode(0);
			SetInitialDistributionRadius(7.5f);
			SetVisualRadius(0.5f);
		}
        m_forceSetters = false;

		// Editor Window:
		editorWindow = std::make_unique<emberEditor::SphFluid3dGpuEditorWindow>(this);
		Reset();
	}
	SphFluid3dGpu::~SphFluid3dGpu()
	{

	}



	// Overrides:
	void SphFluid3dGpu::FixedUpdate()
	{
		if (m_reset)
		{
			RecordReset();
			return;
		}
		if (!m_isRunning)
			return;
		m_tripleBufferState.PublishFinishedWrites();

		// Do multiple iterations of deltaT<=dt if timeScale is bigger 1. Otherwise 1 iteration per FixedUpdate().
		float dt = Time::GetFixedDeltaTime();
		float timeStep = m_timeScale * dt;
		float restTime = timeStep;
		uint32_t sourceDataIndex = m_tripleBufferState.GetSrcIndex();
		uint32_t destinationDataIndex = m_tripleBufferState.GetDstIndex();
		while (restTime > 0.0f)
		{
			float deltaT = math::Min(dt, restTime);
			SphFluid3dGpuSolver::TimeStepRungeKutta2(deltaT, m_settings, m_computeShaders, m_scratchData, m_tripleData, sourceDataIndex, destinationDataIndex);
			m_tripleBufferState.CommitWrite(Compute::Physics::GetRecordingSessionID());
			sourceDataIndex = m_tripleBufferState.GetSrcIndex();
			destinationDataIndex = m_tripleBufferState.GetDstIndex();
			restTime -= deltaT;
		}
		m_timeStep++;
	}
	void SphFluid3dGpu::Update()
	{
		// Detect framerate crash:
		if (m_isRunning && Time::GetDeltaTime() > 0.1f)
		{
			m_isRunning = false;
			LOG_TRACE("Stopped simulation due to framerate crash.");
		}

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
			m_reset = true;
			LOG_TRACE("Simulation stopped and reset.");
		}

		// Reset:
		if (m_reset)
			return;
		m_tripleBufferState.PublishFinishedWrites();

		// Mouse scrolling:
		float mouseScroll = EventSystem::MouseScrollY();
		if (mouseScroll != 0)
		{
			if (EventSystem::KeyDownOrHeld(Input::Key::ShiftLeft))
			{
				float zoomFactor = 1.0f + 0.1f * mouseScroll;
				SetAttractorStrength(zoomFactor * m_attractor.strength);
			}
			if (EventSystem::KeyDownOrHeld(Input::Key::CtrlLeft))
			{
				float zoomFactor = 1.0f + 0.1f * mouseScroll;
				SetAttractorRadius(zoomFactor * m_attractor.radius);
			}
		}

		// Keyboard attractor activation:
		int attractorState = (int)EventSystem::KeyDownOrHeld(Input::Key::Num1) - (int)EventSystem::KeyDownOrHeld(Input::Key::Num2);
		SetAttractorState(attractorState);

		// Rendering:
		Float4x4 localToWorld = GetTransform()->GetLocalToWorldMatrix();
		Renderer::DrawBounds(localToWorld, m_settings.fluidBounds, 0.2f, Float4::white, false, false);
		if (m_attractor.state != 0)
		{
			Float4x4 attractorLocalToWorld = localToWorld * Float4x4::Translate(m_attractor.point);
			ShaderProperties shaderProperties = Renderer::DrawMesh(m_attractorSphereMesh, MaterialManager::GetMaterial("transparentMaterial"), attractorLocalToWorld, false, false);
			shaderProperties.SetValue("SurfaceProperties", "diffuseColor", Float4(1.0f, 0.0f, 0.0f, 0.25f));
		}
		m_particleMaterial.SetBuffer("positionBuffer", m_tripleData.positionBuffer.GetBuffer(m_tripleBufferState.GetReadIndex()));
		m_particleMaterial.SetBuffer("velocityBuffer", m_tripleData.velocityBuffer.GetBuffer(m_tripleBufferState.GetReadIndex()));
		m_particleMaterial.SetBuffer("densityBuffer", m_tripleData.densityBuffer.GetBuffer(m_tripleBufferState.GetReadIndex()));
		m_particleMaterial.SetBuffer("normalBuffer", m_tripleData.normalBuffer.GetBuffer(m_tripleBufferState.GetReadIndex()));
		m_particleMaterial.SetBuffer("curvatureBuffer", m_tripleData.curvatureBuffer.GetBuffer(m_tripleBufferState.GetReadIndex()));
		Renderer::DrawInstanced(m_particleCount, m_particleMesh, m_particleMaterial, m_shaderProperties, localToWorld, false, false);
	}



	// Setters:
	void SphFluid3dGpu::Reset()
	{
		m_reset = true;
	}
	void SphFluid3dGpu::SetIsRunning(bool isRunning)
	{
		m_isRunning = isRunning;
	}
	void SphFluid3dGpu::SetTimeScale(float timeScale)
	{
		m_timeScale = timeScale;
	}
	void SphFluid3dGpu::SetUseHashGridOptimization(bool useGridOptimization)
	{
		if (m_forceSetters || m_settings.useHashGridOptimization != useGridOptimization)
		{
			m_settings.useHashGridOptimization = useGridOptimization;
			m_computeShaders.SetUseHashGridOptimization(m_settings.useHashGridOptimization);
		}
	}
	void SphFluid3dGpu::SetParticleCount(int particleCount)
	{
		particleCount = math::Max(1, particleCount);
		if (m_forceSetters || m_particleCount != particleCount)
		{
			m_particleCount = particleCount;
			int hashGridSize = math::NextPrimeAbove(2 * m_particleCount);
			m_computeShaders.SetHashGridSize(hashGridSize);
			m_reset = true;
		}
	}
	void SphFluid3dGpu::SetEffectRadius(float effectRadius)
	{
		effectRadius = math::Max(1e-4f, effectRadius);
		if (m_forceSetters || m_settings.effectRadius != effectRadius)
		{
			m_settings.effectRadius = effectRadius;
			m_computeShaders.SetEffectRadius(m_settings.effectRadius);
		}
	}
	void SphFluid3dGpu::SetMass(float mass)
	{
		mass = math::Max(1e-4f, mass);
		if (m_forceSetters || m_settings.mass != mass)
		{
			m_settings.mass = mass;
			m_computeShaders.SetMass(m_settings.mass);
		}
	}
	void SphFluid3dGpu::SetViscosity(float viscosity)
	{
		if (m_forceSetters || m_settings.viscosity != viscosity)
		{
			m_settings.viscosity = viscosity;
			m_computeShaders.SetViscosity(m_settings.viscosity);
		}
	}
	void SphFluid3dGpu::SetSurfaceTension(float surfaceTension)
	{
		if (m_forceSetters || m_settings.surfaceTension != surfaceTension)
		{
			m_settings.surfaceTension = surfaceTension;
			m_computeShaders.SetSurfaceTension(m_settings.surfaceTension);
		}
	}
	void SphFluid3dGpu::SetCollisionDampening(float collisionDampening)
	{
		if (m_forceSetters || m_settings.collisionDampening != collisionDampening)
		{
			m_settings.collisionDampening = collisionDampening;
			m_computeShaders.SetCollisionDampening(m_settings.collisionDampening);
		}
	}
	void SphFluid3dGpu::SetTargetDensity(float targetDensity)
	{
		targetDensity = math::Max(1e-4f, targetDensity);
		if (m_forceSetters || m_settings.targetDensity != targetDensity)
		{
			m_settings.targetDensity = targetDensity;
			m_computeShaders.SetTargetDensity(m_settings.targetDensity);
			m_particleMaterial.SetValue("Values", "targetDensity", m_settings.targetDensity);
		}
	}
	void SphFluid3dGpu::SetPressureMultiplier(float pressureMultiplier)
	{
		if (m_forceSetters || m_settings.pressureMultiplier != pressureMultiplier)
		{
			m_settings.pressureMultiplier = pressureMultiplier;
			m_computeShaders.SetPressureMultiplier(m_settings.pressureMultiplier);
		}
	}
	void SphFluid3dGpu::SetGravity(float gravity)
	{
		if (m_forceSetters || m_settings.gravity != gravity)
		{
			m_settings.gravity = gravity;
			m_computeShaders.SetGravity(m_settings.gravity);
		}
	}
	void SphFluid3dGpu::SetMaxVelocity(float maxVelocity)
	{
		maxVelocity = math::Max(1e-4f, maxVelocity);
		if (m_forceSetters || m_settings.maxVelocity != maxVelocity)
		{
			m_settings.maxVelocity = maxVelocity;
			m_computeShaders.SetMaxVelocity(m_settings.maxVelocity);
			m_particleMaterial.SetValue("Values", "maxVelocity", m_settings.maxVelocity);
		}
	}
	void SphFluid3dGpu::SetFluidBounds(const Bounds& bounds)
	{
		if (m_forceSetters || m_settings.fluidBounds != bounds)
		{
			m_settings.fluidBounds = bounds;
			m_computeShaders.SetFluidBounds(m_settings.fluidBounds);
			SetAttractorPoint(m_settings.fluidBounds.center);
		}
	}
	void SphFluid3dGpu::SetAttractorRadius(float attractorRadius)
	{
		attractorRadius = math::Max(1e-4f, attractorRadius);
		if (m_forceSetters || m_attractor.radius != attractorRadius)
		{
			m_attractor.radius = attractorRadius;
			m_attractorSphereMesh = MeshGenerator::CubeSphere(m_attractor.radius, 3, "attractorSphere");
			m_computeShaders.SetAttractorRadius(m_attractor.radius);
		}
	}
	void SphFluid3dGpu::SetAttractorStrength(float attractorStrength)
	{
		attractorStrength = math::Max(1e-4f, attractorStrength);
		if (m_forceSetters || m_attractor.strength != attractorStrength)
		{
			m_attractor.strength = attractorStrength;
			m_computeShaders.SetAttractorStrength(m_attractor.strength);
		}
	}
	void SphFluid3dGpu::SetAttractorState(int attractorState)
	{
		if (m_forceSetters || m_attractor.state != attractorState)
		{
			m_attractor.state = attractorState;
			m_computeShaders.SetAttractorState(m_attractor.state);
		}
	}
	void SphFluid3dGpu::SetAttractorPoint(const Float3& attractorPoint)
	{
		if (m_forceSetters || m_attractor.point != attractorPoint)
		{
			m_attractor.point = attractorPoint;
			m_computeShaders.SetAttractorPoint(m_attractor.point);
		}
	}
	void SphFluid3dGpu::SetColorMode(int colorMode)
	{
		colorMode = math::Clamp(colorMode, 0, 3);
		if (m_forceSetters || m_colorMode != colorMode)
		{
			m_colorMode = colorMode;
			m_particleMaterial.SetValue("Values", "colorMode", m_colorMode);
		}
	}
	void SphFluid3dGpu::SetInitialDistributionRadius(float initialDistributionRadius)
	{
		initialDistributionRadius = math::Max(1e-4f, initialDistributionRadius);
		if (m_forceSetters || m_initialDistributionRadius != initialDistributionRadius)
		{
			m_initialDistributionRadius = initialDistributionRadius;
			m_reset = true;
		}
	}
	void SphFluid3dGpu::SetVisualRadius(float visualRadius)
	{
		visualRadius = math::Max(0.01f, visualRadius);
		if (m_forceSetters || m_visualRadius != visualRadius)
		{
			m_visualRadius = visualRadius;
			m_particleMesh = MeshGenerator::CubeSphere(m_visualRadius, 2, "fluidParticle");
		}
	}



	// Getters:
	bool SphFluid3dGpu::GetIsRunning() const
	{
		return m_isRunning;
	}
	float SphFluid3dGpu::GetTimeScale() const
	{
		return m_timeScale;
	}
	bool SphFluid3dGpu::GetUseGridOptimization() const
	{
		return m_settings.useHashGridOptimization;
	}
	uint32_t SphFluid3dGpu::GetTimeStep() const
	{
		return m_timeStep;
	}
	int SphFluid3dGpu::GetParticleCount() const
	{
		return m_particleCount;
	}
	float SphFluid3dGpu::GetEffectRadius() const
	{
		return m_settings.effectRadius;
	}
	float SphFluid3dGpu::GetMass() const
	{
		return m_settings.mass;
	}
	float SphFluid3dGpu::GetViscosity() const
	{
		return m_settings.viscosity;
	}
	float SphFluid3dGpu::GetSurfaceTension() const
	{
		return m_settings.surfaceTension;
	}
	float SphFluid3dGpu::GetCollisionDampening() const
	{
		return m_settings.collisionDampening;
	}
	float SphFluid3dGpu::GetTargetDensity() const
	{
		return m_settings.targetDensity;
	}
	float SphFluid3dGpu::GetPressureMultiplier() const
	{
		return m_settings.pressureMultiplier;
	}
	float SphFluid3dGpu::GetGravity() const
	{
		return m_settings.gravity;
	}
	float SphFluid3dGpu::GetMaxVelocity() const
	{
		return m_settings.maxVelocity;
	}
	Bounds SphFluid3dGpu::GetFluidBounds() const
	{
		return m_settings.fluidBounds;
	}
	float SphFluid3dGpu::GetAttractorRadius() const
	{
		return m_attractor.radius;
	}
	float SphFluid3dGpu::GetAttractorStrength() const
	{
		return m_attractor.strength;
	}
	int SphFluid3dGpu::GetColorMode() const
	{
		return m_colorMode;
	}
	float SphFluid3dGpu::GetInitialDistributionRadius() const
	{
		return m_initialDistributionRadius;
	}
	float SphFluid3dGpu::GetVisualRadius() const
	{
		return m_visualRadius;
	}



	// Debugging:
	void SphFluid3dGpu::Print()
	{
		LOG_INFO("Printing:");
		m_isRunning = false;

		std::vector<float> densities(m_particleCount);
		m_tripleData.densityBuffer.Download(m_tripleBufferState.GetSrcIndex(), densities);
		std::vector<Float3> positions(m_particleCount);
		m_tripleData.positionBuffer.Download(m_tripleBufferState.GetSrcIndex(), positions);
		std::vector<Float3> forceDensities(m_particleCount);
		m_scratchData.forceDensityBuffer.Download(forceDensities);

		for (int i = 0; i < m_particleCount; i++)
			LOG_TRACE("positions[{}] = {}, density[{}] = {}, forceDensity[{}] = {}", i, positions[i].ToString(), i, densities[i], i, forceDensities[i].ToString());
	}



	// Private methods:
	void SphFluid3dGpu::RecordReset()
	{
		if (m_pendingResetSessionID != Compute::Physics::invalidPhysicsSessionID)
		{
			if (!Compute::Physics::IsFinished(m_pendingResetSessionID))
				return;
			m_pendingResetSessionID = Compute::Physics::invalidPhysicsSessionID;
			m_reset = false;
			return;
		}
		if (!Compute::Physics::IsFinished())
			return;

		LOG_INFO("reset");
		m_timeStep = 0;
		m_tripleBufferState.Reset();
		m_pendingResetSessionID = Compute::Physics::GetRecordingSessionID();
		m_scratchData.Reallocate(m_particleCount);
		m_tripleData.Reallocate(m_particleCount);

		Compute::RecordBarrierWaitStorageWriteBeforeReadWrite(m_computeShaders.computeType, m_computeShaders.sessionID);
		// Reset all buffer slots:
		for (uint32_t i = 0; i < PhysicsTripleBufferState::bufferCount; i++)
		{
			SphFluid3dGpuSolver::ResetData(m_computeShaders, m_scratchData, m_tripleData, i, m_initialDistributionRadius);
			Compute::RecordBarrierWaitStorageWriteBeforeReadWrite(m_computeShaders.computeType, m_computeShaders.sessionID);
		}
		Compute::RecordBarrierWaitStorageWriteBeforeRead(m_computeShaders.computeType, m_computeShaders.sessionID);

		m_isRunning = false;
	}
}