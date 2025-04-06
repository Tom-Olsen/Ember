#include "sphFluid2dGpu.h"
#include "sphFluid2dGpuEditorWindow.h"



namespace emberEngine
{// Constructor/Destructor:
	SphFluid2dGpu::SphFluid2dGpu()
	{
		// Management:
		m_isRunning = false;
		m_timeScale = 2.0f;
		m_timeStep = 0;
		m_particleCount = 1500;
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
		m_visualRadius = 0.25f;
		m_pParticleMesh = MeshManager::GetMesh("unitQuad");
		m_pParticleMaterial = MaterialManager::GetMaterial("particleMaterial");

		editorWindow = std::make_unique<SphFluid2dGpuEditorWindow>(this);
	}
	SphFluid2dGpu::~SphFluid2dGpu()
	{

	}



	// Physics update:
	void SphFluid2dGpu::Reset()
	{
		m_timeStep = 0;
		// ...
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
			// ...
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
	void SphFluid2dGpu::SetUseGridOptimization(bool useGridOptimization)
	{
		m_useGridOptimization = useGridOptimization;
	}
	void SphFluid2dGpu::SetParticleCount(uint32_t particleCount)
	{
		if (m_particleCount != particleCount)
		{
			m_particleCount = particleCount;
			Reset();
		}
	}
	void SphFluid2dGpu::SetFluidBounds(const Bounds& bounds)
	{
		m_fluidBounds = bounds;
	}
	void SphFluid2dGpu::SetEffectRadius(float effectRadius)
	{
		m_effectRadius = effectRadius;
	}
	void SphFluid2dGpu::SetVisualRadius(float visualRadius)
	{
		m_visualRadius = visualRadius;
	}
	void SphFluid2dGpu::SetMass(float mass)
	{
		m_mass = mass;
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
		m_collisionDampening = collisionDampening;
	}
	void SphFluid2dGpu::SetTargetDensity(float targetDensity)
	{
		m_targetDensity = targetDensity;
	}
	void SphFluid2dGpu::SetPressureMultiplier(float pressureMultiplier)
	{
		m_pressureMultiplier = pressureMultiplier;
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
	uint32_t SphFluid2dGpu::GetParticleCount() const
	{
		return m_particleCount;
	}
	Bounds SphFluid2dGpu::GetFluidBounds() const
	{
		return m_fluidBounds;
	}
	float SphFluid2dGpu::GetEffectRadius() const
	{
		return m_effectRadius;
	}
	float SphFluid2dGpu::GetVisualRadius() const
	{
		return m_visualRadius;
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


	// Overrides:
	void SphFluid2dGpu::Start()
	{
		Reset();
	}
	void SphFluid2dGpu::Update()
	{
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

		// Mouse interaction:
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
	}
	const std::string SphFluid2dGpu::ToString() const
	{
		return "SphFluid2dGpu";
	}



	// Private methods:
	// Physics:
	void SphFluid2dGpu::Density()
	{

	}
	void SphFluid2dGpu::Normal()
	{

	}
	void SphFluid2dGpu::Curvature()
	{

	}
	void SphFluid2dGpu::PressureForceDensity()
	{

	}
	void SphFluid2dGpu::ViscosityForceDensity()
	{

	}
	void SphFluid2dGpu::SurfaceTensionForceDensity()
	{

	}
	void SphFluid2dGpu::GravityForceDensity()
	{

	}
	void SphFluid2dGpu::ExternalForceDensity()
	{

	}
	void SphFluid2dGpu::BoundaryCollisions()
	{

	}
}