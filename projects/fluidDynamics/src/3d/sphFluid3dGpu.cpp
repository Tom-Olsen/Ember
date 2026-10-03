#include "sphFluid3dGpu.h"
#include "logger.h"
#include "sphFluid3dGpuEditorWindow.h"



namespace fluidDynamics
{
	// Public methods:
	// Constructor/Destructor:
	SphFluid3dGpu::SphFluid3dGpu(Texture& environmentMap)
		: m_pEnvironmentMap(&environmentMap)
	{
		// Material setup:
		m_particleMaterial = MaterialManager::TryGetForwardMaterial("particleMaterial3d");
		ShadowMaterial particleShadowMaterial = MaterialManager::TryGetShadowMaterial("particleShadowMaterial3d");
		m_particleMaterial.SetShadowMaterial(particleShadowMaterial);
		m_volumeRaycastMaterial = MaterialManager::TryGetForwardMaterial("volumeRaycastMaterial");
		m_volumeRaycastMaterial.SetCullMode(emberCommon::CullMode::front);	// Drawing back-facing triangles enables the camera to enter the fluid volume while it remains rendered.
		m_water0ComputeShader = ComputeShaderManager::TryGetComputeShader("water0");
		m_particleMesh = MeshGenerator::Quad();
		m_volumetricDensityCube = MeshGenerator::Cube();
		m_callProperties = CallProperties(m_particleMaterial);

		m_forceSetters = true;
		{
            // Management:
			SetUseHashGridOptimization(true);
			SetFluidBounds(RotatedBounds(Float3::zero, Float3(16.0f, 12.0f, 10.0f)));

			// Time:
			SetTimeScale(1.0f);
			SetPhysicsTimeScale(1.1f);

			// Fluid:
			SetParticleCount(100000);
			SetInitialDistributionRadius(9.0f);
			SetEffectRadius(1.0f);
			SetMass(1.0f);
			SetViscosity(2.0f);
			SetSurfaceTension(0.5f);
			SetCollisionDampening(0.5f);
			SetTargetDensity(12.5f);
			SetPressureMultiplier(300.0f);
			SetNearPressureRatio(0.01f);
			SetMaxVelocity(30.0f);

            // Forces:
			SetGravity(9.81f);
			SetAttractorRadius(6.0f);
			SetAttractorStrength(10.0f);
			SetAttractorState(0);

			// Visuals:
			SetRenderMode(RenderMode::water);
			// Particles:
			SetColorMode(0);
			SetVisualRadius(0.2f);
			// Cloud:
			SetVolumetricDensityResolution(Uint3(160, 120, 100));
			SetVolumetricDensityRayStepLength(0.4f);
            SetVolumetricDensityAbsorption(0.0001f);
			SetVolumetricScattering(Float3(0.01f, 0.04f, 0.08f));
			SetRenderVolumetricLight(true);
			SetVolumetricLightingResolution(Uint3(120));
			// Water:
		}
		m_forceSetters = false;

		Reset();
	}
	SphFluid3dGpu::~SphFluid3dGpu()
	{

	}



