#include "sphFluid3dGpuSolver.h"
#include "logger.h"



namespace fluidDynamics
{
	// ScratchData struct:
	int SphFluid3dGpuSolver::ScratchData::ParticleCount()
	{
		return forceDensityBuffer.IsValid() ? forceDensityBuffer.GetCount() : 0;
	}
	void SphFluid3dGpuSolver::ScratchData::Reallocate(int particleCount)
	{
		int hashGridSize = math::NextPrimeAbove(2 * particleCount);

		if (particleCount != ParticleCount())
		{
			// Reallocate:
			cellKeyBuffer = BufferTyped<uint32_t>((uint32_t)particleCount, "cellKeyBuffer", BufferUsage::storage);
			startIndexBuffer = BufferTyped<uint32_t>((uint32_t)hashGridSize, "startIndexBuffer", BufferUsage::storage);
			sortPermutationBuffer = BufferTyped<uint32_t>((uint32_t)particleCount, "sortPermutationBuffer", BufferUsage::storage);
			forceDensityBuffer = BufferTyped<Float3>((uint32_t)particleCount, "forceDensityBuffer", BufferUsage::storage);
			nearDensityBuffer = BufferTyped<float>((uint32_t)particleCount, "nearDensityBuffer", BufferUsage::storage);
			tempBuffer0 = BufferTyped<Float3>((uint32_t)particleCount, "tempBuffer0", BufferUsage::storage);
			tempBuffer1 = BufferTyped<Float3>((uint32_t)particleCount, "tempBuffer1", BufferUsage::storage);
			tempBuffer2 = BufferTyped<Float3>((uint32_t)particleCount, "tempBuffer2", BufferUsage::storage);
			tempBuffer3 = BufferTyped<Float3>((uint32_t)particleCount, "tempBuffer3", BufferUsage::storage);
			tempBuffer4 = BufferTyped<Float3>((uint32_t)particleCount, "tempBuffer4", BufferUsage::storage);
			tempBuffer5 = BufferTyped<Float3>((uint32_t)particleCount, "tempBuffer5", BufferUsage::storage);
			tempBuffer6 = BufferTyped<Float3>((uint32_t)particleCount, "tempBuffer6", BufferUsage::storage);
			tempBuffer7 = BufferTyped<Float3>((uint32_t)particleCount, "tempBuffer7", BufferUsage::storage);
			kp1Buffer = BufferTyped<Float3>((uint32_t)particleCount, "kp1Buffer", BufferUsage::storage);
			kv1Buffer = BufferTyped<Float3>((uint32_t)particleCount, "kv1Buffer", BufferUsage::storage);
			tempPositionBuffer = BufferTyped<Float3>((uint32_t)particleCount, "tempPositionBuffer", BufferUsage::storage);
			tempVelocityBuffer = BufferTyped<Float3>((uint32_t)particleCount, "tempVelocityBuffer", BufferUsage::storage);
		}
	}



	// TripleData struct:
	int SphFluid3dGpuSolver::TripleData::ParticleCount()
	{
		return positionBuffer.IsValid() ? positionBuffer.GetCount() : 0;
	}
	void SphFluid3dGpuSolver::TripleData::Reallocate(int particleCount, Uint3 densityResolution, Uint3 opticalDepthResolution)
	{
		if (particleCount != ParticleCount())
		{
			positionBuffer = TripleBuffer<Float3>((uint32_t)particleCount, "positionBuffer", BufferUsage::storage);
			velocityBuffer = TripleBuffer<Float3>((uint32_t)particleCount, "velocityBuffer", BufferUsage::storage);
			densityBuffer = TripleBuffer<float>((uint32_t)particleCount, "densityBuffer", BufferUsage::storage);
			normalBuffer = TripleBuffer<Float3>((uint32_t)particleCount, "normalBuffer", BufferUsage::storage);
			curvatureBuffer = TripleBuffer<float>((uint32_t)particleCount, "curvatureBuffer", BufferUsage::storage);
		}
		ReallocateDensityTexture3d(densityResolution);
		ReallocateOpticalDepthTexture3d(opticalDepthResolution);
	}
	void SphFluid3dGpuSolver::TripleData::ReallocateDensityTexture3d(Uint3 resolution)
	{
		// Limit total voxel count, as gpu memory usage is resolution.x*y*z * 4bytes per texture:
		size_t voxelCount = static_cast<size_t>(resolution.x) * resolution.y * resolution.z;
		constexpr size_t maxVoxelCount = 1 << 24;	// = 2^24 * 4bytes = 64MB as max limit.
		if (voxelCount > maxVoxelCount)
		{
			Uint3 originalResolution = resolution;
			float scale = math::Pow(static_cast<float>(maxVoxelCount) / static_cast<float>(voxelCount), 1.0f / 3.0f);
			resolution = Uint3::Max(Uint3::one, Uint3(Float3::Floor(Float3(resolution) * scale)));
			LOG_WARN("DensityTexture3d resolution {} exceeds maxVoxelCount, clamped to {}.", originalResolution.ToString(), resolution.ToString());
		}

		if (resolution != densityTexture3dResolution)
		{
			densityTexture3dResolution = resolution;
			for (uint32_t i = 0; i < PhysicsTripleBufferState::bufferCount; i++)
				densityTexture3d[i] = Texture3d("densityTexture3d[" + std::to_string(i) + "]", resolution.x, resolution.y, resolution.z, emberCommon::TextureFormats::r32_sfloat, emberCommon::TextureUsage::storageSample);
		}
	}
	void SphFluid3dGpuSolver::TripleData::ReallocateOpticalDepthTexture3d(Uint3 resolution)
	{
		// Limit total voxel count, as gpu memory usage is resolution.x*y*z * 2bytes per texture:
		size_t voxelCount = static_cast<size_t>(resolution.x) * resolution.y * resolution.z;
		constexpr size_t maxVoxelCount = 1 << 24;	// = 2^24 * 2bytes = 32MB as max limit.
		if (voxelCount > maxVoxelCount)
		{
		    Uint3 originalResolution = resolution;
			float scale = math::Pow(static_cast<float>(maxVoxelCount) / static_cast<float>(voxelCount), 1.0f / 3.0f);
			resolution = Uint3::Max(Uint3::one, Uint3(Float3::Floor(Float3(resolution) * scale)));
			LOG_WARN("OpticalDepthTexture3d resolution {} exceeds maxVoxelCount, clamped to {}.", originalResolution.ToString(), resolution.ToString());
		}

		if (resolution != opticalDepthTexture3dResolution)
		{
			opticalDepthTexture3dResolution = resolution;
			for (uint32_t i = 0; i < PhysicsTripleBufferState::bufferCount; i++)
			{
				opticalDepthTexture3d[i] = Texture3d("opticalDepthTexture3d[" + std::to_string(i) + "]", resolution.x, resolution.y, resolution.z, emberCommon::TextureFormats::r16_sfloat, emberCommon::TextureUsage::storageSample);
				hasOpticalDepthTexture3d[i] = false;
			}
		}
	}



