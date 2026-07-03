#include "sphFluid2dGpu.h"
#include "logger.h"
#include "sphFluid2dGpuEditorWindow.h"



namespace fluidDynamics
{
	// Public methods:
	// Constructor/Destructor:
	SphFluid2dGpu::SphFluid2dGpu()
	{
		// Material setup:
		m_particleMaterial = MaterialManager::GetMaterial("particleMaterial2d");
		m_shaderProperties = ShaderProperties(m_particleMaterial);

        m_forceSetters = true;
        //// Settings for old pressure:
		//{
		//	// Management:
		//	SetTimeScale(2.0f);
		//	SetParticleCount(10000);
        //
		//	// Settings:
		//	SetUseHashGridOptimization(false);
		//	SetEffectRadius(0.3f);
		//	SetMass(1.0f);
		//	SetViscosity(5.0f);
		//	SetSurfaceTension(0.5f);
		//	SetCollisionDampening(0.80f);
		//	SetTargetDensity(25.0f);
		//	SetPressureMultiplier(80.0f);
		//	SetNearPressureRatio(0.1f);
		//	SetGravity(1.0f);
		//	SetMaxVelocity(10.0f);
		//	SetFluidBounds(Bounds(Float3::zero, Float3(16.0f, 9.0f, 0.01f)));
        //
		//	// User Interaction:
		//	SetAttractorRadius(3.0f);
		//	SetAttractorStrength(2.0f);
        //
		//	// Visuals:
		//	SetColorMode(0);
		//	SetInitialDistributionRadius(6.0f);
		//	SetVisualRadius(0.25f);
		//}
        // Settings for new pressure:
		{
			// Management:
			SetTimeScale(2.0f);
			SetParticleCount(10000);

			// Settings:
			SetUseHashGridOptimization(false);
			SetEffectRadius(0.3f);
			SetMass(1.0f);
			SetViscosity(3.0f);
			SetSurfaceTension(0.5f);
			SetCollisionDampening(0.80f);
			SetTargetDensity(25.0f);
			SetPressureMultiplier(40.0f);
			SetNearPressureRatio(0.1f);
			SetGravity(1.0f);
			SetMaxVelocity(10.0f);
			SetFluidBounds(Bounds(Float3::zero, Float3(16.0f, 9.0f, 0.01f)));

			// User Interaction:
			SetAttractorRadius(3.0f);
			SetAttractorStrength(2.0f);

			// Visuals:
			SetColorMode(0);
			SetInitialDistributionRadius(6.0f);
			SetVisualRadius(0.25f);
		}
        m_forceSetters = false;

		Reset();
	}
	SphFluid2dGpu::~SphFluid2dGpu()
	{

	}