	// Overrides:
	void SphFluid3dGpu::Start()
	{
		editorWindow = std::make_unique<emberEditor::SphFluid3dGpuEditorWindow>(this);
	}
	void SphFluid3dGpu::FixedUpdate()
	{
		if (m_reset)
		{
			RecordReset();
			return;
		}
		if (m_pendingVolumetricDensityResolutionChange || m_pendingVolumetricLightingResolutionChange)
		{
			Compute::Physics::WaitForFinish();
			if (m_pendingVolumetricDensityResolutionChange)
				m_tripleData.ReallocateDensityTexture3d(m_volumetricDensityResolution);
			if (m_pendingVolumetricLightingResolutionChange)
				m_tripleData.ReallocateOpticalDepthTexture3d(m_volumetricLightingResolution);
			m_pendingVolumetricDensityResolutionChange = false;
			m_pendingVolumetricLightingResolutionChange = false;
			m_pendingRenderRefresh = true;
		}
		// ToDo: update optical depth when the directional light or lighting settings change while the simulation is paused.
		if (!m_isRunning)
			return;
		m_tripleBufferState.PublishFinishedWrites();

		// Do multiple iterations of deltaT<=dt if timeScale is bigger 1. Otherwise 1 iteration per FixedUpdate().
		float dt = Time::GetFixedDeltaTime();
		float timeStep = m_timeScale * dt;
		float restTime = timeStep;
		uint32_t sourceDataIndex = m_tripleBufferState.GetSrcIndex();
		uint32_t destinationDataIndex = m_tripleBufferState.GetDstIndex();
		bool physicsDataWritten = false;
		while (restTime > 0.0f)
		{
			float deltaT = math::Min(dt, restTime);
			SphFluid3dGpuSolver::TimeStepRungeKutta2(deltaT, m_settings, m_computeShaders, m_scratchData, m_tripleData, sourceDataIndex, destinationDataIndex);
			m_tripleBufferState.CommitWrite(Compute::Physics::GetRecordingSessionID());
			physicsDataWritten = true;
			sourceDataIndex = m_tripleBufferState.GetSrcIndex();
			destinationDataIndex = m_tripleBufferState.GetDstIndex();
			restTime -= deltaT;
		}
		m_timeStep++;

		// Refresh volume textures for the newly written particle state:
		if (physicsDataWritten && (m_renderMode == RenderMode::cloud || m_renderMode == RenderMode::water))
		{
			RecordVolumetricRenderData(m_computeShaders, sourceDataIndex);
			m_pendingRenderRefresh = false;
		}
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
		if (m_pendingRenderRefresh)
			RefreshRenderData();

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
		Gizmo::SetDefaultState();
		Gizmo::DrawRotatedBounds(localToWorld, m_settings.fluidBounds, 0.2f);
		if (m_attractor.state != 0)
		{
			Float4x4 attractorLocalToWorld = localToWorld * Float4x4::Translate(m_attractor.point);
			Material attractorMaterial = MaterialManager::TryGetMaterial("transparentMaterial");
			DrawData drawData(attractorLocalToWorld, m_attractorSphereMesh, attractorMaterial, false, false);
			CallProperties callProperties = Renderer::DrawMesh(drawData);
			callProperties.SetValue("SurfaceProperties", "surface_diffuseColor", Float4(1.0f, 0.0f, 0.0f, 0.25f));
		}
		m_tripleBufferState.MarkRead();
		m_lastRenderFrameIndex = Renderer::GetFrameIndex();
		uint32_t readDataIndex = m_tripleBufferState.GetReadIndex();
		switch (m_renderMode)
		{
			case RenderMode::particles:
			{
				m_particleMaterial.SetBuffer("positionBuffer", m_tripleData.positionBuffer.GetBuffer(readDataIndex));
				m_particleMaterial.SetBuffer("velocityBuffer", m_tripleData.velocityBuffer.GetBuffer(readDataIndex));
				m_particleMaterial.SetBuffer("densityBuffer", m_tripleData.densityBuffer.GetBuffer(readDataIndex));
				m_particleMaterial.SetBuffer("normalBuffer", m_tripleData.normalBuffer.GetBuffer(readDataIndex));
				m_particleMaterial.SetBuffer("curvatureBuffer", m_tripleData.curvatureBuffer.GetBuffer(readDataIndex));
				ShadowMaterial shadowMaterial = m_particleMaterial.GetShadowMaterial();
				if (shadowMaterial.IsValid())
					shadowMaterial.SetBuffer("positionBuffer", m_tripleData.positionBuffer.GetBuffer(readDataIndex));
				DrawData drawData(localToWorld, m_particleMesh, m_particleMaterial, m_particleCount, nullptr, true, true);
				Renderer::DrawMesh(drawData, m_callProperties);
				break;
			}
			case RenderMode::cloud:
			{
				const RotatedBounds& fluidBounds = m_tripleData.fluidBounds[readDataIndex];

            	// Compute fluid to light matrix:
				Float4x4 fluidToLightMatrix = Float4x4::identity;
            	bool renderVolumetricLight = m_renderVolumetricLight && m_tripleData.hasOpticalDepthTexture3d[readDataIndex];
				if (renderVolumetricLight)
				{
					const RotatedBounds& lightBounds = m_tripleData.opticalDepthBounds[readDataIndex];
					Float3 fluidSize = fluidBounds.localBounds.GetSize();
					Float3 lightSize = lightBounds.localBounds.GetSize();
					Float4x4 fluidToSimulationMatrix = Float4x4::Translate(fluidBounds.localBounds.center)
						* fluidBounds.GetRotation4x4()
						* Float4x4::Scale(fluidSize)
						* Float4x4::Translate(Float3(-0.5f));
					Float4x4 simulationToLightMatrix = Float4x4::Translate(Float3(0.5f))
						* Float4x4::Scale(Float3::one / lightSize)
						* lightBounds.GetRotation4x4().Inverse()
						* Float4x4::Translate(-lightBounds.localBounds.center);
					fluidToLightMatrix = simulationToLightMatrix * fluidToSimulationMatrix;
				}

            	// Set shader values and bind textures:
				m_volumeRaycastMaterial.SetValue("Values", "fluidSize", fluidBounds.localBounds.GetSize());
				m_volumeRaycastMaterial.SetValue("Values", "absorption", m_volumetricDensityAbsorption);
				m_volumeRaycastMaterial.SetValue("Values", "fluidToLightMatrix", fluidToLightMatrix);
				m_volumeRaycastMaterial.SetValue("Values", "renderVolumetricLight", static_cast<int>(renderVolumetricLight));
				m_volumeRaycastMaterial.SetTexture("densityTexture", m_tripleData.densityTexture3d[readDataIndex]);
				m_volumeRaycastMaterial.SetTexture("opticalDepthTexture", m_tripleData.opticalDepthTexture3d[readDataIndex]);

            	// Draw density cube mesh:
				Float4x4 densityCubeLocalToWorld = localToWorld
					* Float4x4::Translate(fluidBounds.localBounds.center)
					* fluidBounds.GetRotation4x4()
					* Float4x4::Scale(fluidBounds.localBounds.GetSize());
				DrawData drawData(densityCubeLocalToWorld, m_volumetricDensityCube, m_volumeRaycastMaterial, false, false);
				Renderer::DrawMesh(drawData);
				break;
			}
			case RenderMode::water:
			{
				const RotatedBounds& fluidBounds = m_tripleData.fluidBounds[readDataIndex];
				Float4x4 fluidToWorld = localToWorld
					* Float4x4::Translate(fluidBounds.localBounds.center)
					* fluidBounds.GetRotation4x4()
					* Float4x4::Scale(fluidBounds.localBounds.GetSize())
					* Float4x4::Translate(Float3(-0.5f));

				CallProperties callProperties = Compute::PostRender::RecordPostProcessingShader(m_water0ComputeShader);
				callProperties.SetValue("CallValues", "worldToFluidMatrix", fluidToWorld.Inverse());
				callProperties.SetValue("CallValues", "surfaceDensity", 0.5f * m_settings.targetDensity);
				callProperties.SetValue("CallValues", "densityRayStepLength", m_volumetricDensityRayStepLength);
				callProperties.SetValue("CallValues", "surfaceBias", 0.01f);
				callProperties.SetValue("CallValues", "fluidIndexOfRefraction", 1.333f);
				callProperties.SetValue("CallValues", "absorption", Float3(m_volumetricDensityAbsorption));
				callProperties.SetValue("CallValues", "normalSampleDistance", 1.0f);
				callProperties.SetValue("CallValues", "sceneRayStepLength", m_volumetricDensityRayStepLength);
				callProperties.SetValue("CallValues", "sceneRayMaxDistance", 0.0f);
				callProperties.SetValue("CallValues", "sceneSurfaceThickness", 0.05f);
				callProperties.SetValue("CallValues", "environmentMipLevel", 3.0f);
				callProperties.SetTexture("densityTexture", m_tripleData.densityTexture3d[readDataIndex]);
				callProperties.SetTexture("environmentMap", *m_pEnvironmentMap);
				break;
			}
		default:
			break;
		}
	}