	// ComputeShaders struct:
	SphFluid3dGpuSolver::ComputeShaders::ComputeShaders()
	{
		computeType = ComputeType::physics;
		sessionID = -1;

		// Load compute shaders:
		std::filesystem::path directoryPath = (std::filesystem::path(PROJECT_SHADERS_DIR) / "bin").make_preferred();
		resetDataComputeShader = ComputeShader("resetData3d", directoryPath / "resetData3d.comp.spv");
		resetRungeKuttaComputeShader = ComputeShader("resetRungeKutta3d", directoryPath / "resetRungeKutta3d.comp.spv");
		cellKeysComputeShader = ComputeShader("cellKeys3d", directoryPath / "cellKeys3d.comp.spv");
		startIndicesResetComputeShader = ComputeShader("startIndicesReset3d", directoryPath / "startIndicesReset3d.comp.spv");
		startIndicesComputeShader = ComputeShader("startIndices3d", directoryPath / "startIndices3d.comp.spv");
		densityComputeShader = ComputeShader("density3d", directoryPath / "density3d.comp.spv");
		normalAndCurvatureComputeShader = ComputeShader("normalAndCurvature3d", directoryPath / "normalAndCurvature3d.comp.spv");
		forceDensityComputeShader = ComputeShader("forceDensity3d", directoryPath / "forceDensity3d.comp.spv");
		rungeKutta2Step1ComputeShader = ComputeShader("rungeKutta2Step1_3d", directoryPath / "rungeKutta2Step1_3d.comp.spv");
		rungeKutta2Step2ComputeShader = ComputeShader("rungeKutta2Step2_3d", directoryPath / "rungeKutta2Step2_3d.comp.spv");
		boundaryCollisionsComputeShader = ComputeShader("boundaryCollisions3d", directoryPath / "boundaryCollisions3d.comp.spv");
		densityTexture3dComputeShader = ComputeShader("densityTexture3d", directoryPath / "densityTexture3d.comp.spv");
		opticalDepthTexture3dComputeShader = ComputeShader("opticalDepthTexture3d", directoryPath / "opticalDepthTexture3d.comp.spv");
	}
	void SphFluid3dGpuSolver::ComputeShaders::SetUseHashGridOptimization(bool useHashGridOptimization)
	{
		densityComputeShader.SetValue("Values", "useHashGridOptimization", (int)useHashGridOptimization);
		normalAndCurvatureComputeShader.SetValue("Values", "useHashGridOptimization", (int)useHashGridOptimization);
		forceDensityComputeShader.SetValue("Values", "useHashGridOptimization", (int)useHashGridOptimization);
	}
	void SphFluid3dGpuSolver::ComputeShaders::SetEffectRadius(float effectRadius)
	{
		cellKeysComputeShader.SetValue("Values", "effectRadius", effectRadius);
		densityComputeShader.SetValue("Values", "effectRadius", effectRadius);
		normalAndCurvatureComputeShader.SetValue("Values", "effectRadius", effectRadius);
		forceDensityComputeShader.SetValue("Values", "effectRadius", effectRadius);
		densityTexture3dComputeShader.SetValue("Values", "effectRadius", effectRadius);
	}
	void SphFluid3dGpuSolver::ComputeShaders::SetHashGridSize(int hashGridSize)
	{
		cellKeysComputeShader.SetValue("Values", "hashGridSize", hashGridSize);
		densityComputeShader.SetValue("Values", "hashGridSize", hashGridSize);
		normalAndCurvatureComputeShader.SetValue("Values", "hashGridSize", hashGridSize);
		forceDensityComputeShader.SetValue("Values", "hashGridSize", hashGridSize);
		densityTexture3dComputeShader.SetValue("Values", "hashGridSize", hashGridSize);
	}
	void SphFluid3dGpuSolver::ComputeShaders::SetMass(float mass)
	{
		densityComputeShader.SetValue("Values", "mass", mass);
		normalAndCurvatureComputeShader.SetValue("Values", "mass", mass);
		forceDensityComputeShader.SetValue("Values", "mass", mass);
		densityTexture3dComputeShader.SetValue("Values", "mass", mass);
	}
	void SphFluid3dGpuSolver::ComputeShaders::SetViscosity(float viscosity)
	{
		forceDensityComputeShader.SetValue("Values", "viscosity", viscosity);
	}
	void SphFluid3dGpuSolver::ComputeShaders::SetSurfaceTension(float surfaceTension)
	{
		forceDensityComputeShader.SetValue("Values", "surfaceTension", surfaceTension);
	}
	void SphFluid3dGpuSolver::ComputeShaders::SetTargetDensity(float targetDensity)
	{
		forceDensityComputeShader.SetValue("Values", "targetDensity", targetDensity);
	}
	void SphFluid3dGpuSolver::ComputeShaders::SetPressureMultiplier(float pressureMultiplier)
	{
		forceDensityComputeShader.SetValue("Values", "pressureMultiplier", pressureMultiplier);
	}
	void SphFluid3dGpuSolver::ComputeShaders::SetNearPressureRatio(float nearPressureRatio)
	{
		forceDensityComputeShader.SetValue("Values", "nearPressureRatio", nearPressureRatio);
	}
	void SphFluid3dGpuSolver::ComputeShaders::SetGravity(float gravity)
	{
		forceDensityComputeShader.SetValue("Values", "gravity", gravity);
	}
	void SphFluid3dGpuSolver::ComputeShaders::SetMaxVelocity(float maxVelocity)
	{
		rungeKutta2Step1ComputeShader.SetValue("ShaderValues", "maxVelocity", maxVelocity);
		rungeKutta2Step2ComputeShader.SetValue("ShaderValues", "maxVelocity", maxVelocity);
	}
	void SphFluid3dGpuSolver::ComputeShaders::SetAttractorRadius(float attractorRadius)
	{
		forceDensityComputeShader.SetValue("Values", "attractorRadius", attractorRadius);
	}
	void SphFluid3dGpuSolver::ComputeShaders::SetAttractorStrength(float attractorStrength)
	{
		forceDensityComputeShader.SetValue("Values", "attractorStrength", attractorStrength);
	}
	void SphFluid3dGpuSolver::ComputeShaders::SetAttractorState(int attractorState)
	{
		forceDensityComputeShader.SetValue("Values", "attractorState", attractorState);
	}
	void SphFluid3dGpuSolver::ComputeShaders::SetAttractorPoint(const Float3& attractorPoint)
	{
		forceDensityComputeShader.SetValue("Values", "attractorPoint", attractorPoint);
	}




