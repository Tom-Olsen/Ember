#include "sphFluid2dGpu.h"
#include "commonAccessMask.h"
#include "hashGrid2d.h"
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

		//// Settings0:
		//{
		//	// Management:
		//	m_isRunning = false;
		//	m_reset = true;
		//	m_timeScale = 4.0f;
		//	SetUseHashGridOptimization(true);
		//	m_timeStep = 0;
		//	SetParticleCount(2000);
		//
		//	// Physics:
		//	SetEffectRadius(0.5f);
		//	SetMass(1.0f);
		//	SetViscosity(0.5f);
		//	SetSurfaceTension(0.07f);
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
			SetUseHashGridOptimization(false);
			m_timeStep = 0;
			SetParticleCount(20000);

			// Physics:
			SetEffectRadius(0.2f);
			SetMass(1.0f);
			SetViscosity(1.0f);
			SetSurfaceTension(0.0f);
			SetCollisionDampening(0.95f);
			SetTargetDensity(40.0f);
			SetPressureMultiplier(10.0f);
			SetGravity(0.5f);
			SetMaxVelocity(5.0f);

			// User Interaction/Boundaries:
			SetAttractorRadius(3.0f);
			SetAttractorStrength(2.0f);
			SetFluidBounds(Bounds(Float3::zero, Float3(16.0f, 9.0f, 0.01f)));

			// Visuals:
			SetColorMode(0);
			SetInitialDistributionRadius(8.0f);
			SetVisualRadius(0.15f);
		}

		// Editor Window:
		editorWindow = std::make_unique<emberEditor::SphFluid2dGpuEditorWindow>(this);
		Reset();
	}
	SphFluid2dGpu::~SphFluid2dGpu()
	{

	}



	// Physics update:
	void SphFluid2dGpu::Reset()
	{
		m_timeStep = 0;
		m_data.Reallocate(m_particleCount, m_initialDistributionRadius);
		m_rungeKutta.Reallocate(m_particleCount);

		// Compute intial fluid state:
		Compute::RecordBarrierWaitStorageWriteBeforeRead(m_computeShaders.computeType);
		if (m_settings.useHashGridOptimization)
		{
			SphFluid2dGpuSolver::ComputeCellKeys(m_computeShaders, m_data.cellKeyBufferView, m_data.positionBufferView);
			Compute::RecordBarrierWaitStorageWriteBeforeRead(m_computeShaders.computeType);

			GpuSort<int>::SortPermutation(ComputeType::preRender, m_data.cellKeyBufferView, m_data.sortPermutationBufferView);
			Compute::RecordBarrierWaitStorageWriteBeforeRead(m_computeShaders.computeType);

			SphFluid2dGpuSolver::ComputeStartIndices(m_computeShaders, m_data.startIndexBufferView, m_data.cellKeyBufferView);
			GpuSort<Float2>::ApplyPermutation(ComputeType::preRender, m_data.sortPermutationBufferView, m_data.positionBufferView, m_data.tempBufferView0);
			Compute::RecordBarrierWaitStorageWriteBeforeRead(m_computeShaders.computeType);

			std::swap(m_data.positionBufferView, m_data.tempBufferView0);
		}
		SphFluid2dGpuSolver::ComputeDensities(m_computeShaders, m_data.densityBufferView, m_data.positionBufferView, m_data.startIndexBufferView, m_data.cellKeyBufferView);
		Compute::RecordBarrierWaitStorageWriteBeforeRead(m_computeShaders.computeType);
		SphFluid2dGpuSolver::ComputeNormalsAndCurvatures(m_computeShaders, m_data.normalBufferView, m_data.curvatureBufferView, m_data.densityBufferView, m_data.positionBufferView, m_data.startIndexBufferView, m_data.cellKeyBufferView);

		m_isRunning = false;
		m_reset = false;
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
			SphFluid2dGpuSolver::TimeStepRungeKutta2(deltaT, m_settings, m_data, m_computeShaders, m_attractor, m_rungeKutta);
			restTime -= deltaT;
		}
		m_timeStep++;
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
	void SphFluid2dGpu::SetUseHashGridOptimization(bool useGridOptimization)
	{
		if (m_settings.useHashGridOptimization != useGridOptimization)
		{
			m_settings.useHashGridOptimization = useGridOptimization;
			m_computeShaders.SetUseHashGridOptimization(m_settings.useHashGridOptimization);
		}
	}
	void SphFluid2dGpu::SetParticleCount(int particleCount)
	{
		particleCount = math::Max(1, particleCount);
		if (m_particleCount != particleCount)
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
		if (m_settings.effectRadius != effectRadius)
		{
			m_settings.effectRadius = effectRadius;
			m_computeShaders.SetEffectRadius(m_settings.effectRadius);
		}
	}
	void SphFluid2dGpu::SetMass(float mass)
	{
		mass = math::Max(1e-4f, mass);
		if (m_settings.mass != mass)
		{
			m_settings.mass = mass;
			m_computeShaders.SetMass(m_settings.mass);
		}
	}
	void SphFluid2dGpu::SetViscosity(float viscosity)
	{
		if (m_settings.viscosity != viscosity)
		{
			m_settings.viscosity = viscosity;
			m_computeShaders.SetViscosity(m_settings.viscosity);
		}
	}
	void SphFluid2dGpu::SetSurfaceTension(float surfaceTension)
	{
		if (m_settings.surfaceTension != surfaceTension)
		{
			m_settings.surfaceTension = surfaceTension;
			m_computeShaders.SetSurfaceTension(m_settings.surfaceTension);
		}
	}
	void SphFluid2dGpu::SetCollisionDampening(float collisionDampening)
	{
		if (m_settings.collisionDampening != collisionDampening)
		{
			m_settings.collisionDampening = collisionDampening;
			m_computeShaders.SetCollisionDampening(m_settings.collisionDampening);
		}
	}
	void SphFluid2dGpu::SetTargetDensity(float targetDensity)
	{
		targetDensity = math::Max(1e-4f, targetDensity);
		if (m_settings.targetDensity != targetDensity)
		{
			m_settings.targetDensity = targetDensity;
			m_computeShaders.SetTargetDensity(m_settings.targetDensity);
			m_shaderProperties.SetValue("Values", "targetDensity", m_settings.targetDensity);
		}
	}
	void SphFluid2dGpu::SetPressureMultiplier(float pressureMultiplier)
	{
		if (m_settings.pressureMultiplier != pressureMultiplier)
		{
			m_settings.pressureMultiplier = pressureMultiplier;
			m_computeShaders.SetPressureMultiplier(m_settings.pressureMultiplier);
		}
	}
	void SphFluid2dGpu::SetGravity(float gravity)
	{
		if (m_settings.gravity != gravity)
		{
			m_settings.gravity = gravity;
			m_computeShaders.SetGravity(m_settings.gravity);
		}
	}
	void SphFluid2dGpu::SetMaxVelocity(float maxVelocity)
	{
		maxVelocity = math::Max(1e-4f, maxVelocity);
		if (m_settings.maxVelocity != maxVelocity)
		{
			m_settings.maxVelocity = maxVelocity;
			m_computeShaders.SetMaxVelocity(m_settings.maxVelocity);
			m_shaderProperties.SetValue("Values", "maxVelocity", m_settings.maxVelocity);
		}
	}
	void SphFluid2dGpu::SetFluidBounds(const Bounds& bounds)
	{
		if (m_settings.fluidBounds != bounds)
		{
			m_settings.fluidBounds = bounds;
			m_computeShaders.SetFluidBounds(m_settings.fluidBounds);
		}
	}
	void SphFluid2dGpu::SetAttractorRadius(float attractorRadius)
	{
		attractorRadius = math::Max(1e-4f, attractorRadius);
		if (m_attractor.radius != attractorRadius)
		{
			m_attractor.radius = attractorRadius;
			m_ringMesh = MeshGenerator::ArcFlatUv(m_attractor.radius - 0.1f, m_attractor.radius + 0.1f, 360.0f, 100, "attractorRing");
			m_computeShaders.SetAttractorRadius(m_attractor.radius);
		}
	}
	void SphFluid2dGpu::SetAttractorStrength(float attractorStrength)
	{
		attractorStrength = math::Max(1e-4f, attractorStrength);
		if (m_attractor.strength != attractorStrength)
		{
			m_attractor.strength = attractorStrength;
			m_computeShaders.SetAttractorStrength(m_attractor.strength);
		}
	}
	void SphFluid2dGpu::SetAttractorState(int attractorState)
	{
		if (m_attractor.state != attractorState)
		{
			m_attractor.state = attractorState;
			m_computeShaders.SetAttractorState(m_attractor.state);
		}
	}
	void SphFluid2dGpu::SetAttractorPoint(const Float2& attractorPoint)
	{
		if (m_attractor.point != attractorPoint)
		{
			m_attractor.point = attractorPoint;
			m_computeShaders.SetAttractorPoint(m_attractor.point);
		}
	}
	void SphFluid2dGpu::SetColorMode(int colorMode)
	{
		colorMode = math::Clamp(colorMode, 0, 3);
		if (m_colorMode != colorMode)
		{
			m_colorMode = colorMode;
			m_shaderProperties.SetValue("Values", "colorMode", m_colorMode);
		}
	}
	void SphFluid2dGpu::SetInitialDistributionRadius(float initialDistributionRadius)
	{
		initialDistributionRadius = math::Max(1e-4f, initialDistributionRadius);
		if (m_initialDistributionRadius != initialDistributionRadius)
		{
			m_initialDistributionRadius = initialDistributionRadius;
			m_reset = true;
		}
	}
	void SphFluid2dGpu::SetVisualRadius(float visualRadius)
	{
		visualRadius = math::Max(0.01f, visualRadius);
		if (m_visualRadius != visualRadius)
		{
			m_visualRadius = visualRadius;
			m_particleMesh = MeshGenerator::UnitQuad().Scale(m_visualRadius);
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



	// Overrides:
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
		{
			Reset();
			return;
		}

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
				ShaderProperties shaderProperties = Renderer::DrawMesh(m_ringMesh, MaterialManager::GetMaterial("simpleUnlitMaterial"), hit.value(), Float3x3::identity, 1.0f, false, false);
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
		Renderer::DrawBounds(localToWorld, m_settings.fluidBounds, 0.01f, Float4::white, false, false);
		m_shaderProperties.SetBuffer("positionBuffer", m_data.positionBuffer);
		m_shaderProperties.SetBuffer("velocityBuffer", m_data.velocityBuffer);
		m_shaderProperties.SetBuffer("densityBuffer", m_data.densityBuffer);
		m_shaderProperties.SetBuffer("normalBuffer", m_data.normalBuffer);
		m_shaderProperties.SetBuffer("curvatureBuffer", m_data.curvatureBuffer);
		Renderer::DrawInstanced(m_particleCount, m_data.positionBuffer, m_particleMesh, m_particleMaterial, m_shaderProperties, localToWorld, false, false);
	}
}