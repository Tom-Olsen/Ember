#include "sphFluid2dCpu.h"
#include "smoothingKernals.h"
#include "sphFluid2dCpuEditorWindow.h"



namespace fluidDynamics
{
	// Public methods:
	// Constructor/Destructor:
	SphFluid2dCpu::SphFluid2dCpu()
	{
		// Management:
		m_isRunning = false;
		m_reset = true;
		m_timeScale = 2.0f;
		m_useHashGridOptimization = true;
		m_timeStep = 0;
		m_particleCount = 400;

		// Settings:
		m_hashGrid.Reallocate(m_particleCount);
		m_settings.pHashGrid = (m_useHashGridOptimization ? & m_hashGrid : nullptr);
		m_settings.effectRadius = 0.5f;
		m_settings.mass = 1.0f;
		m_settings.viscosity = 0.5f;
		m_settings.surfaceTension = 0.07f;
		m_settings.collisionDampening = 0.95f;
		m_settings.targetDensity = 15.0f;
		m_settings.pressureMultiplier = 20.0f;
		m_settings.gravity = 0.5f;
		m_settings.maxVelocity = 5.0f;
		m_settings.fluidBounds = Bounds(Float3::zero, Float3(16.0f, 9.0f, 0.01f));

		// Attractor:
		m_attractor.point = Float2::zero;
		SetAttractorRadius(3.0f);	// also sets m_ringMesh.
		m_attractor.strength = 2.0f;
		m_attractor.state = 0;

		// Visuals:
		m_colorMode = 1;
		m_initialDistributionRadius = 6.0f;
		SetVisualRadius(0.25f);		// also sets m_particleMesh.
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
		m_data.Reallocate(m_particleCount);
		m_rungeKutta.Reallocate(m_particleCount);

		// Initial particle positions:
		float phi = math::pi * (math::Sqrt(5.0f) - 1.0f);
		for (int i = 0; i < m_particleCount; i++)
		{
			float r = i / (m_particleCount - 1.0f) * m_initialDistributionRadius;
			float theta = phi * i;
			m_data.positions[i].x = math::Cos(theta) * r;
			m_data.positions[i].y = math::Sin(theta) * r;
		}

		// Reset fluid to initial data:
		if (m_useHashGridOptimization)
		{
			m_hashGrid.Reallocate(m_particleCount);
			m_hashGrid.UpdateGrid(m_data.positions, m_settings.effectRadius);
			m_hashGrid.ApplySort(m_data.positions);
			m_hashGrid.ApplySort(m_data.velocities);
		}
		SphFluid2dCpuSolver::ComputeDensities(m_settings, m_data.densities, m_data.positions);
		SphFluid2dCpuSolver::ComputeNormals(m_settings, m_data.normals, m_data.positions, m_data.densities);
		SphFluid2dCpuSolver::ComputeCurvatures(m_settings, m_data.curvatures, m_data.positions, m_data.densities, m_data.normals);

		m_isRunning = false;
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
			SphFluid2dCpuSolver::TimeStepRungeKutta2(deltaT, m_data, m_settings, m_attractor, m_rungeKutta);
			restTime -= deltaT;
		}
		m_timeStep++;
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
	void SphFluid2dCpu::SetUseHashGridOptimization(bool useGridOptimization)
	{
		m_useHashGridOptimization = useGridOptimization;
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
		m_settings.effectRadius = effectRadius;
	}
	void SphFluid2dCpu::SetMass(float mass)
	{
		m_settings.mass = mass;
	}
	void SphFluid2dCpu::SetViscosity(float viscosity)
	{
		m_settings.viscosity = viscosity;
	}
	void SphFluid2dCpu::SetSurfaceTension(float surfaceTension)
	{
		m_settings.surfaceTension = surfaceTension;
	}
	void SphFluid2dCpu::SetCollisionDampening(float collisionDampening)
	{
		m_settings.collisionDampening = collisionDampening;
	}
	void SphFluid2dCpu::SetTargetDensity(float targetDensity)
	{
		m_settings.targetDensity = targetDensity;
	}
	void SphFluid2dCpu::SetPressureMultiplier(float pressureMultiplier)
	{
		m_settings.pressureMultiplier = pressureMultiplier;
	}
	void SphFluid2dCpu::SetGravity(float gravity)
	{
		m_settings.gravity = gravity;
	}
	void SphFluid2dCpu::SetMaxVelocity(float maxVelocity)
	{
		m_settings.maxVelocity = maxVelocity;
	}
	void SphFluid2dCpu::SetAttractorRadius(float attractorRadius)
	{
		attractorRadius = math::Max(0.01f, attractorRadius);
		if (m_attractor.radius != attractorRadius)
		{
			m_ringMesh = MeshGenerator::ArcFlatUv(attractorRadius - 0.1f, attractorRadius + 0.1f, 360.0f, 100, "attractorRing");
			m_attractor.radius = attractorRadius;
		}
	}
	void SphFluid2dCpu::SetAttractorStrength(float attractorStrength)
	{
		m_attractor.strength = math::Max(0.01f, attractorStrength);
	}
	void SphFluid2dCpu::SetFluidBounds(const Bounds& bounds)
	{
		m_settings.fluidBounds = bounds;
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
	bool SphFluid2dCpu::GetUseHashGridOptimization() const
	{
		return m_useHashGridOptimization;
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
		return m_settings.effectRadius;
	}
	float SphFluid2dCpu::GetMass() const
	{
		return m_settings.mass;
	}
	float SphFluid2dCpu::GetViscosity() const
	{
		return m_settings.viscosity;
	}
	float SphFluid2dCpu::GetSurfaceTension() const
	{
		return m_settings.surfaceTension;
	}
	float SphFluid2dCpu::GetCollisionDampening() const
	{
		return m_settings.collisionDampening;
	}
	float SphFluid2dCpu::GetTargetDensity() const
	{
		return m_settings.targetDensity;
	}
	float SphFluid2dCpu::GetPressureMultiplier() const
	{
		return m_settings.pressureMultiplier;
	}
	float SphFluid2dCpu::GetGravity() const
	{
		return m_settings.gravity;
	}
	float SphFluid2dCpu::GetMaxVelocity() const
	{
		return m_settings.maxVelocity;
	}
	float SphFluid2dCpu::GetAttractorRadius() const
	{
		return m_attractor.radius;
	}
	float SphFluid2dCpu::GetAttractorStrength() const
	{
		return m_attractor.strength;
	}
	Bounds SphFluid2dCpu::GetFluidBounds() const
	{
		return m_settings.fluidBounds;
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
				SetAttractorStrength(zoomFactor * m_attractor.strength);
			}
			else
			{
				float zoomFactor = 1.0f + 0.1f * mouseScroll;
				SetAttractorRadius(zoomFactor * m_attractor.radius);
			}
		}