	// Public methods:
	// Ordinary Differential Equation Solvers:
	void SphFluid3dGpuSolver::TimeStepRungeKutta2(float dt, const Settings& settings, ComputeShaders& computeShaders, ScratchData& scratchData, TripleData& tripleData, uint32_t sourceDataIndex, uint32_t destinationDataIndex)
	{
        // Scratch data buffer views:
		BufferView<uint32_t>& cellKeyBufferView = scratchData.cellKeyBuffer.GetBufferView();
		BufferView<uint32_t>& startIndexBufferView = scratchData.startIndexBuffer.GetBufferView();
		BufferView<uint32_t>& sortPermutationBufferView = scratchData.sortPermutationBuffer.GetBufferView();
		BufferView<Float3>& forceDensityBufferView = scratchData.forceDensityBuffer.GetBufferView();
		BufferView<float>& nearDensityBufferView = scratchData.nearDensityBuffer.GetBufferView();
        BufferView<Float3>& tempBufferView0 = scratchData.tempBuffer0.GetBufferView();
        BufferView<Float3>& tempBufferView1 = scratchData.tempBuffer1.GetBufferView();
        BufferView<Float3>& tempBufferView2 = scratchData.tempBuffer2.GetBufferView();
        BufferView<Float3>& tempBufferView3 = scratchData.tempBuffer3.GetBufferView();
        BufferView<Float3>& tempBufferView4 = scratchData.tempBuffer4.GetBufferView();
        BufferView<Float3>& tempBufferView5 = scratchData.tempBuffer5.GetBufferView();
        BufferView<Float3>& tempBufferView6 = scratchData.tempBuffer6.GetBufferView();
        BufferView<Float3>& tempBufferView7 = scratchData.tempBuffer7.GetBufferView();
        BufferView<Float3>& kp1BufferView = scratchData.kp1Buffer.GetBufferView();
        BufferView<Float3>& kv1BufferView = scratchData.kv1Buffer.GetBufferView();
        BufferView<Float3>& tempPositionBufferView = scratchData.tempPositionBuffer.GetBufferView();
        BufferView<Float3>& tempVelocityBufferView = scratchData.tempVelocityBuffer.GetBufferView();
        // Triple buffer views:
		BufferView<Float3>& sourcePositionBufferView = tripleData.positionBuffer.GetBufferView(sourceDataIndex);
		BufferView<Float3>& sourceVelocityBufferView = tripleData.velocityBuffer.GetBufferView(sourceDataIndex);
		BufferView<float>& destinationDensityBufferView = tripleData.densityBuffer.GetBufferView(destinationDataIndex);
		BufferView<Float3>& destinationNormalBufferView = tripleData.normalBuffer.GetBufferView(destinationDataIndex);
		BufferView<float>& destinationCurvatureBufferView = tripleData.curvatureBuffer.GetBufferView(destinationDataIndex);
		BufferView<Float3>& destinationPositionBufferView = tripleData.positionBuffer.GetBufferView(destinationDataIndex);
		BufferView<Float3>& destinationVelocityBufferView = tripleData.velocityBuffer.GetBufferView(destinationDataIndex);

		// Update hash grid for fast nearest neighbor particle look up:
		Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType, computeShaders.sessionID);
		if (settings.useHashGridOptimization)
		{
			ComputeCellKeys(computeShaders, cellKeyBufferView, sourcePositionBufferView);
			Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType, computeShaders.sessionID);