	// Overrides:
	void SphFluid2dGpu::Start()
	{
		editorWindow = std::make_unique<emberEditor::SphFluid2dGpuEditorWindow>(this);
	}
	void SphFluid2dGpu::FixedUpdate()
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
			SphFluid2dGpuSolver::TimeStepRungeKutta2(deltaT, m_settings, m_attractor, m_computeShaders, m_scratchData, m_tripleData, sourceDataIndex, destinationDataIndex);
			m_tripleBufferState.CommitWrite(Compute::Physics::GetRecordingSessionID());
			sourceDataIndex = m_tripleBufferState.GetSrcIndex();
			destinationDataIndex = m_tripleBufferState.GetDstIndex();
			restTime -= deltaT;
		}
		m_timeStep++;
	}
	void SphFluid2dGpu::Update()
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
			else
			{
				float zoomFactor = 1.0f + 0.1f * mouseScroll;
				SetAttractorRadius(zoomFactor * m_attractor.radius);
			}
		}

		// Mouse left and right click:
		if (EventSystem::MouseHeld(Input::MouseButton::Left) ^ EventSystem::MouseHeld(Input::MouseButton::Right)) // exlusive or
		{
			Ray ray = Scene::GetActiveScene()->GetActiveCamera()->GetClickRay();
			std::optional<Float3> hit = m_settings.fluidBounds.IntersectRay(ray);
			if (hit.has_value())
			{
				SetAttractorPoint(Float2(hit.value()));
				ShaderProperties shaderProperties = Renderer::DrawMesh(m_ringMesh, MaterialManager::GetMaterial("simpleUnlitMaterial"), Float4x4::TRS(hit.value(), Float3x3::identity, Float3(1.0f)), false, false);
				shaderProperties.SetValue("SurfaceProperties", "diffuseColor", Float4::red);
				if (EventSystem::MouseHeld(Input::MouseButton::Left))
					SetAttractorState(1);
				if (EventSystem::MouseHeld(Input::MouseButton::Right))
					SetAttractorState(-1);
			}
			else
				SetAttractorState(0);
		}
		else
			SetAttractorState(0);

		// Rendering:
		Float4x4 localToWorld = GetTransform()->GetLocalToWorldMatrix();
		DebugRenderer::SetColor(Float4::white);
		DebugRenderer::SetReceiveShadows(false);
		DebugRenderer::SetCastShadows(false);
		DebugRenderer::DrawBounds(localToWorld, m_settings.fluidBounds, 0.01f);
		m_tripleBufferState.MarkRead();
		uint32_t readDataIndex = m_tripleBufferState.GetReadIndex();
		m_particleMaterial.SetBuffer("positionBuffer", m_tripleData.positionBuffer.GetBuffer(readDataIndex));
		m_particleMaterial.SetBuffer("velocityBuffer", m_tripleData.velocityBuffer.GetBuffer(readDataIndex));
		m_particleMaterial.SetBuffer("densityBuffer", m_tripleData.densityBuffer.GetBuffer(readDataIndex));
		m_particleMaterial.SetBuffer("normalBuffer", m_tripleData.normalBuffer.GetBuffer(readDataIndex));
		m_particleMaterial.SetBuffer("curvatureBuffer", m_tripleData.curvatureBuffer.GetBuffer(readDataIndex));
		Renderer::DrawInstanced(m_particleCount, m_particleMesh, m_particleMaterial, m_shaderProperties, localToWorld, false, false);
	}



	// Setters:
	void SphFluid2dGpu::Reset()
	{
		m_reset = true;
	}
	void SphFluid2dGpu::SetIsRunning(bool isRunning)
	{
		m_isRunning = isRunning;
	}
	void SphFluid2dGpu::SetTimeScale(float timeScale)
	{
		m_timeScale = timeScale;
	}
	void SphFluid2dGpu::SetUseHashGridOptimization(bool useGridOptimization)
	{
		if (m_forceSetters || m_settings.useHashGridOptimization != useGridOptimization)
		{
			m_settings.useHashGridOptimization = useGridOptimization;
			m_computeShaders.SetUseHashGridOptimization(m_settings.useHashGridOptimization);
		}
	}
	void SphFluid2dGpu::SetParticleCount(int particleCount)
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
	void SphFluid2dGpu::SetEffectRadius(float effectRadius)
	{
		effectRadius = math::Max(1e-4f, effectRadius);
		if (m_forceSetters || m_settings.effectRadius != effectRadius)
		{
			m_settings.effectRadius = effectRadius;
			m_computeShaders.SetEffectRadius(m_settings.effectRadius);
		}
	}
	void SphFluid2dGpu::SetMass(float mass)
	{
		mass = math::Max(1e-4f, mass);
		if (m_forceSetters || m_settings.mass != mass)
		{
			m_settings.mass = mass;
			m_computeShaders.SetMass(m_settings.mass);
		}
	}
	void SphFluid2dGpu::SetViscosity(float viscosity)
	{
		if (m_forceSetters || m_settings.viscosity != viscosity)
		{
			m_settings.viscosity = viscosity;
			m_computeShaders.SetViscosity(m_settings.viscosity);
		}
	}
	void SphFluid2dGpu::SetSurfaceTension(float surfaceTension)
	{
		if (m_forceSetters || m_settings.surfaceTension != surfaceTension)
		{
			m_settings.surfaceTension = surfaceTension;
			m_computeShaders.SetSurfaceTension(m_settings.surfaceTension);
		}
	}
	void SphFluid2dGpu::SetCollisionDampening(float collisionDampening)
	{
		if (m_forceSetters || m_settings.collisionDampening != collisionDampening)
		{
			m_settings.collisionDampening = collisionDampening;
			m_computeShaders.SetCollisionDampening(m_settings.collisionDampening);
		}
	}
	void SphFluid2dGpu::SetTargetDensity(float targetDensity)
	{
		targetDensity = math::Max(1e-4f, targetDensity);
		if (m_forceSetters || m_settings.targetDensity != targetDensity)
		{
			m_settings.targetDensity = targetDensity;
			m_computeShaders.SetTargetDensity(m_settings.targetDensity);
			m_particleMaterial.SetValue("Values", "targetDensity", m_settings.targetDensity);
		}
	}
	void SphFluid2dGpu::SetPressureMultiplier(float pressureMultiplier)
	{
		if (m_forceSetters || m_settings.pressureMultiplier != pressureMultiplier)
		{
			m_settings.pressureMultiplier = pressureMultiplier;
			m_computeShaders.SetPressureMultiplier(m_settings.pressureMultiplier);
		}
	}
	void SphFluid2dGpu::SetNearPressureRatio(float nearPressureRatio)
	{
		nearPressureRatio = math::Max(0.0f, nearPressureRatio);
		if (m_forceSetters || m_settings.nearPressureRatio != nearPressureRatio)
		{
			m_settings.nearPressureRatio = nearPressureRatio;
			m_computeShaders.SetNearPressureRatio(m_settings.nearPressureRatio);
		}
	}
	void SphFluid2dGpu::SetGravity(float gravity)
	{
		if (m_forceSetters || m_settings.gravity != gravity)
		{
			m_settings.gravity = gravity;
			m_computeShaders.SetGravity(m_settings.gravity);
		}
	}
	void SphFluid2dGpu::SetMaxVelocity(float maxVelocity)
	{
		maxVelocity = math::Max(1e-4f, maxVelocity);
		if (m_forceSetters || m_settings.maxVelocity != maxVelocity)
		{
			m_settings.maxVelocity = maxVelocity;
			m_computeShaders.SetMaxVelocity(m_settings.maxVelocity);
			m_particleMaterial.SetValue("Values", "maxVelocity", m_settings.maxVelocity);
		}
	}
	void SphFluid2dGpu::SetFluidBounds(const Bounds& bounds)
	{
		if (m_forceSetters || m_settings.fluidBounds != bounds)
		{
			m_settings.fluidBounds = bounds;
			m_computeShaders.SetFluidBounds(m_settings.fluidBounds);
		}
	}
	void SphFluid2dGpu::SetAttractorRadius(float attractorRadius)
	{
		attractorRadius = math::Max(1e-4f, attractorRadius);
		if (m_forceSetters || m_attractor.radius != attractorRadius)
		{
			m_attractor.radius = attractorRadius;
			m_ringMesh = MeshGenerator::ArcFlatUv(m_attractor.radius - 0.1f, m_attractor.radius + 0.1f, 360.0f, 100, "attractorRing");
			m_computeShaders.SetAttractorRadius(m_attractor.radius);
		}
	}
	void SphFluid2dGpu::SetAttractorStrength(float attractorStrength)
	{
		attractorStrength = math::Max(1e-4f, attractorStrength);
		if (m_forceSetters || m_attractor.strength != attractorStrength)
		{
			m_attractor.strength = attractorStrength;
			m_computeShaders.SetAttractorStrength(m_attractor.strength);
		}
	}
	void SphFluid2dGpu::SetAttractorState(int attractorState)
	{
		if (m_forceSetters || m_attractor.state != attractorState)
		{
			m_attractor.state = attractorState;
			m_computeShaders.SetAttractorState(m_attractor.state);
		}
	}
	void SphFluid2dGpu::SetAttractorPoint(const Float2& attractorPoint)
	{
		if (m_forceSetters || m_attractor.point != attractorPoint)
		{
			m_attractor.point = attractorPoint;
			m_computeShaders.SetAttractorPoint(m_attractor.point);
		}
	}
	void SphFluid2dGpu::SetColorMode(int colorMode)
	{
		colorMode = math::Clamp(colorMode, 0, 3);
		if (m_forceSetters || m_colorMode != colorMode)
		{
			m_colorMode = colorMode;
			m_particleMaterial.SetValue("Values", "colorMode", m_colorMode);
		}
	}
	void SphFluid2dGpu::SetInitialDistributionRadius(float initialDistributionRadius)
	{
		initialDistributionRadius = math::Max(1e-4f, initialDistributionRadius);
		if (m_forceSetters || m_initialDistributionRadius != initialDistributionRadius)
		{
			m_initialDistributionRadius = initialDistributionRadius;
			m_reset = true;
		}
	}
	void SphFluid2dGpu::SetVisualRadius(float visualRadius)
	{
		visualRadius = math::Max(0.01f, visualRadius);
		if (m_forceSetters || m_visualRadius != visualRadius)
		{
			m_visualRadius = visualRadius;
			m_particleMesh = MeshGenerator::Quad().Scale(m_visualRadius);
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
		return m_settings.useHashGridOptimization;
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
		return m_settings.effectRadius;
	}
	float SphFluid2dGpu::GetMass() const
	{
		return m_settings.mass;
	}
	float SphFluid2dGpu::GetViscosity() const
	{
		return m_settings.viscosity;
	}
	float SphFluid2dGpu::GetSurfaceTension() const
	{
		return m_settings.surfaceTension;
	}
	float SphFluid2dGpu::GetCollisionDampening() const
	{
		return m_settings.collisionDampening;
	}
	float SphFluid2dGpu::GetTargetDensity() const
	{
		return m_settings.targetDensity;
	}
	float SphFluid2dGpu::GetPressureMultiplier() const
	{
		return m_settings.pressureMultiplier;
	}
	float SphFluid2dGpu::GetNearPressureRatio() const
	{
		return m_settings.nearPressureRatio;
	}
	float SphFluid2dGpu::GetGravity() const
	{
		return m_settings.gravity;
	}
	float SphFluid2dGpu::GetMaxVelocity() const
	{
		return m_settings.maxVelocity;
	}
	Bounds SphFluid2dGpu::GetFluidBounds() const
	{
		return m_settings.fluidBounds;
	}
	float SphFluid2dGpu::GetAttractorRadius() const
	{
		return m_attractor.radius;
	}
	float SphFluid2dGpu::GetAttractorStrength() const
	{
		return m_attractor.strength;
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



	// Debugging:
	void SphFluid2dGpu::Print()
	{
		LOG_INFO("Printing:");
		m_isRunning = false;

		std::vector<float> densities(m_particleCount);
		m_tripleData.densityBuffer.Download(m_tripleBufferState.GetSrcIndex(), densities);
		std::vector<Float2> positions(m_particleCount);
		m_tripleData.positionBuffer.Download(m_tripleBufferState.GetSrcIndex(), positions);
		std::vector<Float2> forceDensities(m_particleCount);
		m_scratchData.forceDensityBuffer.Download(forceDensities);
		for (int i = 0; i < m_particleCount; i++)
			LOG_TRACE("positions[{}] = {}, density[{}] = {}, forceDensity[{}] = {}", i, positions[i].ToString(), i, densities[i], i, forceDensities[i].ToString());
	}



	// Private methods:
	void SphFluid2dGpu::RecordReset()
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
			SphFluid2dGpuSolver::ResetData(m_computeShaders, m_scratchData, m_tripleData, i, m_initialDistributionRadius);
			Compute::RecordBarrierWaitStorageWriteBeforeReadWrite(m_computeShaders.computeType, m_computeShaders.sessionID);
		}
		Compute::RecordBarrierWaitStorageWriteBeforeRead(m_computeShaders.computeType, m_computeShaders.sessionID);

		m_isRunning = false;
	}
}