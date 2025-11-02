#include "sphFluid3d.h"
#include "sphFluid3dEditorWindow.h"
#include "commonAccessMask.h"
#include "sphBitonicSort3d.h"
#include "vmaBuffer.h"



namespace fluidDynamics
{
	// Constructor/Destructor:
	SphFluid3d::SphFluid3d()
	{
		// Load compute shaders:
		std::string directoryPath = (std::string)PROJECT_ROOT_PATH + "/bin/shaders";

		cs_reset = ComputeShader("reset3d", directoryPath + "/reset3d.comp.spv");
		cs_density = ComputeShader("density3d", directoryPath + "/density3d.comp.spv");
		cs_normalAndCurvature = ComputeShader("normalAndCurvature3d", directoryPath + "/normalAndCurvature3d.comp.spv");
		cs_forceDensity = ComputeShader("forceDensity3d", directoryPath + "/forceDensity3d.comp.spv");
		cs_rungeKutta2Step1 = ComputeShader("rungeKutta2Step1_3d", directoryPath + "/rungeKutta2Step1_3d.comp.spv");
		cs_rungeKutta2Step2 = ComputeShader("rungeKutta2Step2_3d", directoryPath + "/rungeKutta2Step2_3d.comp.spv");
		cs_boundaryCollisions = ComputeShader("boundaryCollisions3d", directoryPath + "/boundaryCollisions3d.comp.spv");

		// Initialize shader properties:
		m_resetProperties = ShaderProperties(cs_reset);
		for (int i = 0; i < 2; i++)
		{
			m_densityProperties[i] = ShaderProperties(cs_density);
			m_normalAndCurvatureProperties[i] = ShaderProperties(cs_normalAndCurvature);
			m_forceDensityProperties[i] = ShaderProperties(cs_forceDensity);
		}
		m_rungeKutta2Step1Properties = ShaderProperties(cs_rungeKutta2Step1);
		m_rungeKutta2Step2Properties = ShaderProperties(cs_rungeKutta2Step2);
		m_boundaryCollisionsProperties = ShaderProperties(cs_boundaryCollisions);

		// Material setup:
		m_particleMaterial = MaterialManager::GetMaterial("particleMaterial3d");
		m_shaderProperties = ShaderProperties(m_particleMaterial);

		// Settings:
		{
			// Management:
			m_isRunning = false;
			m_reset = true;
			m_timeScale = 4.0f;
			SetUseGridOptimization(true);
			m_timeStep = 0;
			pGpuSort = std::make_unique<SphBitonicSort3d>();
			SetParticleCount(5000);

			// Physics:
			SetEffectRadius(0.6f);
			SetMass(1.0f);
			SetViscosity(0.5f);
			SetSurfaceTension(0.0f);
			SetCollisionDampening(0.95f);
			SetTargetDensity(7.0f);
			SetPressureMultiplier(6.0f);
			SetGravity(0.5f);
			SetMaxVelocity(5.0f);

			// User Interaction/Boundaries:
			SetFluidBounds(Bounds(Float3::zero, Float3(16.0f, 9.0f, 9.0f)));

			// Visuals:
			SetColorMode(0);
			SetInitialDistributionShellCount(8);
			SetInitialDistributionRadius(6.0f);
			SetVisualRadius(0.15f);
		}

		// Editor window:
		editorWindow = std::make_unique<emberEditor::SphFluid3dEditorWindow>(this);
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
		if (m_positionBuffer.IsValid() == false || m_positionBuffer.GetCount() != m_particleCount)
		{
			m_cellKeyBuffer = Buffer((uint32_t)m_particleCount, (uint32_t)sizeof(int), "cellKeyBuffer", emberCommon::BufferUsage::storage);
			m_startIndexBuffer = Buffer((uint32_t)m_particleCount, (uint32_t)sizeof(int), "startIndexBuffer", emberCommon::BufferUsage::storage);
			m_positionBuffer = Buffer((uint32_t)m_particleCount, (uint32_t)sizeof(Float3), "positionBuffer", emberCommon::BufferUsage::storage);
			m_velocityBuffer = Buffer((uint32_t)m_particleCount, (uint32_t)sizeof(Float3), "velocityBuffer", emberCommon::BufferUsage::storage);
			m_densityBuffer = Buffer((uint32_t)m_particleCount, (uint32_t)sizeof(float), "densityBuffer", emberCommon::BufferUsage::storage);
			m_normalBuffer = Buffer((uint32_t)m_particleCount, (uint32_t)sizeof(Float3), "normalBuffer", emberCommon::BufferUsage::storage);
			m_curvatureBuffer = Buffer((uint32_t)m_particleCount, (uint32_t)sizeof(float), "curvatureBuffer", emberCommon::BufferUsage::storage);
			m_forceDensityBuffer = Buffer((uint32_t)m_particleCount, (uint32_t)sizeof(Float3), "forceDensityBuffer", emberCommon::BufferUsage::storage);
			m_kp1Buffer = Buffer((uint32_t)m_particleCount, (uint32_t)sizeof(Float3), "kp1Buffer", emberCommon::BufferUsage::storage);
			m_kv1Buffer = Buffer((uint32_t)m_particleCount, (uint32_t)sizeof(Float3), "kv1Buffer", emberCommon::BufferUsage::storage);
			m_kp2Buffer = Buffer((uint32_t)m_particleCount, (uint32_t)sizeof(Float3), "kp2Buffer", emberCommon::BufferUsage::storage);
			m_kv2Buffer = Buffer((uint32_t)m_particleCount, (uint32_t)sizeof(Float3), "kv2Buffer", emberCommon::BufferUsage::storage);
			m_tempPositionBuffer = Buffer((uint32_t)m_particleCount, (uint32_t)sizeof(Float3), "tempPositionBuffer", emberCommon::BufferUsage::storage);
			m_tempVelocityBuffer = Buffer((uint32_t)m_particleCount, (uint32_t)sizeof(Float3), "tempVelocityBuffer", emberCommon::BufferUsage::storage);
		}

		// Reset fluid to initial data:
		ResetFluid();
		pGpuSort->ComputeCellKeys(m_cellKeyBuffer, m_positionBuffer, m_effectRadius);
		pGpuSort->Sort(m_cellKeyBuffer, m_positionBuffer, m_velocityBuffer);
		pGpuSort->ComputeStartIndices(m_cellKeyBuffer, m_startIndexBuffer);
		ComputeDensity(m_positionBuffer, m_densityProperties[0], m_effectRadius);
		ComputeNormalAndCurvature(m_positionBuffer, m_normalAndCurvatureProperties[0], m_effectRadius);
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
	void SphFluid3d::TimeStepRungeKutta2(float deltaT)
	{
		static int frame = 0;
		m_rungeKutta2Step1Properties.SetValue("Values", "deltaT", deltaT);
		m_rungeKutta2Step2Properties.SetValue("Values", "deltaT", deltaT);

		// Update hash grid for fast nearest neighbor particle look up:
		float q = 3.0f / 4.0f;	// Runge-Kutta-2 first timestep fraction.
		float gridRadius = 2.0f * m_effectRadius + q * m_maxVelocity * deltaT;
		pGpuSort->ComputeCellKeys(m_cellKeyBuffer, m_positionBuffer, gridRadius);
		pGpuSort->Sort(m_cellKeyBuffer, m_positionBuffer, m_velocityBuffer);
		pGpuSort->ComputeStartIndices(m_cellKeyBuffer, m_startIndexBuffer);

		// First Runte-Kutta step:
		ComputeDensity(m_positionBuffer, m_densityProperties[0], gridRadius);
		ComputeNormalAndCurvature(m_positionBuffer, m_normalAndCurvatureProperties[0], gridRadius);
		ComputeForceDensity(m_positionBuffer, m_velocityBuffer, m_forceDensityProperties[0], gridRadius);
		ComputeRungeKutta2Step1();

		// Second Runge-Kutta step:
		ComputeDensity(m_tempPositionBuffer, m_densityProperties[1], gridRadius);
		ComputeNormalAndCurvature(m_tempPositionBuffer, m_normalAndCurvatureProperties[1], gridRadius);
		ComputeForceDensity(m_tempPositionBuffer, m_tempVelocityBuffer, m_forceDensityProperties[1], gridRadius);
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
			m_densityProperties[0].SetValue("Values", "useGridOptimization", (int)m_useGridOptimization);
			m_densityProperties[1].SetValue("Values", "useGridOptimization", (int)m_useGridOptimization);
			m_normalAndCurvatureProperties[0].SetValue("Values", "useGridOptimization", (int)m_useGridOptimization);
			m_normalAndCurvatureProperties[1].SetValue("Values", "useGridOptimization", (int)m_useGridOptimization);
			m_forceDensityProperties[0].SetValue("Values", "useGridOptimization", (int)m_useGridOptimization);
			m_forceDensityProperties[1].SetValue("Values", "useGridOptimization", (int)m_useGridOptimization);
		}
	}
	void SphFluid3d::SetParticleCount(int particleCount)
	{
		particleCount = math::Max(1, particleCount);
		if (m_particleCount != particleCount)
		{
			m_particleCount = particleCount;
			m_threadCount = Uint3(m_particleCount, 1, 1);
			// TODO: remove particle count from compute shaders and use threadCount.x instead.
			m_densityProperties[0].SetValue("Values", "particleCount", m_particleCount);
			m_densityProperties[1].SetValue("Values", "particleCount", m_particleCount);
			m_normalAndCurvatureProperties[0].SetValue("Values", "particleCount", m_particleCount);
			m_normalAndCurvatureProperties[1].SetValue("Values", "particleCount", m_particleCount);
			m_forceDensityProperties[0].SetValue("Values", "particleCount", m_particleCount);
			m_forceDensityProperties[1].SetValue("Values", "particleCount", m_particleCount);
			m_reset = true;
		}
	}
	void SphFluid3d::SetEffectRadius(float effectRadius)
	{
		effectRadius = math::Max(1e-4f, effectRadius);
		if (m_effectRadius != effectRadius)
		{
			m_effectRadius = effectRadius;
			m_densityProperties[0].SetValue("Values", "effectRadius", m_effectRadius);
			m_densityProperties[1].SetValue("Values", "effectRadius", m_effectRadius);
			m_normalAndCurvatureProperties[0].SetValue("Values", "effectRadius", m_effectRadius);
			m_normalAndCurvatureProperties[1].SetValue("Values", "effectRadius", m_effectRadius);
			m_forceDensityProperties[0].SetValue("Values", "effectRadius", m_effectRadius);
			m_forceDensityProperties[1].SetValue("Values", "effectRadius", m_effectRadius);
		}
	}
	void SphFluid3d::SetMass(float mass)
	{
		mass = math::Max(1e-4f, mass);
		if (m_mass != mass)
		{
			m_mass = mass;
			m_densityProperties[0].SetValue("Values", "mass", m_mass);
			m_densityProperties[1].SetValue("Values", "mass", m_mass);
			m_normalAndCurvatureProperties[0].SetValue("Values", "mass", m_mass);
			m_normalAndCurvatureProperties[1].SetValue("Values", "mass", m_mass);
			m_forceDensityProperties[0].SetValue("Values", "mass", m_mass);
			m_forceDensityProperties[1].SetValue("Values", "mass", m_mass);
		}
	}
	void SphFluid3d::SetViscosity(float viscosity)
	{
		if (m_viscosity != viscosity)
		{
			m_viscosity = viscosity;
			m_forceDensityProperties[0].SetValue("Values", "viscosity", m_viscosity);
			m_forceDensityProperties[1].SetValue("Values", "viscosity", m_viscosity);
		}
	}
	void SphFluid3d::SetSurfaceTension(float surfaceTension)
	{
		if (m_surfaceTension != surfaceTension)
		{
			m_surfaceTension = surfaceTension;
			m_forceDensityProperties[0].SetValue("Values", "surfaceTension", m_surfaceTension);
			m_forceDensityProperties[1].SetValue("Values", "surfaceTension", m_surfaceTension);
		}
	}
	void SphFluid3d::SetCollisionDampening(float collisionDampening)
	{
		if (m_collisionDampening != collisionDampening)
		{
			m_collisionDampening = collisionDampening;
			m_boundaryCollisionsProperties.SetValue("Values", "collisionDampening", m_collisionDampening);
		}
	}
	void SphFluid3d::SetTargetDensity(float targetDensity)
	{
		targetDensity = math::Max(1e-4f, targetDensity);
		if (m_targetDensity != targetDensity)
		{
			m_targetDensity = targetDensity;
			m_shaderProperties.SetValue("Values", "targetDensity", m_targetDensity);
			m_resetProperties.SetValue("Values", "targetDensity", m_targetDensity);
			m_forceDensityProperties[0].SetValue("Values", "targetDensity", m_targetDensity);
			m_forceDensityProperties[1].SetValue("Values", "targetDensity", m_targetDensity);
		}
	}
	void SphFluid3d::SetPressureMultiplier(float pressureMultiplier)
	{
		if (m_pressureMultiplier != pressureMultiplier)
		{
			m_pressureMultiplier = pressureMultiplier;
			m_forceDensityProperties[0].SetValue("Values", "pressureMultiplier", m_pressureMultiplier);
			m_forceDensityProperties[1].SetValue("Values", "pressureMultiplier", m_pressureMultiplier);
		}
	}
	void SphFluid3d::SetGravity(float gravity)
	{
		if (m_gravity != gravity)
		{
			m_gravity = gravity;
			m_forceDensityProperties[0].SetValue("Values", "gravity", m_gravity);
			m_forceDensityProperties[1].SetValue("Values", "gravity", m_gravity);
		}
	}
	void SphFluid3d::SetMaxVelocity(float maxVelocity)
	{
		maxVelocity = math::Max(1e-4f, maxVelocity);
		if (m_maxVelocity != maxVelocity)
		{
			m_maxVelocity = maxVelocity;
			m_rungeKutta2Step1Properties.SetValue("Values", "maxVelocity", m_maxVelocity);
			m_rungeKutta2Step2Properties.SetValue("Values", "maxVelocity", m_maxVelocity);
			m_shaderProperties.SetValue("Values", "maxVelocity", m_maxVelocity);
		}
	}
	void SphFluid3d::SetFluidBounds(const Bounds& bounds)
	{
		if (m_fluidBounds != bounds)
		{
			m_fluidBounds = bounds;
			m_boundaryCollisionsProperties.SetValue("Values", "min", m_fluidBounds.GetMin());
			m_boundaryCollisionsProperties.SetValue("Values", "max", m_fluidBounds.GetMax());
		}
	}
	void SphFluid3d::SetColorMode(int colorMode)
	{
		colorMode = math::Clamp(colorMode, 0, 3);
		if (m_colorMode != colorMode)
		{
			m_colorMode = colorMode;
			m_shaderProperties.SetValue("Values", "colorMode", m_colorMode);
		}
	}
	void SphFluid3d::SetInitialDistributionShellCount(int initialDistributionShellCount)
	{
		initialDistributionShellCount = math::Max(1, initialDistributionShellCount);
		if (m_initialDistributionShellCount != initialDistributionShellCount)
		{
			m_initialDistributionShellCount = initialDistributionShellCount;
			m_resetProperties.SetValue("Values", "initialDistributionShellCount", m_initialDistributionShellCount);
			m_reset = true;
		}
	}
	void SphFluid3d::SetInitialDistributionRadius(float initialDistributionRadius)
	{
		initialDistributionRadius = math::Max(1e-4f, initialDistributionRadius);
		if (m_initialDistributionRadius != initialDistributionRadius)
		{
			m_initialDistributionRadius = initialDistributionRadius;
			m_resetProperties.SetValue("Values", "initialDistributionRadius", m_initialDistributionRadius);
			m_reset = true;
		}
	}
	void SphFluid3d::SetVisualRadius(float visualRadius)
	{
		visualRadius = math::Max(0.01f, visualRadius);
		if (m_visualRadius != visualRadius)
		{
			m_visualRadius = visualRadius;
			m_particleMesh = MeshGenerator::CubeSphere(m_visualRadius, 2, "");
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
	int SphFluid3d::GetInitialDistributionShellCount() const
	{
		return m_initialDistributionShellCount;
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
		Renderer::DrawBounds(localToWorld, m_fluidBounds, 0.01f, Float4::white, false, false);
		
		m_shaderProperties.SetBuffer("positionBuffer", m_positionBuffer);
		m_shaderProperties.SetBuffer("velocityBuffer", m_velocityBuffer);
		m_shaderProperties.SetBuffer("densityBuffer", m_densityBuffer);
		m_shaderProperties.SetBuffer("normalBuffer", m_normalBuffer);
		m_shaderProperties.SetBuffer("curvatureBuffer", m_curvatureBuffer);
		Renderer::DrawInstanced(m_particleCount, m_positionBuffer, m_particleMesh, m_particleMaterial, m_shaderProperties, localToWorld, false, false);
		//static void DrawInstanced(uint32_t instanceCount, Buffer & instanceBuffer, Mesh & mesh, const Material & material, ShaderProperties & shaderProperties, const Float4x4 & localToWorldMatrix, bool receiveShadows = true, bool castShadows = true);
	}



	// Private methods:
	// Physics:
	void SphFluid3d::ResetFluid()
	{
		m_resetProperties.SetBuffer("positionBuffer", m_positionBuffer);
		m_resetProperties.SetBuffer("velocityBuffer", m_velocityBuffer);
		m_resetProperties.SetBuffer("densityBuffer", m_densityBuffer);
		m_resetProperties.SetBuffer("normalBuffer", m_normalBuffer);
		m_resetProperties.SetBuffer("curvatureBuffer", m_curvatureBuffer);
		m_resetProperties.SetBuffer("forceDensityBuffer", m_forceDensityBuffer);
		m_resetProperties.SetBuffer("kp1Buffer", m_kp1Buffer);
		m_resetProperties.SetBuffer("kv1Buffer", m_kv1Buffer);
		m_resetProperties.SetBuffer("kp2Buffer", m_kp2Buffer);
		m_resetProperties.SetBuffer("kv2Buffer", m_kv2Buffer);
		m_resetProperties.SetBuffer("tempPositionBuffer", m_tempPositionBuffer);
		m_resetProperties.SetBuffer("tempVelocityBuffer", m_tempVelocityBuffer);
		Compute::PreRender::RecordComputeShader(cs_reset, m_resetProperties, m_threadCount);
		Compute::PreRender::RecordBarrier(emberCommon::AccessMasks::computeShader_shaderWrite, emberCommon::AccessMasks::computeShader_shaderRead);
	}
	void SphFluid3d::ComputeDensity(Buffer& positionBuffer, ShaderProperties& shaderProperties, float gridRadius)
	{
		shaderProperties.SetBuffer("cellKeyBuffer", m_cellKeyBuffer);
		shaderProperties.SetBuffer("startIndexBuffer", m_startIndexBuffer);
		shaderProperties.SetBuffer("positionBuffer", positionBuffer);
		shaderProperties.SetBuffer("densityBuffer", m_densityBuffer);
		shaderProperties.SetValue("Values", "gridRadius", gridRadius);
		Compute::PreRender::RecordComputeShader(cs_density, shaderProperties, m_threadCount);
		Compute::PreRender::RecordBarrier(emberCommon::AccessMasks::computeShader_shaderWrite, emberCommon::AccessMasks::computeShader_shaderRead);
	}
	void SphFluid3d::ComputeNormalAndCurvature(Buffer& positionBuffer, ShaderProperties& shaderProperties, float gridRadius)
	{
		shaderProperties.SetBuffer("cellKeyBuffer", m_cellKeyBuffer);
		shaderProperties.SetBuffer("startIndexBuffer", m_startIndexBuffer);
		shaderProperties.SetBuffer("positionBuffer", positionBuffer);
		shaderProperties.SetBuffer("densityBuffer", m_densityBuffer);
		shaderProperties.SetBuffer("normalBuffer", m_normalBuffer);
		shaderProperties.SetBuffer("curvatureBuffer", m_curvatureBuffer);
		shaderProperties.SetValue("Values", "gridRadius", gridRadius);
		Compute::PreRender::RecordComputeShader(cs_normalAndCurvature, shaderProperties, m_threadCount);
		Compute::PreRender::RecordBarrier(emberCommon::AccessMasks::computeShader_shaderWrite, emberCommon::AccessMasks::computeShader_shaderRead);
	}
	void SphFluid3d::ComputeCurvature()
	{

	}
	void SphFluid3d::ComputeForceDensity(Buffer& positionBuffer, Buffer& velocityBuffer, ShaderProperties& shaderProperties, float gridRadius)
	{
		shaderProperties.SetBuffer("cellKeyBuffer", m_cellKeyBuffer);
		shaderProperties.SetBuffer("startIndexBuffer", m_startIndexBuffer);
		shaderProperties.SetBuffer("positionBuffer", positionBuffer);
		shaderProperties.SetBuffer("velocityBuffer", velocityBuffer);
		shaderProperties.SetBuffer("densityBuffer", m_densityBuffer);
		shaderProperties.SetBuffer("normalBuffer", m_normalBuffer);
		shaderProperties.SetBuffer("curvatureBuffer", m_curvatureBuffer);
		shaderProperties.SetBuffer("forceDensityBuffer", m_forceDensityBuffer);
		shaderProperties.SetValue("Values", "gridRadius", gridRadius);
		Compute::PreRender::RecordComputeShader(cs_forceDensity, shaderProperties, m_threadCount);
		Compute::PreRender::RecordBarrier(emberCommon::AccessMasks::computeShader_shaderWrite, emberCommon::AccessMasks::computeShader_shaderRead);
	}
	void SphFluid3d::ComputeRungeKutta2Step1()
	{
		m_rungeKutta2Step1Properties.SetBuffer("forceDensityBuffer", m_forceDensityBuffer);
		m_rungeKutta2Step1Properties.SetBuffer("densityBuffer", m_densityBuffer);
		m_rungeKutta2Step1Properties.SetBuffer("positionBuffer", m_positionBuffer);
		m_rungeKutta2Step1Properties.SetBuffer("velocityBuffer", m_velocityBuffer);
		m_rungeKutta2Step1Properties.SetBuffer("kp1Buffer", m_kp1Buffer);
		m_rungeKutta2Step1Properties.SetBuffer("kv1Buffer", m_kv1Buffer);
		m_rungeKutta2Step1Properties.SetBuffer("tempPositionBuffer", m_tempPositionBuffer);
		m_rungeKutta2Step1Properties.SetBuffer("tempVelocityBuffer", m_tempVelocityBuffer);
		Compute::PreRender::RecordComputeShader(cs_rungeKutta2Step1, m_rungeKutta2Step1Properties, m_threadCount);
		Compute::PreRender::RecordBarrier(emberCommon::AccessMasks::computeShader_shaderWrite, emberCommon::AccessMasks::computeShader_shaderRead);
	}
	void SphFluid3d::ComputeRungeKutta2Step2()
	{
		m_rungeKutta2Step2Properties.SetBuffer("forceDensityBuffer", m_forceDensityBuffer);
		m_rungeKutta2Step2Properties.SetBuffer("densityBuffer", m_densityBuffer);
		m_rungeKutta2Step2Properties.SetBuffer("kp1Buffer", m_kp1Buffer);
		m_rungeKutta2Step2Properties.SetBuffer("kv1Buffer", m_kv1Buffer);
		m_rungeKutta2Step2Properties.SetBuffer("tempPositionBuffer", m_tempPositionBuffer);
		m_rungeKutta2Step2Properties.SetBuffer("tempVelocityBuffer", m_tempVelocityBuffer);
		m_rungeKutta2Step2Properties.SetBuffer("kp2Buffer", m_kp2Buffer);
		m_rungeKutta2Step2Properties.SetBuffer("kv2Buffer", m_kv2Buffer);
		m_rungeKutta2Step2Properties.SetBuffer("positionBuffer", m_positionBuffer);
		m_rungeKutta2Step2Properties.SetBuffer("velocityBuffer", m_velocityBuffer);
		Compute::PreRender::RecordComputeShader(cs_rungeKutta2Step2, m_rungeKutta2Step2Properties, m_threadCount);
		Compute::PreRender::RecordBarrier(emberCommon::AccessMasks::computeShader_shaderWrite, emberCommon::AccessMasks::computeShader_shaderRead);
	}
	void SphFluid3d::ComputeBoundaryCollisions()
	{
		m_boundaryCollisionsProperties.SetBuffer("positionBuffer", m_positionBuffer);
		m_boundaryCollisionsProperties.SetBuffer("velocityBuffer", m_velocityBuffer);
		Compute::PreRender::RecordComputeShader(cs_boundaryCollisions, m_boundaryCollisionsProperties, m_threadCount);
		Compute::PreRender::RecordBarrier(emberCommon::AccessMasks::computeShader_shaderWrite, emberCommon::AccessMasks::computeShader_shaderRead);
	}
}