	// Setters:
    // Management:
	void SphFluid3dGpu::Reset()
	{
		m_reset = true;
	}
	void SphFluid3dGpu::SetIsRunning(bool isRunning)
	{
		m_isRunning = isRunning;
	}
	void SphFluid3dGpu::SetUseHashGridOptimization(bool useGridOptimization)
	{
		if (m_forceSetters || m_settings.useHashGridOptimization != useGridOptimization)
		{
			m_settings.useHashGridOptimization = useGridOptimization;
			m_computeShaders.SetUseHashGridOptimization(m_settings.useHashGridOptimization);
		}
	}
	void SphFluid3dGpu::SetFluidBounds(const RotatedBounds& bounds)
	{
		if (m_forceSetters || m_settings.fluidBounds != bounds)
		{
			m_settings.fluidBounds = bounds;
			SetAttractorPoint(m_settings.fluidBounds.localBounds.center);
		}
	}
    // Time:
	void SphFluid3dGpu::SetTimeScale(float timeScale)
	{
		m_timeScale = timeScale;
	}
	void SphFluid3dGpu::SetPhysicsTimeScale(float physicsTimeScale)
	{
		physicsTimeScale = math::Max(1e-4f, physicsTimeScale);
		if (m_forceSetters || m_physicsTimeScale != physicsTimeScale)
		{
			m_physicsTimeScale = physicsTimeScale;
		    float accelerationScale = m_physicsTimeScale * m_physicsTimeScale;
		    m_computeShaders.SetViscosity(m_physicsTimeScale * m_settings.viscosity);
		    m_computeShaders.SetSurfaceTension(accelerationScale * m_settings.surfaceTension);
		    m_computeShaders.SetPressureMultiplier(accelerationScale * m_settings.pressureMultiplier);
		    m_computeShaders.SetGravity(accelerationScale * m_settings.gravity);
		    m_computeShaders.SetMaxVelocity(m_physicsTimeScale * m_settings.maxVelocity);
		    m_computeShaders.SetAttractorStrength(accelerationScale * m_attractor.strength);
		    m_particleMaterial.SetValue("Values", "maxVelocity", m_physicsTimeScale * m_settings.maxVelocity);
		}
	}
    // Fluid:
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
	void SphFluid3dGpu::SetInitialDistributionRadius(float initialDistributionRadius)
	{
		initialDistributionRadius = math::Max(1e-4f, initialDistributionRadius);
		if (m_forceSetters || m_initialDistributionRadius != initialDistributionRadius)
		{
			m_initialDistributionRadius = initialDistributionRadius;
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
			m_computeShaders.SetViscosity(m_physicsTimeScale * m_settings.viscosity);
		}
	}
	void SphFluid3dGpu::SetSurfaceTension(float surfaceTension)
	{
		if (m_forceSetters || m_settings.surfaceTension != surfaceTension)
		{
			m_settings.surfaceTension = surfaceTension;
			m_computeShaders.SetSurfaceTension(m_physicsTimeScale * m_physicsTimeScale * m_settings.surfaceTension);
		}
	}
	void SphFluid3dGpu::SetCollisionDampening(float collisionDampening)
	{
		if (m_forceSetters || m_settings.collisionDampening != collisionDampening)
			m_settings.collisionDampening = collisionDampening;
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
			m_computeShaders.SetPressureMultiplier(m_physicsTimeScale * m_physicsTimeScale * m_settings.pressureMultiplier);
		}
	}
	void SphFluid3dGpu::SetNearPressureRatio(float nearPressureRatio)
	{
		nearPressureRatio = math::Max(0.0f, nearPressureRatio);
		if (m_forceSetters || m_settings.nearPressureRatio != nearPressureRatio)
		{
			m_settings.nearPressureRatio = nearPressureRatio;
			m_computeShaders.SetNearPressureRatio(m_settings.nearPressureRatio);
		}
	}
	void SphFluid3dGpu::SetMaxVelocity(float maxVelocity)
	{
		maxVelocity = math::Max(1e-4f, maxVelocity);
		if (m_forceSetters || m_settings.maxVelocity != maxVelocity)
		{
			m_settings.maxVelocity = maxVelocity;
			m_computeShaders.SetMaxVelocity(m_physicsTimeScale * m_settings.maxVelocity);
			m_particleMaterial.SetValue("Values", "maxVelocity", m_physicsTimeScale * m_settings.maxVelocity);
		}
	}
    // Forces:
	void SphFluid3dGpu::SetGravity(float gravity)
	{
		if (m_forceSetters || m_settings.gravity != gravity)
		{
			m_settings.gravity = gravity;
			m_computeShaders.SetGravity(m_physicsTimeScale * m_physicsTimeScale * m_settings.gravity);
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
			m_computeShaders.SetAttractorStrength(m_physicsTimeScale * m_physicsTimeScale * m_attractor.strength);
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
	// Visuals:
	void SphFluid3dGpu::SetRenderMode(SphFluid3dGpu::RenderMode renderMode)
	{
		if (m_renderMode != renderMode)
		{
			m_renderMode = renderMode;
			m_pendingRenderRefresh = true;
		}
	}
    // Particles:
	void SphFluid3dGpu::SetColorMode(int colorMode)
	{
		colorMode = math::Clamp(colorMode, 0, 3);
		if (m_forceSetters || m_colorMode != colorMode)
		{
			m_colorMode = colorMode;
			m_particleMaterial.SetValue("Values", "colorMode", m_colorMode);
		}
	}
	void SphFluid3dGpu::SetVisualRadius(float visualRadius)
	{
		visualRadius = math::Max(0.01f, visualRadius);
		if (m_forceSetters || m_visualRadius != visualRadius)
		{
			m_visualRadius = visualRadius;
			m_particleMaterial.SetValue("Values", "renderWidth", 2.0f * m_visualRadius);
			m_particleMaterial.GetShadowMaterial().SetValue("Values", "renderWidth", 2.0f * m_visualRadius);
		}
	}
    // Cloud:
	void SphFluid3dGpu::SetVolumetricDensityResolution(const Uint3& volumetricDensityResolution)
	{
		Uint3 resolution = Uint3::Max(volumetricDensityResolution, Uint3::one);
		if (m_forceSetters || m_volumetricDensityResolution != resolution)
		{
			m_volumetricDensityResolution = resolution;
			m_pendingVolumetricDensityResolutionChange = true;
		}
	}
	void SphFluid3dGpu::SetVolumetricDensityRayStepLength(float volumetricDensityRayStepLength)
	{
		volumetricDensityRayStepLength = math::Max(1e-4f, volumetricDensityRayStepLength);
		if (m_forceSetters || m_volumetricDensityRayStepLength != volumetricDensityRayStepLength)
		{
			m_volumetricDensityRayStepLength = volumetricDensityRayStepLength;
			m_volumeRaycastMaterial.SetValue("Values", "rayStepLengthSimulation", m_volumetricDensityRayStepLength);
		}
	}
	void SphFluid3dGpu::SetVolumetricDensityAbsorption(float volumetricDensityAbsorption)
	{
		volumetricDensityAbsorption = math::Max(1e-4f, volumetricDensityAbsorption);
		if (m_forceSetters || m_volumetricDensityAbsorption != volumetricDensityAbsorption)
			m_volumetricDensityAbsorption = volumetricDensityAbsorption;
	}
	void SphFluid3dGpu::SetVolumetricScattering(const Float3& volumetricScattering)
	{
		Float3 scattering = Float3::Max(Float3::zero, volumetricScattering);
		if (m_forceSetters || m_volumetricScattering != scattering)
		{
			m_volumetricScattering = scattering;
			m_volumeRaycastMaterial.SetValue("Values", "scattering", m_volumetricScattering);
		}
	}
	void SphFluid3dGpu::SetRenderVolumetricLight(bool renderVolumetricLight)
	{
		m_renderVolumetricLight = renderVolumetricLight;
	}
	void SphFluid3dGpu::SetVolumetricLightingResolution(const Uint3& volumetricLightingResolution)
	{
		Uint3 resolution = Uint3::Max(volumetricLightingResolution, Uint3::one);
		if (m_forceSetters || m_volumetricLightingResolution != resolution)
		{
			m_volumetricLightingResolution = resolution;
			m_pendingVolumetricLightingResolutionChange = true;
		}
	}
	// Water:


    
	// Getters:
    // Management:
	bool SphFluid3dGpu::GetIsRunning() const
	{
		return m_isRunning;
	}
	bool SphFluid3dGpu::GetUseGridOptimization() const
	{
		return m_settings.useHashGridOptimization;
	}
	RotatedBounds SphFluid3dGpu::GetFluidBounds() const
	{
		return m_settings.fluidBounds;
	}
    // Time:
	float SphFluid3dGpu::GetTimeScale() const
	{
		return m_timeScale;
	}
	float SphFluid3dGpu::GetPhysicsTimeScale() const
	{
		return m_physicsTimeScale;
	}
	uint32_t SphFluid3dGpu::GetTimeStep() const
	{
		return m_timeStep;
	}
    // Fluid:
	int SphFluid3dGpu::GetParticleCount() const
	{
		return m_particleCount;
	}
	float SphFluid3dGpu::GetInitialDistributionRadius() const
	{
		return m_initialDistributionRadius;
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
	float SphFluid3dGpu::GetNearPressureRatio() const
	{
		return m_settings.nearPressureRatio;
	}
	float SphFluid3dGpu::GetMaxVelocity() const
	{
		return m_settings.maxVelocity;
	}
    // Forces:
	float SphFluid3dGpu::GetGravity() const
	{
		return m_settings.gravity;
	}
	float SphFluid3dGpu::GetAttractorRadius() const
	{
		return m_attractor.radius;
	}
	float SphFluid3dGpu::GetAttractorStrength() const
	{
		return m_attractor.strength;
	}
    int SphFluid3dGpu::GetAttractorState() const
    {
        return m_attractor.state;
    }
    Float3 SphFluid3dGpu::GetAttractorPoint() const
    {
        return m_attractor.point;
    }
	// Visuals:
	SphFluid3dGpu::RenderMode SphFluid3dGpu::GetRenderMode() const
	{
		return m_renderMode;
	}
    // Particles:
	int SphFluid3dGpu::GetColorMode() const
	{
		return m_colorMode;
	}
	float SphFluid3dGpu::GetVisualRadius() const
	{
		return m_visualRadius;
	}
    // Cloud:
	Uint3 SphFluid3dGpu::GetVolumetricDensityResolution() const
	{
		return m_volumetricDensityResolution;
	}
	float SphFluid3dGpu::GetVolumetricDensityRayStepLength() const
	{
        return m_volumetricDensityRayStepLength;
	}
    float SphFluid3dGpu::GetVolumetricDensityAbsorption() const
    {
        return m_volumetricDensityAbsorption;
    }
    Float3 SphFluid3dGpu::GetVolumetricScattering() const
    {
        return m_volumetricScattering;
    }
	bool SphFluid3dGpu::GetRenderVolumetricLight() const
	{
		return m_renderVolumetricLight;
	}
	Uint3 SphFluid3dGpu::GetVolumetricLightingResolution() const
	{
		return m_volumetricLightingResolution;
	}
	// Water:



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
		m_tripleData.Reallocate(m_particleCount, m_volumetricDensityResolution, m_volumetricLightingResolution);

		// Reset all buffer slots:
		Compute::RecordBarrierWaitStorageWriteBeforeReadWrite(m_computeShaders.computeType, m_computeShaders.sessionID);
		for (uint32_t i = 0; i < PhysicsTripleBufferState::bufferCount; i++)
		{
			SphFluid3dGpuSolver::ResetData(m_computeShaders, m_scratchData, m_tripleData, i, m_initialDistributionRadius);
			if (m_renderMode == RenderMode::cloud || m_renderMode == RenderMode::water)
				RecordVolumetricRenderData(m_computeShaders, i);
			else
				m_tripleData.hasOpticalDepthTexture3d[i] = false;
			Compute::RecordBarrierWaitStorageWriteBeforeReadWrite(m_computeShaders.computeType, m_computeShaders.sessionID);
		}
		Compute::RecordBarrierWaitStorageWriteBeforeRead(m_computeShaders.computeType, m_computeShaders.sessionID);

		m_pendingRenderRefresh = false;
		m_isRunning = false;
	}
	void SphFluid3dGpu::RefreshRenderData()
	{
		if (m_renderMode == RenderMode::cloud || m_renderMode == RenderMode::water)
		{
			// Finish particle writes and graphics reads before rebuilding the current read slot.
			Compute::Physics::WaitForFinish();
			m_tripleBufferState.PublishFinishedWrites();
			if (m_lastRenderFrameIndex != PhysicsTripleBufferState::invalidFrameIndex)
				Renderer::WaitForFrameFinished(m_lastRenderFrameIndex);

			SphFluid3dGpuSolver::ComputeShaders computeShaders = m_computeShaders;
			computeShaders.computeType = ComputeType::async;
			computeShaders.sessionID = Compute::Async::CreateComputeSession();
			RecordVolumetricRenderData(computeShaders, m_tripleBufferState.GetReadIndex());
			Compute::Async::DispatchComputeSessionAndWait(computeShaders.sessionID);
		}
		m_pendingRenderRefresh = false;
	}
	void SphFluid3dGpu::RecordVolumetricRenderData(SphFluid3dGpuSolver::ComputeShaders& computeShaders, uint32_t dataIndex)
	{
		Compute::RecordBarrierWaitStorageWriteBeforeReadWrite(computeShaders.computeType, computeShaders.sessionID);
		m_tripleData.fluidBounds[dataIndex] = m_settings.fluidBounds;
		m_tripleData.extinctionCoefficients[dataIndex] = Float3(m_volumetricDensityAbsorption) + m_volumetricScattering;
		SphFluid3dGpuSolver::ComputeDensityTexture3d(computeShaders, m_scratchData, m_tripleData, dataIndex);
		RotatedBounds lightBounds;
		if (m_renderVolumetricLight && TryGetDirectionalLightBounds(lightBounds))
		{
			m_tripleData.opticalDepthBounds[dataIndex] = lightBounds;
			m_tripleData.hasOpticalDepthTexture3d[dataIndex] = true;
			Compute::RecordBarrierWaitStorageWriteBeforeSampleReadStorageWrite(computeShaders.computeType, computeShaders.sessionID);
			SphFluid3dGpuSolver::ComputeOpticalDepthTexture3d(computeShaders, m_tripleData, dataIndex);
		}
		else
			m_tripleData.hasOpticalDepthTexture3d[dataIndex] = false;
	}
	bool SphFluid3dGpu::TryGetDirectionalLightBounds(RotatedBounds& lightBounds)
	{
		emberCommon::DirectionalLight directionalLight;
		if (!Renderer::TryGetDirectionalLight(directionalLight, 0))
			return false;

		Float3 lightDirection = Float3(GetTransform()->GetWorldToLocalMatrix() * Float4(directionalLight.direction, 0.0f)).Normalize();
		Float3x3 lightBoundsRotation = Float3x3::RotateFromTo(Float3::up, lightDirection);
		Float3x3 inverseLightBoundsRotation = lightBoundsRotation.Inverse();
		Float3 boundsCenter = m_settings.fluidBounds.localBounds.center;
		std::array<Float3, 8> corners = m_settings.fluidBounds.GetCorners();
		for (Float3& corner : corners)
			corner = boundsCenter + inverseLightBoundsRotation * (corner - boundsCenter);
		lightBounds = RotatedBounds(Bounds(corners.data()), lightBoundsRotation);
		return true;
	}
}