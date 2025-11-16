#include "sphFluid2dGpu.h"
#include "commonAccessMask.h"
#include "hashGrid2d.h"
#include "smoothingKernals.h"
#include "sphFluid2dGpuEditorWindow.h"



namespace fluidDynamics
{
	// Constructor/Destructor:
	SphFluid2dGpu::SphFluid2dGpu()
	{
		// Load compute shaders:
		std::filesystem::path directoryPath = (std::filesystem::path(PROJECT_SHADERS_DIR) / "bin").make_preferred();
		m_resetComputeShader = ComputeShader("reset2d", directoryPath / "reset2d.comp.spv");
		m_cellKeysComputeShader = ComputeShader("cellKeys2d", directoryPath / "cellKeys2d.comp.spv");
		m_startIndicesComputeShader = ComputeShader("startIndices2d", directoryPath / "startIndices2d.comp.spv");
		m_densityComputeShader = ComputeShader("density2d", directoryPath / "density2d.comp.spv");
		m_normalAndCurvatureComputeShader = ComputeShader("normalAndCurvature2d", directoryPath / "normalAndCurvature2d.comp.spv");
		m_forceDensityComputeShader = ComputeShader("forceDensity2d", directoryPath / "forceDensity2d.comp.spv");
		m_rungeKutta2Step1ComputeShader = ComputeShader("rungeKutta2Step1_2d", directoryPath / "rungeKutta2Step1_2d.comp.spv");
		m_rungeKutta2Step2ComputeShader = ComputeShader("rungeKutta2Step2_2d", directoryPath / "rungeKutta2Step2_2d.comp.spv");
		m_boundaryCollisionsComputeShader = ComputeShader("boundaryCollisions2d", directoryPath / "boundaryCollisions2d.comp.spv");
		
		// Initialize shader properties:
		m_resetProperties = ShaderProperties(m_resetComputeShader);
		m_cellKeysProperties = ShaderProperties(m_cellKeysComputeShader);
		m_startIndicesProperties = ShaderProperties(m_startIndicesComputeShader);
		for (int i = 0; i < 2; i++)
		{
			m_densityProperties[i] = ShaderProperties(m_densityComputeShader);
			m_normalAndCurvatureProperties[i] = ShaderProperties(m_normalAndCurvatureComputeShader);
			m_forceDensityProperties[i] = ShaderProperties(m_forceDensityComputeShader);
		}
		m_rungeKutta2Step1Properties = ShaderProperties(m_rungeKutta2Step1ComputeShader);
		m_rungeKutta2Step2Properties = ShaderProperties(m_rungeKutta2Step2ComputeShader);
		m_boundaryCollisionsProperties = ShaderProperties(m_boundaryCollisionsComputeShader);

		// Material setup:
		m_particleMaterial = MaterialManager::GetMaterial("particleMaterial2d");
		m_shaderProperties = ShaderProperties(m_particleMaterial);

		//// Settings0:
		//{
		//	// Management:
		//	m_isRunning = false;
		//	m_timeScale = 4.0f;
		//	SetUseGridOptimization(true);
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
			m_timeScale = 4.0f;
			SetUseGridOptimization(true);
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

		// Reallocate buffers:
		if (m_positionBuffer.IsValid() == false || m_positionBuffer.GetCount() != m_particleCount)
		{
			// Data buffers:
			m_cellKeyBuffer = Buffer((uint32_t)m_particleCount, (uint32_t)sizeof(int), "cellKeyBuffer", BufferUsage::storage);
			m_startIndexBuffer = Buffer((uint32_t)m_hashGridSize, (uint32_t)sizeof(int), "startIndexBuffer", BufferUsage::storage);
			m_positionBuffer = Buffer((uint32_t)m_particleCount, (uint32_t)sizeof(Float2), "positionBuffer", BufferUsage::storage);
			m_velocityBuffer = Buffer((uint32_t)m_particleCount, (uint32_t)sizeof(Float2), "velocityBuffer", BufferUsage::storage);
			m_densityBuffer = Buffer((uint32_t)m_particleCount, (uint32_t)sizeof(float), "densityBuffer", BufferUsage::storage);
			m_normalBuffer = Buffer((uint32_t)m_particleCount, (uint32_t)sizeof(Float2), "normalBuffer", BufferUsage::storage);
			m_curvatureBuffer = Buffer((uint32_t)m_particleCount, (uint32_t)sizeof(float), "curvatureBuffer", BufferUsage::storage);
			m_forceDensityBuffer = Buffer((uint32_t)m_particleCount, (uint32_t)sizeof(Float2), "forceDensityBuffer", BufferUsage::storage);
			// Runge Kutta buffers:
			m_kp1Buffer = Buffer((uint32_t)m_particleCount, (uint32_t)sizeof(Float2), "kp1Buffer", BufferUsage::storage);
			m_kv1Buffer = Buffer((uint32_t)m_particleCount, (uint32_t)sizeof(Float2), "kv1Buffer", BufferUsage::storage);
			m_kp2Buffer = Buffer((uint32_t)m_particleCount, (uint32_t)sizeof(Float2), "kp2Buffer", BufferUsage::storage);
			m_kv2Buffer = Buffer((uint32_t)m_particleCount, (uint32_t)sizeof(Float2), "kv2Buffer", BufferUsage::storage);
			m_tempPositionBuffer = Buffer((uint32_t)m_particleCount, (uint32_t)sizeof(Float2), "tempPositionBuffer", BufferUsage::storage);
			m_tempVelocityBuffer = Buffer((uint32_t)m_particleCount, (uint32_t)sizeof(Float2), "tempVelocityBuffer", BufferUsage::storage);
			// Temp buffers:
			m_sortPermutationBuffer = Buffer((uint32_t)m_particleCount, (uint32_t)sizeof(int), "sortPermutationBuffer", BufferUsage::storage);
			m_reorderBuffer0 = Buffer((uint32_t)m_particleCount, (uint32_t)sizeof(Float2), "reorderBuffer0", BufferUsage::storage);
			m_reorderBuffer1 = Buffer((uint32_t)m_particleCount, (uint32_t)sizeof(Float2), "reorderBuffer1", BufferUsage::storage);
			m_reorderBuffer2 = Buffer((uint32_t)m_particleCount, (uint32_t)sizeof(Float2), "reorderBuffer2", BufferUsage::storage);
			m_reorderBuffer3 = Buffer((uint32_t)m_particleCount, (uint32_t)sizeof(Float2), "reorderBuffer3", BufferUsage::storage);
			m_reorderBuffer4 = Buffer((uint32_t)m_particleCount, (uint32_t)sizeof(Float2), "reorderBuffer4", BufferUsage::storage);

			// Data buffer views:
			m_cellKeyBufferView = BufferView<int>(m_cellKeyBuffer);
			m_startIndexBufferView = BufferView<int>(m_startIndexBuffer);
			m_positionBufferView = BufferView<Float2>(m_positionBuffer);
			m_velocityBufferView = BufferView<Float2>(m_velocityBuffer);
			m_densityBufferView = BufferView<float>(m_densityBuffer);
			m_normalBufferView = BufferView<Float2>(m_normalBuffer);
			m_curvatureBufferView = BufferView<Float2>(m_curvatureBuffer);
			m_forceDensityBufferView = BufferView<Float2>(m_forceDensityBuffer);
			// Runge Kutta buffer views:
			m_kp1BufferView = BufferView<Float2>(m_kp1Buffer);
			m_kv1BufferView = BufferView<Float2>(m_kv1Buffer);
			m_kp2BufferView = BufferView<Float2>(m_kp2Buffer);
			m_kv2BufferView = BufferView<Float2>(m_kv2Buffer);
			m_tempPositionBufferView = BufferView<Float2>(m_tempPositionBuffer);
			m_tempVelocityBufferView = BufferView<Float2>(m_tempVelocityBuffer);
			// Temp buffer views:
			m_sortPermutationBufferView = BufferView<int>(m_sortPermutationBuffer);
			m_reorderBufferView0 = BufferView<Float2>(m_reorderBuffer0);
			m_reorderBufferView1 = BufferView<Float2>(m_reorderBuffer1);
			m_reorderBufferView2 = BufferView<Float2>(m_reorderBuffer2);
			m_reorderBufferView3 = BufferView<Float2>(m_reorderBuffer3);
			m_reorderBufferView4 = BufferView<Float2>(m_reorderBuffer4);
		}

		// Reset fluid to initial data:
		ResetFluid();
		Compute::PreRender::RecordBarrierWaitStorageWriteBeforeRead();
		if (m_useGridOptimization)
		{
			ComputeCellKeys(m_positionBufferView);
			Compute::PreRender::RecordBarrierWaitStorageWriteBeforeRead();

			GpuSort<int>::SortPermutation(ComputeType::preRender, m_cellKeyBufferView, m_sortPermutationBufferView);
			Compute::PreRender::RecordBarrierWaitStorageWriteBeforeRead();

			ComputeStartIndices();
			GpuSort<Float2>::ApplyPermutation(ComputeType::preRender, m_sortPermutationBufferView, m_positionBufferView, m_reorderBufferView0);
			Compute::PreRender::RecordBarrierWaitStorageWriteBeforeRead();

			std::swap(m_positionBufferView, m_reorderBufferView0);
		}
		ComputeDensity(m_positionBufferView, m_densityProperties[0]);
		Compute::PreRender::RecordBarrierWaitStorageWriteBeforeRead();
		ComputeNormalAndCurvature(m_positionBufferView, m_normalAndCurvatureProperties[0]);

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
		m_rungeKutta2Step1Properties.SetValue("Values", "deltaT", deltaT);
		m_rungeKutta2Step2Properties.SetValue("Values", "deltaT", deltaT);

		// Update hash grid for fast nearest neighbor particle look up:
		if (m_useGridOptimization)
		{
			ComputeCellKeys(m_positionBufferView);
			Compute::PreRender::RecordBarrierWaitStorageWriteBeforeRead();

			GpuSort<int>::SortPermutation(ComputeType::preRender, m_cellKeyBufferView, m_sortPermutationBufferView);
			Compute::PreRender::RecordBarrierWaitStorageWriteBeforeRead();

			ComputeStartIndices();
			GpuSort<Float2>::ApplyPermutation(ComputeType::preRender, m_sortPermutationBufferView, m_positionBufferView, m_reorderBufferView0);
			GpuSort<Float2>::ApplyPermutation(ComputeType::preRender, m_sortPermutationBufferView, m_velocityBufferView, m_reorderBufferView1);
			Compute::PreRender::RecordBarrierWaitStorageWriteBeforeRead();

			std::swap(m_positionBufferView, m_reorderBufferView0);
			std::swap(m_velocityBufferView, m_reorderBufferView1);
		}

		// First Runte-Kutta step:
		ComputeDensity(m_positionBufferView, m_densityProperties[0]);
		Compute::PreRender::RecordBarrierWaitStorageWriteBeforeRead();
		ComputeNormalAndCurvature(m_positionBufferView, m_normalAndCurvatureProperties[0]);
		Compute::PreRender::RecordBarrierWaitStorageWriteBeforeRead();
		ComputeForceDensity(m_positionBufferView, m_velocityBufferView, m_forceDensityProperties[0]);
		Compute::PreRender::RecordBarrierWaitStorageWriteBeforeRead();
		ComputeRungeKutta2Step1();
		Compute::PreRender::RecordBarrierWaitStorageWriteBeforeRead();

		// Update hash grid for fast nearest neighbor particle look up:
		if (m_useGridOptimization)
		{
			ComputeCellKeys(m_positionBufferView);
			Compute::PreRender::RecordBarrierWaitStorageWriteBeforeRead();

			GpuSort<int>::SortPermutation(ComputeType::preRender, m_cellKeyBufferView, m_sortPermutationBufferView);
			Compute::PreRender::RecordBarrierWaitStorageWriteBeforeRead();

			ComputeStartIndices();
			GpuSort<Float2>::ApplyPermutation(ComputeType::preRender, m_sortPermutationBufferView,  m_tempPositionBufferView, m_reorderBufferView0);
			GpuSort<Float2>::ApplyPermutation(ComputeType::preRender, m_sortPermutationBufferView,  m_tempVelocityBufferView, m_reorderBufferView1);
			GpuSort<Float2>::ApplyPermutation(ComputeType::preRender, m_sortPermutationBufferView,  m_positionBufferView, m_reorderBufferView2);
			GpuSort<Float2>::ApplyPermutation(ComputeType::preRender, m_sortPermutationBufferView,  m_velocityBufferView, m_reorderBufferView3);
			GpuSort<Float2>::ApplyPermutation(ComputeType::preRender, m_sortPermutationBufferView,  m_kv1BufferView, m_reorderBufferView4);
			Compute::PreRender::RecordBarrierWaitStorageWriteBeforeRead();

			std::swap(m_tempPositionBufferView, m_reorderBufferView0);
			std::swap(m_tempVelocityBufferView, m_reorderBufferView1);
			std::swap(m_positionBufferView, m_reorderBufferView2);
			std::swap(m_velocityBufferView, m_reorderBufferView3);
			std::swap(m_kv1BufferView, m_reorderBufferView4);
		}

		// Second Runge-Kutta step:
		ComputeDensity(m_tempPositionBufferView, m_densityProperties[1]);
		Compute::PreRender::RecordBarrierWaitStorageWriteBeforeRead();
		ComputeNormalAndCurvature(m_tempPositionBufferView, m_normalAndCurvatureProperties[1]);
		Compute::PreRender::RecordBarrierWaitStorageWriteBeforeRead();
		ComputeForceDensity(m_tempPositionBufferView, m_tempVelocityBufferView, m_forceDensityProperties[1]);
		Compute::PreRender::RecordBarrierWaitStorageWriteBeforeRead();
		ComputeRungeKutta2Step2();
		Compute::PreRender::RecordBarrierWaitStorageWriteBeforeRead();

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
			m_densityProperties[0].SetValue("Values", "useGridOptimization", (int)m_useGridOptimization);
			m_densityProperties[1].SetValue("Values", "useGridOptimization", (int)m_useGridOptimization);
			m_normalAndCurvatureProperties[0].SetValue("Values", "useGridOptimization", (int)m_useGridOptimization);
			m_normalAndCurvatureProperties[1].SetValue("Values", "useGridOptimization", (int)m_useGridOptimization);
			m_forceDensityProperties[0].SetValue("Values", "useGridOptimization", (int)m_useGridOptimization);
			m_forceDensityProperties[1].SetValue("Values", "useGridOptimization", (int)m_useGridOptimization);
		}
	}
	void SphFluid2dGpu::SetParticleCount(int particleCount)
	{
		particleCount = math::Max(1, particleCount);
		if (m_particleCount != particleCount)
		{
			m_particleCount = particleCount;
			m_hashGridSize = math::NextPrimeAbove(2 * m_particleCount);
			m_threadCount = Uint3(m_particleCount, 1, 1);
			m_cellKeysProperties.SetValue("Values", "hashGridSize", m_hashGridSize);
			m_densityProperties[0].SetValue("Values", "particleCount", m_particleCount);
			m_densityProperties[1].SetValue("Values", "particleCount", m_particleCount);
			m_densityProperties[0].SetValue("Values", "hashGridSize", m_hashGridSize);
			m_densityProperties[1].SetValue("Values", "hashGridSize", m_hashGridSize);
			m_normalAndCurvatureProperties[0].SetValue("Values", "particleCount", m_particleCount);
			m_normalAndCurvatureProperties[1].SetValue("Values", "particleCount", m_particleCount);
			m_normalAndCurvatureProperties[0].SetValue("Values", "hashGridSize", m_hashGridSize);
			m_normalAndCurvatureProperties[1].SetValue("Values", "hashGridSize", m_hashGridSize);
			m_forceDensityProperties[0].SetValue("Values", "particleCount", m_particleCount);
			m_forceDensityProperties[1].SetValue("Values", "particleCount", m_particleCount);
			m_forceDensityProperties[0].SetValue("Values", "hashGridSize", m_hashGridSize);
			m_forceDensityProperties[1].SetValue("Values", "hashGridSize", m_hashGridSize);
			m_reset = true;
		}
	}
	void SphFluid2dGpu::SetEffectRadius(float effectRadius)
	{
		effectRadius = math::Max(1e-4f, effectRadius);
		if (m_effectRadius != effectRadius)
		{
			m_effectRadius = effectRadius;
			m_cellKeysProperties.SetValue("Values", "effectRadius", m_effectRadius);
			m_densityProperties[0].SetValue("Values", "effectRadius", m_effectRadius);
			m_densityProperties[1].SetValue("Values", "effectRadius", m_effectRadius);
			m_normalAndCurvatureProperties[0].SetValue("Values", "effectRadius", m_effectRadius);
			m_normalAndCurvatureProperties[1].SetValue("Values", "effectRadius", m_effectRadius);
			m_forceDensityProperties[0].SetValue("Values", "effectRadius", m_effectRadius);
			m_forceDensityProperties[1].SetValue("Values", "effectRadius", m_effectRadius);
		}
	}
	void SphFluid2dGpu::SetMass(float mass)
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
	void SphFluid2dGpu::SetViscosity(float viscosity)
	{
		if (m_viscosity != viscosity)
		{
			m_viscosity = viscosity;
			m_forceDensityProperties[0].SetValue("Values", "viscosity", m_viscosity);
			m_forceDensityProperties[1].SetValue("Values", "viscosity", m_viscosity);
		}
	}
	void SphFluid2dGpu::SetSurfaceTension(float surfaceTension)
	{
		if (m_surfaceTension != surfaceTension)
		{
			m_surfaceTension = surfaceTension;
			m_forceDensityProperties[0].SetValue("Values", "surfaceTension", m_surfaceTension);
			m_forceDensityProperties[1].SetValue("Values", "surfaceTension", m_surfaceTension);
		}
	}
	void SphFluid2dGpu::SetCollisionDampening(float collisionDampening)
	{
		if (m_collisionDampening != collisionDampening)
		{
			m_collisionDampening = collisionDampening;
			m_boundaryCollisionsProperties.SetValue("Values", "collisionDampening", m_collisionDampening);
		}
	}
	void SphFluid2dGpu::SetTargetDensity(float targetDensity)
	{
		targetDensity = math::Max(1e-4f, targetDensity);
		if (m_targetDensity != targetDensity)
		{
			m_targetDensity = targetDensity;
			m_shaderProperties.SetValue("Values", "targetDensity", m_targetDensity);
			m_forceDensityProperties[0].SetValue("Values", "targetDensity", m_targetDensity);
			m_forceDensityProperties[1].SetValue("Values", "targetDensity", m_targetDensity);
		}
	}
	void SphFluid2dGpu::SetPressureMultiplier(float pressureMultiplier)
	{
		if (m_pressureMultiplier != pressureMultiplier)
		{
			m_pressureMultiplier = pressureMultiplier;
			m_forceDensityProperties[0].SetValue("Values", "pressureMultiplier", m_pressureMultiplier);
			m_forceDensityProperties[1].SetValue("Values", "pressureMultiplier", m_pressureMultiplier);
		}
	}
	void SphFluid2dGpu::SetGravity(float gravity)
	{
		if (m_gravity != gravity)
		{
			m_gravity = gravity;
			m_forceDensityProperties[0].SetValue("Values", "gravity", m_gravity);
			m_forceDensityProperties[1].SetValue("Values", "gravity", m_gravity);
		}
	}
	void SphFluid2dGpu::SetMaxVelocity(float maxVelocity)
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
	void SphFluid2dGpu::SetAttractorRadius(float attractorRadius)
	{
		attractorRadius = math::Max(1e-4f, attractorRadius);
		if (m_attractorRadius != attractorRadius)
		{
			m_ringMesh = MeshGenerator::ArcFlatUv(attractorRadius - 0.1f, attractorRadius + 0.1f, 360.0f, 100, "attractorRing");
			m_attractorRadius = attractorRadius;
			m_forceDensityProperties[0].SetValue("Values", "attractorRadius", m_attractorRadius);
			m_forceDensityProperties[1].SetValue("Values", "attractorRadius", m_attractorRadius);
		}
	}
	void SphFluid2dGpu::SetAttractorStrength(float attractorStrength)
	{
		attractorStrength = math::Max(1e-4f, attractorStrength);
		if (m_attractorStrength != attractorStrength)
		{
			m_attractorStrength = attractorStrength;
			m_forceDensityProperties[0].SetValue("Values", "attractorStrength", m_attractorStrength);
			m_forceDensityProperties[1].SetValue("Values", "attractorStrength", m_attractorStrength);
		}
	}
	void SphFluid2dGpu::SetAttractorState(int attractorState)
	{
		if (m_attractorState != attractorState)
		{
			m_attractorState = attractorState;
			m_forceDensityProperties[0].SetValue("Values", "attractorState", m_attractorState);
			m_forceDensityProperties[1].SetValue("Values", "attractorState", m_attractorState);
		}
	}
	void SphFluid2dGpu::SetAttractorPoint(const Float2& attractorPoint)
	{
		if (m_attractorPoint != attractorPoint)
		{
			m_attractorPoint = attractorPoint;
			m_forceDensityProperties[0].SetValue("Values", "attractorPoint", m_attractorPoint);
			m_forceDensityProperties[1].SetValue("Values", "attractorPoint", m_attractorPoint);
		}
	}
	void SphFluid2dGpu::SetFluidBounds(const Bounds& bounds)
	{
		if (m_fluidBounds != bounds)
		{
			m_fluidBounds = bounds;
			m_boundaryCollisionsProperties.SetValue("Values", "min", m_fluidBounds.GetMin());
			m_boundaryCollisionsProperties.SetValue("Values", "max", m_fluidBounds.GetMax());
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
			m_resetProperties.SetValue("Values", "initialDistributionRadius", m_initialDistributionRadius);
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
				SetAttractorStrength(zoomFactor * m_attractorStrength);
			}
			else
			{
				float zoomFactor = 1.0f + 0.1f * mouseScroll;
				SetAttractorRadius(zoomFactor * m_attractorRadius);
			}
		}

		// Mouse left and right click:
		if (EventSystem::MouseHeld(Input::MouseButton::Left) ^ EventSystem::MouseHeld(Input::MouseButton::Right)) // exlusive or
		{
			Ray ray = Scene::GetActiveScene()->GetActiveCamera()->GetClickRay();
			std::optional<Float3> hit = m_fluidBounds.IntersectRay(ray);
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
		Renderer::DrawBounds(localToWorld, m_fluidBounds, 0.01f, Float4::white, false, false);
		m_shaderProperties.SetBuffer("positionBuffer", m_positionBuffer);
		m_shaderProperties.SetBuffer("velocityBuffer", m_velocityBuffer);
		m_shaderProperties.SetBuffer("densityBuffer", m_densityBuffer);
		m_shaderProperties.SetBuffer("normalBuffer", m_normalBuffer);
		m_shaderProperties.SetBuffer("curvatureBuffer", m_curvatureBuffer);
		Renderer::DrawInstanced(m_particleCount, m_positionBuffer, m_particleMesh, m_particleMaterial, m_shaderProperties, localToWorld, false, false);
	}



	// Private methods:
	// Compute shader recording:
	void SphFluid2dGpu::ResetFluid()
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
		Compute::PreRender::RecordComputeShader(m_resetComputeShader, m_resetProperties, m_threadCount);
	}
	void SphFluid2dGpu::ComputeCellKeys(BufferView<Float2>& positionBufferView)
	{
		Uint3 threadCount(m_cellKeyBuffer.GetCount(), 1, 1);
		m_cellKeysProperties.SetBuffer("cellKeyBuffer", m_cellKeyBuffer);
		m_cellKeysProperties.SetBuffer("positionBuffer", positionBufferView.GetBuffer());
		Compute::PreRender::RecordComputeShader(m_cellKeysComputeShader, m_cellKeysProperties, threadCount);
	}
	void SphFluid2dGpu::ComputeStartIndices()
	{
		Uint3 threadCount(m_cellKeyBuffer.GetCount(), 1, 1);
		m_startIndicesProperties.SetBuffer("startIndexBuffer", m_startIndexBuffer);
		m_startIndicesProperties.SetBuffer("cellKeyBuffer", m_cellKeyBuffer);
		Compute::PreRender::RecordComputeShader(m_startIndicesComputeShader, m_startIndicesProperties, threadCount);
	}
	void SphFluid2dGpu::ComputeDensity(BufferView<Float2>& positionBufferView, ShaderProperties& shaderProperties)
	{
		shaderProperties.SetBuffer("cellKeyBuffer", m_cellKeyBuffer);
		shaderProperties.SetBuffer("startIndexBuffer", m_startIndexBuffer);
		shaderProperties.SetBuffer("positionBuffer", positionBufferView.GetBuffer());
		shaderProperties.SetBuffer("densityBuffer", m_densityBuffer);
		Compute::PreRender::RecordComputeShader(m_densityComputeShader, shaderProperties, m_threadCount);
	}
	void SphFluid2dGpu::ComputeNormalAndCurvature(BufferView<Float2>& positionBufferView, ShaderProperties& shaderProperties)
	{
		shaderProperties.SetBuffer("cellKeyBuffer", m_cellKeyBuffer);
		shaderProperties.SetBuffer("startIndexBuffer", m_startIndexBuffer);
		shaderProperties.SetBuffer("positionBuffer", positionBufferView.GetBuffer());
		shaderProperties.SetBuffer("densityBuffer", m_densityBuffer);
		shaderProperties.SetBuffer("normalBuffer", m_normalBuffer);
		shaderProperties.SetBuffer("curvatureBuffer", m_curvatureBuffer);
		Compute::PreRender::RecordComputeShader(m_normalAndCurvatureComputeShader, shaderProperties, m_threadCount);
	}
	void SphFluid2dGpu::ComputeCurvature()
	{

	}
	void SphFluid2dGpu::ComputeForceDensity(BufferView<Float2>& positionBufferView, BufferView<Float2>& velocityBufferView, ShaderProperties& shaderProperties)
	{
		shaderProperties.SetBuffer("cellKeyBuffer", m_cellKeyBuffer);
		shaderProperties.SetBuffer("startIndexBuffer", m_startIndexBuffer);
		shaderProperties.SetBuffer("positionBuffer", positionBufferView.GetBuffer());
		shaderProperties.SetBuffer("velocityBuffer", velocityBufferView.GetBuffer());
		shaderProperties.SetBuffer("densityBuffer", m_densityBuffer);
		shaderProperties.SetBuffer("normalBuffer", m_normalBuffer);
		shaderProperties.SetBuffer("curvatureBuffer", m_curvatureBuffer);
		shaderProperties.SetBuffer("forceDensityBuffer", m_forceDensityBuffer);
		Compute::PreRender::RecordComputeShader(m_forceDensityComputeShader, shaderProperties, m_threadCount);
	}
	void SphFluid2dGpu::ComputeRungeKutta2Step1()
	{
		m_rungeKutta2Step1Properties.SetBuffer("forceDensityBuffer", m_forceDensityBuffer);
		m_rungeKutta2Step1Properties.SetBuffer("densityBuffer", m_densityBuffer);
		m_rungeKutta2Step1Properties.SetBuffer("positionBuffer", m_positionBuffer);
		m_rungeKutta2Step1Properties.SetBuffer("velocityBuffer", m_velocityBuffer);
		m_rungeKutta2Step1Properties.SetBuffer("kp1Buffer", m_kp1Buffer);
		m_rungeKutta2Step1Properties.SetBuffer("kv1Buffer", m_kv1Buffer);
		m_rungeKutta2Step1Properties.SetBuffer("tempPositionBuffer", m_tempPositionBuffer);
		m_rungeKutta2Step1Properties.SetBuffer("tempVelocityBuffer", m_tempVelocityBuffer);
		Compute::PreRender::RecordComputeShader(m_rungeKutta2Step1ComputeShader, m_rungeKutta2Step1Properties, m_threadCount);
	}
	void SphFluid2dGpu::ComputeRungeKutta2Step2()
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
		Compute::PreRender::RecordComputeShader(m_rungeKutta2Step2ComputeShader, m_rungeKutta2Step2Properties, m_threadCount);
	}
	void SphFluid2dGpu::ComputeBoundaryCollisions()
	{
		m_boundaryCollisionsProperties.SetBuffer("positionBuffer", m_positionBuffer);
		m_boundaryCollisionsProperties.SetBuffer("velocityBuffer", m_velocityBuffer);
		Compute::PreRender::RecordComputeShader(m_boundaryCollisionsComputeShader, m_boundaryCollisionsProperties, m_threadCount);
	}
}