		// Mouse left and right click:
		if (EventSystem::MouseHeld(Input::MouseButton::Left) ^ EventSystem::MouseHeld(Input::MouseButton::Right)) // exlusive or.
		{
			Ray ray = Scene::GetActiveScene()->GetActiveCamera()->GetClickRay();
			std::optional<Float3> hit = m_settings.fluidBounds.IntersectRay(ray);
			if (hit.has_value())
			{
				m_attractor.point = Float2(hit.value());
				ShaderProperties shaderProperties = Renderer::DrawMesh(m_ringMesh, MaterialManager::GetMaterial("simpleUnlitMaterial"), hit.value(), Float3x3::identity, 1.0f, false, false);
				shaderProperties.SetValue("SurfaceProperties", "diffuseColor", Float4::red);
				if (EventSystem::MouseHeld(Input::MouseButton::Left))
					m_attractor.state = 1;
				if (EventSystem::MouseHeld(Input::MouseButton::Right))
					m_attractor.state = -1;
			}
			else
				m_attractor.state = 0;
		}
		else
			m_attractor.state = 0;


		// Rendering:
		Float4x4 localToWorld = GetTransform()->GetLocalToWorldMatrix();
		Renderer::DrawBounds(localToWorld, m_settings.fluidBounds, 0.01f, Float4::white, false, false);
		for (int i = 0; i < m_particleCount; i++)
		{
			Float4x4 matrix = localToWorld * Float4x4::Translate(Float3(m_data.positions[i]));
			ShaderProperties shaderProperties = Renderer::DrawMesh(m_particleMesh, m_particleMaterial, matrix, false, false);
		
			// Color by density:
			if (m_colorMode == 0)
			{
				float t = (m_data.densities[i] - m_settings.targetDensity) / m_settings.targetDensity;
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
				float t = m_data.velocities[i].Length() / m_settings.maxVelocity;
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
				Float4 color = Float4(m_data.normals[i].x, m_data.normals[i].y, 0, 1);
				shaderProperties.SetValue("SurfaceProperties", "diffuseColor", color);
			}
			// Color by curvature:
			else if (m_colorMode == 3)
			{
        		Float4 color = (0.5f + m_data.curvatures[i]) * Float4(0, 0, 1, 1);
				shaderProperties.SetValue("SurfaceProperties", "diffuseColor", color);
			}
		}
	}
}