			GpuSort<uint32_t>::SortPermutation(computeShaders.computeType, cellKeyBufferView, sortPermutationBufferView, computeShaders.sessionID);
			Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType, computeShaders.sessionID);

			ComputeStartIndices(computeShaders, startIndexBufferView, cellKeyBufferView);
			GpuSort<Float3>::ApplyPermutation(computeShaders.computeType, sortPermutationBufferView, sourcePositionBufferView, tempBufferView0, computeShaders.sessionID);
			GpuSort<Float3>::ApplyPermutation(computeShaders.computeType, sortPermutationBufferView, sourceVelocityBufferView, tempBufferView1, computeShaders.sessionID);
			Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType, computeShaders.sessionID);
		}

		// First Runge-Kutta step:
		BufferView<Float3>& currentPositionBufferView = settings.useHashGridOptimization ? tempBufferView0 : sourcePositionBufferView;
		BufferView<Float3>& currentVelocityBufferView = settings.useHashGridOptimization ? tempBufferView1 : sourceVelocityBufferView;
		ComputeDensities(computeShaders, destinationDensityBufferView, nearDensityBufferView, currentPositionBufferView, startIndexBufferView, cellKeyBufferView);
		Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType, computeShaders.sessionID);
		ComputeNormalsAndCurvatures(computeShaders, destinationNormalBufferView, destinationCurvatureBufferView, destinationDensityBufferView, currentPositionBufferView, startIndexBufferView, cellKeyBufferView);
		Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType, computeShaders.sessionID);
		ComputeForceDensities(computeShaders, forceDensityBufferView, destinationDensityBufferView, nearDensityBufferView, currentPositionBufferView, currentVelocityBufferView, destinationNormalBufferView, destinationCurvatureBufferView, startIndexBufferView, cellKeyBufferView);
		Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType, computeShaders.sessionID);
		ComputeRungeKutta2Step1(computeShaders, dt, forceDensityBufferView, destinationDensityBufferView, currentPositionBufferView, currentVelocityBufferView, kp1BufferView, kv1BufferView, tempPositionBufferView, tempVelocityBufferView);
		Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType, computeShaders.sessionID);

		// Resolve boundary collisions for the intermediate Runge-Kutta state:
		ComputeBoundaryCollisions(computeShaders, tempPositionBufferView, tempVelocityBufferView, settings.fluidBounds, settings.collisionDampening);
		Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType, computeShaders.sessionID);

		// Update hash grid for fast nearest neighbor particle look up:
		if (settings.useHashGridOptimization)
		{
			ComputeCellKeys(computeShaders, cellKeyBufferView, tempPositionBufferView);
			Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType, computeShaders.sessionID);

			GpuSort<uint32_t>::SortPermutation(computeShaders.computeType, cellKeyBufferView, sortPermutationBufferView, computeShaders.sessionID);
			Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType, computeShaders.sessionID);

			ComputeStartIndices(computeShaders, startIndexBufferView, cellKeyBufferView);
			GpuSort<Float3>::ApplyPermutation(computeShaders.computeType, sortPermutationBufferView, tempPositionBufferView, tempBufferView2, computeShaders.sessionID);
			GpuSort<Float3>::ApplyPermutation(computeShaders.computeType, sortPermutationBufferView, tempVelocityBufferView, tempBufferView3, computeShaders.sessionID);
			GpuSort<Float3>::ApplyPermutation(computeShaders.computeType, sortPermutationBufferView, kp1BufferView, tempBufferView4, computeShaders.sessionID);
			GpuSort<Float3>::ApplyPermutation(computeShaders.computeType, sortPermutationBufferView, kv1BufferView, tempBufferView5, computeShaders.sessionID);
			GpuSort<Float3>::ApplyPermutation(computeShaders.computeType, sortPermutationBufferView, currentPositionBufferView, tempBufferView6, computeShaders.sessionID);
			GpuSort<Float3>::ApplyPermutation(computeShaders.computeType, sortPermutationBufferView, currentVelocityBufferView, tempBufferView7, computeShaders.sessionID);
			Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType, computeShaders.sessionID);
		}
		BufferView<Float3>& secondStepPositionBufferView = settings.useHashGridOptimization ? tempBufferView2 : tempPositionBufferView;
		BufferView<Float3>& secondStepVelocityBufferView = settings.useHashGridOptimization ? tempBufferView3 : tempVelocityBufferView;
		BufferView<Float3>& secondStepKp1BufferView = settings.useHashGridOptimization ? tempBufferView4 : kp1BufferView;
		BufferView<Float3>& secondStepKv1BufferView = settings.useHashGridOptimization ? tempBufferView5 : kv1BufferView;
		BufferView<Float3>& orderedSourcePositionBufferView = settings.useHashGridOptimization ? tempBufferView6 : sourcePositionBufferView;
		BufferView<Float3>& orderedSourceVelocityBufferView = settings.useHashGridOptimization ? tempBufferView7 : sourceVelocityBufferView;

		// Second Runge-Kutta step:
		ComputeDensities(computeShaders, destinationDensityBufferView, nearDensityBufferView, secondStepPositionBufferView, startIndexBufferView, cellKeyBufferView);
		Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType, computeShaders.sessionID);
		ComputeNormalsAndCurvatures(computeShaders, destinationNormalBufferView, destinationCurvatureBufferView, destinationDensityBufferView, secondStepPositionBufferView, startIndexBufferView, cellKeyBufferView);
		Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType, computeShaders.sessionID);
		ComputeForceDensities(computeShaders, forceDensityBufferView, destinationDensityBufferView, nearDensityBufferView, secondStepPositionBufferView, secondStepVelocityBufferView, destinationNormalBufferView, destinationCurvatureBufferView, startIndexBufferView, cellKeyBufferView);
		Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType, computeShaders.sessionID);
		ComputeRungeKutta2Step2(computeShaders, dt, forceDensityBufferView, destinationDensityBufferView, secondStepKp1BufferView, secondStepKv1BufferView, secondStepVelocityBufferView, orderedSourcePositionBufferView, orderedSourceVelocityBufferView, destinationPositionBufferView, destinationVelocityBufferView);
		Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType, computeShaders.sessionID);

		// Resolve boundary collisions:
		ComputeBoundaryCollisions(computeShaders, destinationPositionBufferView, destinationVelocityBufferView, settings.fluidBounds, settings.collisionDampening);
	}



	// Data management:
	void SphFluid3dGpuSolver::ResetData(ComputeShaders& computeShaders, ScratchData& scratchData, TripleData& tripleData, uint32_t dataIndex, float initialDistributionRadius)
	{
		int hashGridSize = math::NextPrimeAbove(2 * tripleData.ParticleCount());
		Uint3 threadCount(tripleData.ParticleCount(), 1, 1);
		ShaderProperties shaderProperties = Compute::RecordComputeShader(computeShaders.computeType, computeShaders.resetDataComputeShader, threadCount, computeShaders.sessionID);
		shaderProperties.SetValue("CallValues", "hashGridSize", hashGridSize);
		shaderProperties.SetValue("CallValues", "initialDistributionRadius", initialDistributionRadius);
		shaderProperties.SetBuffer("cellKeyBuffer", scratchData.cellKeyBuffer.GetBuffer());
		shaderProperties.SetBuffer("startIndexBuffer", scratchData.startIndexBuffer.GetBuffer());
		shaderProperties.SetBuffer("sortPermutationBuffer", scratchData.sortPermutationBuffer.GetBuffer());
		shaderProperties.SetBuffer("positionBuffer", tripleData.positionBuffer.GetBuffer(dataIndex));
		shaderProperties.SetBuffer("velocityBuffer", tripleData.velocityBuffer.GetBuffer(dataIndex));
		shaderProperties.SetBuffer("densityBuffer", tripleData.densityBuffer.GetBuffer(dataIndex));
		shaderProperties.SetBuffer("normalBuffer", tripleData.normalBuffer.GetBuffer(dataIndex));
		shaderProperties.SetBuffer("curvatureBuffer", tripleData.curvatureBuffer.GetBuffer(dataIndex));
		shaderProperties.SetBuffer("forceDensityBuffer", scratchData.forceDensityBuffer.GetBuffer());
		shaderProperties.SetBuffer("nearDensityBuffer", scratchData.nearDensityBuffer.GetBuffer());
		shaderProperties.SetBuffer("tempBuffer0", scratchData.tempBuffer0.GetBuffer());
		shaderProperties.SetBuffer("tempBuffer1", scratchData.tempBuffer1.GetBuffer());
		shaderProperties.SetBuffer("tempBuffer2", scratchData.tempBuffer2.GetBuffer());
		shaderProperties.SetBuffer("tempBuffer3", scratchData.tempBuffer3.GetBuffer());
		shaderProperties.SetBuffer("tempBuffer4", scratchData.tempBuffer4.GetBuffer());
		shaderProperties.SetBuffer("tempBuffer5", scratchData.tempBuffer5.GetBuffer());
		shaderProperties.SetBuffer("tempBuffer6", scratchData.tempBuffer6.GetBuffer());
		shaderProperties.SetBuffer("tempBuffer7", scratchData.tempBuffer7.GetBuffer());

		ShaderProperties rungeKuttaShaderProperties = Compute::RecordComputeShader(computeShaders.computeType, computeShaders.resetRungeKuttaComputeShader, threadCount, computeShaders.sessionID);
		rungeKuttaShaderProperties.SetBuffer("kp1Buffer", scratchData.kp1Buffer.GetBuffer());
		rungeKuttaShaderProperties.SetBuffer("kv1Buffer", scratchData.kv1Buffer.GetBuffer());
		rungeKuttaShaderProperties.SetBuffer("tempPositionBuffer", scratchData.tempPositionBuffer.GetBuffer());
		rungeKuttaShaderProperties.SetBuffer("tempVelocityBuffer", scratchData.tempVelocityBuffer.GetBuffer());
	}
	// Field computations:
	void SphFluid3dGpuSolver::ComputeCellKeys(ComputeShaders& computeShaders, const BufferView<uint32_t>& cellKeyBufferView, const BufferView<Float3>& positionBufferView)
	{
		Uint3 threadCount(cellKeyBufferView.GetCount(), 1, 1);
		ShaderProperties shaderProperties = Compute::RecordComputeShader(computeShaders.computeType, computeShaders.cellKeysComputeShader, threadCount, computeShaders.sessionID);
		shaderProperties.SetBuffer("cellKeyBuffer", cellKeyBufferView.GetBuffer());
		shaderProperties.SetBuffer("positionBuffer", positionBufferView.GetBuffer());
	}
	void SphFluid3dGpuSolver::ComputeStartIndices(ComputeShaders& computeShaders, const BufferView<uint32_t>& startIndexBufferView, const BufferView<uint32_t>& cellKeyBufferView)
	{
		// Reset start index buffer:
		{
			Uint3 threadCount(startIndexBufferView.GetCount(), 1, 1);	// reset all possible start indices.
			ShaderProperties shaderProperties = Compute::RecordComputeShader(computeShaders.computeType, computeShaders.startIndicesResetComputeShader, threadCount, computeShaders.sessionID);
			shaderProperties.SetBuffer("startIndexBuffer", startIndexBufferView.GetBuffer());
		}
		// Compute start indices:
		Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType, computeShaders.sessionID);
		{
			Uint3 threadCount(cellKeyBufferView.GetCount(), 1, 1);	// start indices only needed for each particle.
			ShaderProperties shaderProperties = Compute::RecordComputeShader(computeShaders.computeType, computeShaders.startIndicesComputeShader, threadCount, computeShaders.sessionID);
			shaderProperties.SetBuffer("startIndexBuffer", startIndexBufferView.GetBuffer());
			shaderProperties.SetBuffer("cellKeyBuffer", cellKeyBufferView.GetBuffer());
		}
	}
	void SphFluid3dGpuSolver::ComputeDensities(ComputeShaders& computeShaders, const BufferView<float>& densityBufferView, const BufferView<float>& nearDensityBufferView, const BufferView<Float3>& positionBufferView, const BufferView<uint32_t>& startIndexBufferView, const BufferView<uint32_t>& cellKeyBufferView)
	{
		Uint3 threadCount(densityBufferView.GetCount(), 1, 1);
		ShaderProperties shaderProperties = Compute::RecordComputeShader(computeShaders.computeType, computeShaders.densityComputeShader, threadCount, computeShaders.sessionID);
		shaderProperties.SetBuffer("densityBuffer", densityBufferView.GetBuffer());
		shaderProperties.SetBuffer("nearDensityBuffer", nearDensityBufferView.GetBuffer());
		shaderProperties.SetBuffer("positionBuffer", positionBufferView.GetBuffer());
		shaderProperties.SetBuffer("startIndexBuffer", startIndexBufferView.GetBuffer());
		shaderProperties.SetBuffer("cellKeyBuffer", cellKeyBufferView.GetBuffer());
	}
	void SphFluid3dGpuSolver::ComputeNormalsAndCurvatures(ComputeShaders& computeShaders, const BufferView<Float3>& normalBufferView, const BufferView<float>& curvatureBufferView, const BufferView<float>& densityBufferView, const BufferView<Float3>& positionBufferView, const BufferView<uint32_t>& startIndexBufferView, const BufferView<uint32_t>& cellKeyBufferView)
	{
		Uint3 threadCount(normalBufferView.GetCount(), 1, 1);
		ShaderProperties shaderProperties = Compute::RecordComputeShader(computeShaders.computeType, computeShaders.normalAndCurvatureComputeShader, threadCount, computeShaders.sessionID);
		shaderProperties.SetBuffer("normalBuffer", normalBufferView.GetBuffer());
		shaderProperties.SetBuffer("curvatureBuffer", curvatureBufferView.GetBuffer());
		shaderProperties.SetBuffer("densityBuffer", densityBufferView.GetBuffer());
		shaderProperties.SetBuffer("positionBuffer", positionBufferView.GetBuffer());
		shaderProperties.SetBuffer("startIndexBuffer", startIndexBufferView.GetBuffer());
		shaderProperties.SetBuffer("cellKeyBuffer", cellKeyBufferView.GetBuffer());
	}
	void SphFluid3dGpuSolver::ComputeForceDensities(ComputeShaders& computeShaders, const BufferView<Float3>& forceDensityBufferView, const BufferView<float>& densityBufferView, const BufferView<float>& nearDensityBufferView, const BufferView<Float3>& positionBufferView, const BufferView<Float3>& velocityBufferView, const BufferView<Float3>& normalBufferView, const BufferView<float>& curvatureBufferView, const BufferView<uint32_t>& startIndexBufferView, const BufferView<uint32_t>& cellKeyBufferView)
	{
		Uint3 threadCount(forceDensityBufferView.GetCount(), 1, 1);
		ShaderProperties shaderProperties = Compute::RecordComputeShader(computeShaders.computeType, computeShaders.forceDensityComputeShader, threadCount, computeShaders.sessionID);
		shaderProperties.SetBuffer("cellKeyBuffer", cellKeyBufferView.GetBuffer());
		shaderProperties.SetBuffer("startIndexBuffer", startIndexBufferView.GetBuffer());
		shaderProperties.SetBuffer("positionBuffer", positionBufferView.GetBuffer());
		shaderProperties.SetBuffer("velocityBuffer", velocityBufferView.GetBuffer());
		shaderProperties.SetBuffer("densityBuffer", densityBufferView.GetBuffer());
		shaderProperties.SetBuffer("nearDensityBuffer", nearDensityBufferView.GetBuffer());
		shaderProperties.SetBuffer("normalBuffer", normalBufferView.GetBuffer());
		shaderProperties.SetBuffer("curvatureBuffer", curvatureBufferView.GetBuffer());
		shaderProperties.SetBuffer("forceDensityBuffer", forceDensityBufferView.GetBuffer());
	}
	void SphFluid3dGpuSolver::ComputeRungeKutta2Step1(ComputeShaders& computeShaders, float dt, const BufferView<Float3>& forceDensityBufferView, const BufferView<float>& densityBufferView, const BufferView<Float3>& positionBufferView, const BufferView<Float3>& velocityBufferView, const BufferView<Float3>& kp1BufferView, const BufferView<Float3>& kv1BufferView, const BufferView<Float3>& tempPositionBufferView, const BufferView<Float3>& tempVelocityBufferView)
	{
		Uint3 threadCount(positionBufferView.GetCount(), 1, 1);
		ShaderProperties shaderProperties = Compute::RecordComputeShader(computeShaders.computeType, computeShaders.rungeKutta2Step1ComputeShader, threadCount, computeShaders.sessionID);
		shaderProperties.SetValue("CallValues", "dt", dt);
		shaderProperties.SetBuffer("forceDensityBuffer", forceDensityBufferView.GetBuffer());
		shaderProperties.SetBuffer("densityBuffer", densityBufferView.GetBuffer());
		shaderProperties.SetBuffer("positionBuffer", positionBufferView.GetBuffer());
		shaderProperties.SetBuffer("velocityBuffer", velocityBufferView.GetBuffer());
		shaderProperties.SetBuffer("kp1Buffer", kp1BufferView.GetBuffer());
		shaderProperties.SetBuffer("kv1Buffer", kv1BufferView.GetBuffer());
		shaderProperties.SetBuffer("tempPositionBuffer", tempPositionBufferView.GetBuffer());
		shaderProperties.SetBuffer("tempVelocityBuffer", tempVelocityBufferView.GetBuffer());
	}
	void SphFluid3dGpuSolver::ComputeRungeKutta2Step2(ComputeShaders& computeShaders, float dt, const BufferView<Float3>& forceDensityBufferView, const BufferView<float>& densityBufferView, const BufferView<Float3>& kp1BufferView, const BufferView<Float3>& kv1BufferView, const BufferView<Float3>& tempVelocityBufferView, const BufferView<Float3>& sourcePositionBufferView, const BufferView<Float3>& sourceVelocityBufferView, const BufferView<Float3>& positionBufferView, const BufferView<Float3>& velocityBufferView)
	{
		Uint3 threadCount(positionBufferView.GetCount(), 1, 1);
		ShaderProperties shaderProperties = Compute::RecordComputeShader(computeShaders.computeType, computeShaders.rungeKutta2Step2ComputeShader, threadCount, computeShaders.sessionID);
		shaderProperties.SetValue("CallValues", "dt", dt);
		shaderProperties.SetBuffer("forceDensityBuffer", forceDensityBufferView.GetBuffer());
		shaderProperties.SetBuffer("densityBuffer", densityBufferView.GetBuffer());
		shaderProperties.SetBuffer("kp1Buffer", kp1BufferView.GetBuffer());
		shaderProperties.SetBuffer("kv1Buffer", kv1BufferView.GetBuffer());
		shaderProperties.SetBuffer("tempVelocityBuffer", tempVelocityBufferView.GetBuffer());
		shaderProperties.SetBuffer("sourcePositionBuffer", sourcePositionBufferView.GetBuffer());
		shaderProperties.SetBuffer("sourceVelocityBuffer", sourceVelocityBufferView.GetBuffer());
		shaderProperties.SetBuffer("positionBuffer", positionBufferView.GetBuffer());
		shaderProperties.SetBuffer("velocityBuffer", velocityBufferView.GetBuffer());
	}
	void SphFluid3dGpuSolver::ComputeBoundaryCollisions(ComputeShaders& computeShaders, const BufferView<Float3>& positionBufferView, const BufferView<Float3>& velocityBufferView, const RotatedBounds& fluidBounds, float collisionDampening)
	{
		Uint3 threadCount(positionBufferView.GetCount(), 1, 1);
		ShaderProperties shaderProperties = Compute::RecordComputeShader(computeShaders.computeType, computeShaders.boundaryCollisionsComputeShader, threadCount, computeShaders.sessionID);
		shaderProperties.SetValue("CallValues", "boundsMin", fluidBounds.localBounds.GetMin());
		shaderProperties.SetValue("CallValues", "boundsMax", fluidBounds.localBounds.GetMax());
		shaderProperties.SetValue("CallValues", "collisionDampening", collisionDampening);
		shaderProperties.SetValue("CallValues", "rotation", fluidBounds.GetRotation4x4());
		shaderProperties.SetValue("CallValues", "inverseRotation", fluidBounds.GetRotation4x4().Inverse());
		shaderProperties.SetBuffer("positionBuffer", positionBufferView.GetBuffer());
		shaderProperties.SetBuffer("velocityBuffer", velocityBufferView.GetBuffer());
	}

	void SphFluid3dGpuSolver::ComputeDensityTexture3d(ComputeShaders& computeShaders, ScratchData& scratchData, TripleData& tripleData, uint32_t dataIndex)
	{
        // Reference to needed buffers:
		BufferView<uint32_t>& cellKeyBufferView = scratchData.cellKeyBuffer.GetBufferView();
		BufferView<uint32_t>& startIndexBufferView = scratchData.startIndexBuffer.GetBufferView();
		BufferView<uint32_t>& sortPermutationBufferView = scratchData.sortPermutationBuffer.GetBufferView();
		BufferView<Float3>& positionBufferView = tripleData.positionBuffer.GetBufferView(dataIndex);
		BufferView<Float3>& sortedPositionBufferView = scratchData.tempBuffer0.GetBufferView();
		Texture3d& densityTexture = tripleData.densityTexture3d[dataIndex];
		const RotatedBounds& fluidBounds = tripleData.fluidBounds[dataIndex];

		// Build hash grid:
		ComputeCellKeys(computeShaders, cellKeyBufferView, positionBufferView);
		Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType, computeShaders.sessionID);
		GpuSort<uint32_t>::SortPermutation(computeShaders.computeType, cellKeyBufferView, sortPermutationBufferView, computeShaders.sessionID);
		Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType, computeShaders.sessionID);
		ComputeStartIndices(computeShaders, startIndexBufferView, cellKeyBufferView);
		GpuSort<Float3>::ApplyPermutation(computeShaders.computeType, sortPermutationBufferView, positionBufferView, sortedPositionBufferView, computeShaders.sessionID);
		Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType, computeShaders.sessionID);

        // Compute density texture3d:
		Uint3 threadCount(densityTexture.GetWidth(), densityTexture.GetHeight(), densityTexture.GetDepth());
		ShaderProperties shaderProperties = Compute::RecordComputeShader(computeShaders.computeType, computeShaders.densityTexture3dComputeShader, threadCount, computeShaders.sessionID);
		shaderProperties.SetValue("CallValues", "particleCount", (int)positionBufferView.GetCount());
		shaderProperties.SetValue("CallValues", "boundsMin", fluidBounds.localBounds.GetMin());
		shaderProperties.SetValue("CallValues", "boundsMax", fluidBounds.localBounds.GetMax());
		shaderProperties.SetValue("CallValues", "boundsRotation", fluidBounds.GetRotation4x4());
		shaderProperties.SetTexture("densityTexture", densityTexture);
		shaderProperties.SetBuffer("cellKeyBuffer", cellKeyBufferView.GetBuffer());
		shaderProperties.SetBuffer("startIndexBuffer", startIndexBufferView.GetBuffer());
		shaderProperties.SetBuffer("positionBuffer", sortedPositionBufferView.GetBuffer());
	}

	void SphFluid3dGpuSolver::ComputeOpticalDepthTexture3d(ComputeShaders& computeShaders, TripleData& tripleData, uint32_t dataIndex)
	{
		Texture3d& densityTexture = tripleData.densityTexture3d[dataIndex];
		Texture3d& opticalDepthTexture = tripleData.opticalDepthTexture3d[dataIndex];
		const RotatedBounds& fluidBounds = tripleData.fluidBounds[dataIndex];
		const RotatedBounds& lightBounds = tripleData.opticalDepthBounds[dataIndex];

		Uint3 threadCount(opticalDepthTexture.GetWidth(), opticalDepthTexture.GetHeight(), 1);
		ShaderProperties shaderProperties = Compute::RecordComputeShader(computeShaders.computeType, computeShaders.opticalDepthTexture3dComputeShader, threadCount, computeShaders.sessionID);
		shaderProperties.SetValue("CallValues", "lightBoundsMin", lightBounds.localBounds.GetMin());
		shaderProperties.SetValue("CallValues", "lightBoundsMax", lightBounds.localBounds.GetMax());
		shaderProperties.SetValue("CallValues", "lightToSimulationRotation", lightBounds.GetRotation4x4());
		shaderProperties.SetValue("CallValues", "fluidBoundsMin", fluidBounds.localBounds.GetMin());
		shaderProperties.SetValue("CallValues", "fluidBoundsMax", fluidBounds.localBounds.GetMax());
		shaderProperties.SetValue("CallValues", "simulationToFluidRotation", fluidBounds.GetRotation4x4().Inverse());
		shaderProperties.SetValue("CallValues", "extinctionCoefficient", tripleData.extinctionCoefficients[dataIndex]);
		shaderProperties.SetValue("CallValues", "opticalDepthTextureDepth", static_cast<int>(opticalDepthTexture.GetDepth()));
		shaderProperties.SetTexture("densityTexture", densityTexture);
		shaderProperties.SetTexture("opticalDepthTexture", opticalDepthTexture);
	}
}