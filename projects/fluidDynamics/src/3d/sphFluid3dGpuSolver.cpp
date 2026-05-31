#include "sphFluid3dGpuSolver.h"
#include "logger.h"



namespace fluidDynamics
{
	// Data struct:
	SphFluid3dGpuSolver::Data::Data()
	{
		std::filesystem::path directoryPath = (std::filesystem::path(PROJECT_SHADERS_DIR) / "bin").make_preferred();
		resetComputeShader = ComputeShader("resetData3d", directoryPath / "resetData3d.comp.spv");
		resetProperties = ShaderProperties(resetComputeShader);
	}
	int SphFluid3dGpuSolver::Data::ParticleCount()
	{
		return positionBuffer.IsValid() ? positionBuffer.GetCount() : 0;
	}
	void SphFluid3dGpuSolver::Data::Reallocate(int particleCount, float initialDistributionRadius, ComputeType computeType)
	{
		int hashGridSize = math::NextPrimeAbove(2 * particleCount);

		if (particleCount != ParticleCount())
		{
			// Reallocate:
			cellKeyBuffer = BufferTyped<uint32_t>((uint32_t)particleCount, "cellKeyBuffer", BufferUsage::storage);
			startIndexBuffer = BufferTyped<uint32_t>((uint32_t)hashGridSize, "startIndexBuffer", BufferUsage::storage);
			sortPermutationBuffer = BufferTyped<uint32_t>((uint32_t)particleCount, "sortPermutationBuffer", BufferUsage::storage);
			positionBuffer = BufferTyped<Float3>((uint32_t)particleCount, "positionBuffer", BufferUsage::storage);
			velocityBuffer = BufferTyped<Float3>((uint32_t)particleCount, "velocityBuffer", BufferUsage::storage);
			densityBuffer = BufferTyped<float>((uint32_t)particleCount, "densityBuffer", BufferUsage::storage);
			normalBuffer = BufferTyped<Float3>((uint32_t)particleCount, "normalBuffer", BufferUsage::storage);
			curvatureBuffer = BufferTyped<float>((uint32_t)particleCount, "curvatureBuffer", BufferUsage::storage);
			forceDensityBuffer = BufferTyped<Float3>((uint32_t)particleCount, "forceDensityBuffer", BufferUsage::storage);
			tempBuffer0 = BufferTyped<Float3>((uint32_t)particleCount, "tempBuffer0", BufferUsage::storage);
			tempBuffer1 = BufferTyped<Float3>((uint32_t)particleCount, "tempBuffer1", BufferUsage::storage);
			tempBuffer2 = BufferTyped<Float3>((uint32_t)particleCount, "tempBuffer2", BufferUsage::storage);
			tempBuffer3 = BufferTyped<Float3>((uint32_t)particleCount, "tempBuffer3", BufferUsage::storage);
			tempBuffer4 = BufferTyped<Float3>((uint32_t)particleCount, "tempBuffer4", BufferUsage::storage);
			tempBuffer5 = BufferTyped<Float3>((uint32_t)particleCount, "tempBuffer5", BufferUsage::storage);
		}

		// Reset data:
		Uint3 threadCount(positionBuffer.GetCount(), 1, 1);
		resetProperties.SetValue("Values", "hashGridSize", hashGridSize);
		resetProperties.SetValue("Values", "initialDistributionRadius", initialDistributionRadius);
		resetProperties.SetBuffer("cellKeyBuffer", cellKeyBuffer.GetBuffer());
		resetProperties.SetBuffer("startIndexBuffer", startIndexBuffer.GetBuffer());
		resetProperties.SetBuffer("sortPermutationBuffer", sortPermutationBuffer.GetBuffer());
		resetProperties.SetBuffer("positionBuffer", positionBuffer.GetBuffer());
		resetProperties.SetBuffer("velocityBuffer", velocityBuffer.GetBuffer());
		resetProperties.SetBuffer("densityBuffer", densityBuffer.GetBuffer());
		resetProperties.SetBuffer("normalBuffer", normalBuffer.GetBuffer());
		resetProperties.SetBuffer("curvatureBuffer", curvatureBuffer.GetBuffer());
		resetProperties.SetBuffer("forceDensityBuffer", forceDensityBuffer.GetBuffer());
		resetProperties.SetBuffer("tempBuffer0", tempBuffer0.GetBuffer());
		resetProperties.SetBuffer("tempBuffer1", tempBuffer1.GetBuffer());
		resetProperties.SetBuffer("tempBuffer2", tempBuffer2.GetBuffer());
		resetProperties.SetBuffer("tempBuffer3", tempBuffer3.GetBuffer());
		resetProperties.SetBuffer("tempBuffer4", tempBuffer4.GetBuffer());
		resetProperties.SetBuffer("tempBuffer5", tempBuffer5.GetBuffer());
		Compute::RecordComputeShader(computeType, resetComputeShader, resetProperties, threadCount);
	}



	// RungeKutta struct:
	SphFluid3dGpuSolver::RungeKutta::RungeKutta()
	{
		std::filesystem::path directoryPath = (std::filesystem::path(PROJECT_SHADERS_DIR) / "bin").make_preferred();
		resetComputeShader = ComputeShader("resetRungeKutta3d", directoryPath / "resetRungeKutta3d.comp.spv");
		resetProperties = ShaderProperties(resetComputeShader);
	}
	int SphFluid3dGpuSolver::RungeKutta::ParticleCount()
	{
		return kp1Buffer.IsValid() ? kp1Buffer.GetCount() : 0;
	}
	void SphFluid3dGpuSolver::RungeKutta::Reallocate(int particleCount, ComputeType computeType)
	{
		if (particleCount != ParticleCount())
		{
			// Reallocate:
			kp1Buffer = BufferTyped<Float3>((uint32_t)particleCount, "kp1Buffer", BufferUsage::storage);
			kv1Buffer = BufferTyped<Float3>((uint32_t)particleCount, "kv1Buffer", BufferUsage::storage);
			tempPositionBuffer = BufferTyped<Float3>((uint32_t)particleCount, "tempPositionBuffer", BufferUsage::storage);
			tempVelocityBuffer = BufferTyped<Float3>((uint32_t)particleCount, "tempVelocityBuffer", BufferUsage::storage);
		}

		// Reset data:
		Uint3 threadCount(kp1Buffer.GetCount(), 1, 1);
		resetProperties.SetBuffer("kp1Buffer", kp1Buffer.GetBuffer());
		resetProperties.SetBuffer("kv1Buffer", kv1Buffer.GetBuffer());
		resetProperties.SetBuffer("tempPositionBuffer", tempPositionBuffer.GetBuffer());
		resetProperties.SetBuffer("tempVelocityBuffer", tempVelocityBuffer.GetBuffer());
		Compute::RecordComputeShader(computeType, resetComputeShader, resetProperties, threadCount);
	}



	// ComputeShaders struct:
	SphFluid3dGpuSolver::ComputeShaders::ComputeShaders()
	{
		computeType = ComputeType::preRender;

		// Load compute shaders:
		std::filesystem::path directoryPath = (std::filesystem::path(PROJECT_SHADERS_DIR) / "bin").make_preferred();
		cellKeysComputeShader = ComputeShader("cellKeys3d", directoryPath / "cellKeys3d.comp.spv");
		startIndicesResetComputeShader = ComputeShader("startIndicesReset3d", directoryPath / "startIndicesReset3d.comp.spv");
		startIndicesComputeShader = ComputeShader("startIndices3d", directoryPath / "startIndices3d.comp.spv");
		densityComputeShader = ComputeShader("density3d", directoryPath / "density3d.comp.spv");
		normalAndCurvatureComputeShader = ComputeShader("normalAndCurvature3d", directoryPath / "normalAndCurvature3d.comp.spv");
		forceDensityComputeShader = ComputeShader("forceDensity3d", directoryPath / "forceDensity3d.comp.spv");
		rungeKutta2Step1ComputeShader = ComputeShader("rungeKutta2Step1_3d", directoryPath / "rungeKutta2Step1_3d.comp.spv");
		rungeKutta2Step2ComputeShader = ComputeShader("rungeKutta2Step2_3d", directoryPath / "rungeKutta2Step2_3d.comp.spv");
		boundaryCollisionsComputeShader = ComputeShader("boundaryCollisions3d", directoryPath / "boundaryCollisions3d.comp.spv");

		// Initialize shader properties:
        for (size_t i = 0; i < rungeKuttaStepCount; i++)
        {
            cellKeysProperties[i] = ShaderProperties(cellKeysComputeShader);
            startIndicesResetProperties[i] = ShaderProperties(startIndicesResetComputeShader);
            startIndicesProperties[i] = ShaderProperties(startIndicesComputeShader);
            densityProperties[i] = ShaderProperties(densityComputeShader);
            normalAndCurvatureProperties[i] = ShaderProperties(normalAndCurvatureComputeShader);
            forceDensityProperties[i] = ShaderProperties(forceDensityComputeShader);
        }
		rungeKutta2StepProperties[0] = ShaderProperties(rungeKutta2Step1ComputeShader);
		rungeKutta2StepProperties[1] = ShaderProperties(rungeKutta2Step2ComputeShader);
		boundaryCollisionsProperties = ShaderProperties(boundaryCollisionsComputeShader);
	}
	void SphFluid3dGpuSolver::ComputeShaders::SetUseHashGridOptimization(bool useHashGridOptimization)
	{
        for (size_t i = 0; i < rungeKuttaStepCount; i++)
        {
            densityProperties[i].SetValue("Values", "useHashGridOptimization", (int)useHashGridOptimization);
            normalAndCurvatureProperties[i].SetValue("Values", "useHashGridOptimization", (int)useHashGridOptimization);
            forceDensityProperties[i].SetValue("Values", "useHashGridOptimization", (int)useHashGridOptimization);
        }
	}
	void SphFluid3dGpuSolver::ComputeShaders::SetEffectRadius(float effectRadius)
	{
        for (size_t i = 0; i < rungeKuttaStepCount; i++)
        {
            cellKeysProperties[i].SetValue("Values", "effectRadius", effectRadius);
            densityProperties[i].SetValue("Values", "effectRadius", effectRadius);
            normalAndCurvatureProperties[i].SetValue("Values", "effectRadius", effectRadius);
            forceDensityProperties[i].SetValue("Values", "effectRadius", effectRadius);
        }
	}
	void SphFluid3dGpuSolver::ComputeShaders::SetHashGridSize(int hashGridSize)
	{
        for (size_t i = 0; i < rungeKuttaStepCount; i++)
        {
            cellKeysProperties[i].SetValue("Values", "hashGridSize", hashGridSize);
            densityProperties[i].SetValue("Values", "hashGridSize", hashGridSize);
            normalAndCurvatureProperties[i].SetValue("Values", "hashGridSize", hashGridSize);
            forceDensityProperties[i].SetValue("Values", "hashGridSize", hashGridSize);
        }
	}
	void SphFluid3dGpuSolver::ComputeShaders::SetMass(float mass)
	{
        for (size_t i = 0; i < rungeKuttaStepCount; i++)
        {
            densityProperties[i].SetValue("Values", "mass", mass);
            normalAndCurvatureProperties[i].SetValue("Values", "mass", mass);
            forceDensityProperties[i].SetValue("Values", "mass", mass);
        }
	}
	void SphFluid3dGpuSolver::ComputeShaders::SetViscosity(float viscosity)
	{
        for (size_t i = 0; i < rungeKuttaStepCount; i++)
			forceDensityProperties[i].SetValue("Values", "viscosity", viscosity);
	}
	void SphFluid3dGpuSolver::ComputeShaders::SetSurfaceTension(float surfaceTension)
	{
        for (size_t i = 0; i < rungeKuttaStepCount; i++)
            forceDensityProperties[i].SetValue("Values", "surfaceTension", surfaceTension);
	}
	void SphFluid3dGpuSolver::ComputeShaders::SetCollisionDampening(float collisionDampening)
	{
		boundaryCollisionsProperties.SetValue("Values", "collisionDampening", collisionDampening);
	}
	void SphFluid3dGpuSolver::ComputeShaders::SetTargetDensity(float targetDensity)
	{
        for (size_t i = 0; i < rungeKuttaStepCount; i++)
            forceDensityProperties[i].SetValue("Values", "targetDensity", targetDensity);
	}
	void SphFluid3dGpuSolver::ComputeShaders::SetPressureMultiplier(float pressureMultiplier)
	{
        for (size_t i = 0; i < rungeKuttaStepCount; i++)
			forceDensityProperties[i].SetValue("Values", "pressureMultiplier", pressureMultiplier);
	}
	void SphFluid3dGpuSolver::ComputeShaders::SetGravity(float gravity)
	{
        for (size_t i = 0; i < rungeKuttaStepCount; i++)
			forceDensityProperties[i].SetValue("Values", "gravity", gravity);
	}
	void SphFluid3dGpuSolver::ComputeShaders::SetMaxVelocity(float maxVelocity)
	{
        for (size_t i = 0; i < rungeKuttaStepCount; i++)
            rungeKutta2StepProperties[i].SetValue("Values", "maxVelocity", maxVelocity);
	}
	void SphFluid3dGpuSolver::ComputeShaders::SetFluidBounds(const Bounds& bounds)
	{
		boundaryCollisionsProperties.SetValue("Values", "min", bounds.GetMin());
		boundaryCollisionsProperties.SetValue("Values", "max", bounds.GetMax());
	}
	void SphFluid3dGpuSolver::ComputeShaders::SetAttractorRadius(float attractorRadius)
	{
        for (size_t i = 0; i < rungeKuttaStepCount; i++)
			forceDensityProperties[i].SetValue("Values", "attractorRadius", attractorRadius);
	}
	void SphFluid3dGpuSolver::ComputeShaders::SetAttractorStrength(float attractorStrength)
	{
        for (size_t i = 0; i < rungeKuttaStepCount; i++)
			forceDensityProperties[i].SetValue("Values", "attractorStrength", attractorStrength);
	}
	void SphFluid3dGpuSolver::ComputeShaders::SetAttractorState(int attractorState)
	{
        for (size_t i = 0; i < rungeKuttaStepCount; i++)
			forceDensityProperties[i].SetValue("Values", "attractorState", attractorState);
	}
	void SphFluid3dGpuSolver::ComputeShaders::SetAttractorPoint(const Float3& attractorPoint)
	{
        for (size_t i = 0; i < rungeKuttaStepCount; i++)
			forceDensityProperties[i].SetValue("Values", "attractorPoint", attractorPoint);
	}




	// Public methods:
	// Ordinary Differential Equation Solvers:
	void SphFluid3dGpuSolver::TimeStepRungeKutta2(float dt, const Settings& settings, Data& data, ComputeShaders& computeShaders, RungeKutta& rungeKutta)
	{
        size_t statePropertiesIndex = 0;
        for (size_t i = 0; i < ComputeShaders::rungeKuttaStepCount; i++)
            computeShaders.rungeKutta2StepProperties[i].SetValue("Values", "dt", dt);

		// Update hash grid for fast nearest neighbor particle look up:
		Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType);
		if (settings.useHashGridOptimization)
		{
			ComputeCellKeys(computeShaders, data.cellKeyBuffer.GetBufferView(), data.positionBuffer.GetBufferView(), statePropertiesIndex);
			Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType);

			GpuSort<uint32_t>::SortPermutation(computeShaders.computeType, data.cellKeyBuffer.GetBufferView(), data.sortPermutationBuffer.GetBufferView());
			Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType);

			ComputeStartIndices(computeShaders, data.startIndexBuffer.GetBufferView(), data.cellKeyBuffer.GetBufferView(), statePropertiesIndex);
			GpuSort<Float3>::ApplyPermutation(computeShaders.computeType, data.sortPermutationBuffer.GetBufferView(), data.positionBuffer.GetBufferView(), data.tempBuffer0.GetBufferView());
			GpuSort<Float3>::ApplyPermutation(computeShaders.computeType, data.sortPermutationBuffer.GetBufferView(), data.velocityBuffer.GetBufferView(), data.tempBuffer1.GetBufferView());
			Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType);
		}

		// First Runge-Kutta step:
		BufferView<Float3>& currentPositionBufferView = settings.useHashGridOptimization ? data.tempBuffer0.GetBufferView() : data.positionBuffer.GetBufferView();
		BufferView<Float3>& currentVelocityBufferView = settings.useHashGridOptimization ? data.tempBuffer1.GetBufferView() : data.velocityBuffer.GetBufferView();
		ComputeDensities(computeShaders, data.densityBuffer.GetBufferView(), currentPositionBufferView, data.startIndexBuffer.GetBufferView(), data.cellKeyBuffer.GetBufferView(), statePropertiesIndex);
		Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType);
		ComputeNormalsAndCurvatures(computeShaders, data.normalBuffer.GetBufferView(), data.curvatureBuffer.GetBufferView(), data.densityBuffer.GetBufferView(), currentPositionBufferView, data.startIndexBuffer.GetBufferView(), data.cellKeyBuffer.GetBufferView(), statePropertiesIndex);
		Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType);
		ComputeForceDensities(computeShaders, data.forceDensityBuffer.GetBufferView(), data.densityBuffer.GetBufferView(), currentPositionBufferView, currentVelocityBufferView, data.normalBuffer.GetBufferView(), data.curvatureBuffer.GetBufferView(), data.startIndexBuffer.GetBufferView(), data.cellKeyBuffer.GetBufferView(), statePropertiesIndex);
		Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType);
		ComputeRungeKutta2Step1(computeShaders, data.forceDensityBuffer.GetBufferView(), data.densityBuffer.GetBufferView(), currentPositionBufferView, currentVelocityBufferView, rungeKutta.kp1Buffer.GetBufferView(), rungeKutta.kv1Buffer.GetBufferView(), rungeKutta.tempPositionBuffer.GetBufferView(), rungeKutta.tempVelocityBuffer.GetBufferView());
		Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType);
        statePropertiesIndex++; // first Runge-Kutta step complete, increment stateProperties index for next phase.

		// Update hash grid for fast nearest neighbor particle look up:
		BufferView<Float3>* pTempPositionBufferView = &rungeKutta.tempPositionBuffer.GetBufferView();
		BufferView<Float3>* pTempVelocityBufferView = &rungeKutta.tempVelocityBuffer.GetBufferView();
		BufferView<Float3>* pKp1BufferView = &rungeKutta.kp1Buffer.GetBufferView();
		BufferView<Float3>* pKv1BufferView = &rungeKutta.kv1Buffer.GetBufferView();
		if (settings.useHashGridOptimization)
		{
			ComputeCellKeys(computeShaders, data.cellKeyBuffer.GetBufferView(), rungeKutta.tempPositionBuffer.GetBufferView(), statePropertiesIndex);
			Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType);

			GpuSort<uint32_t>::SortPermutation(computeShaders.computeType, data.cellKeyBuffer.GetBufferView(), data.sortPermutationBuffer.GetBufferView());
			Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType);

			ComputeStartIndices(computeShaders, data.startIndexBuffer.GetBufferView(), data.cellKeyBuffer.GetBufferView(), statePropertiesIndex);
			GpuSort<Float3>::ApplyPermutation(computeShaders.computeType, data.sortPermutationBuffer.GetBufferView(), rungeKutta.tempPositionBuffer.GetBufferView(), data.tempBuffer2.GetBufferView());
			GpuSort<Float3>::ApplyPermutation(computeShaders.computeType, data.sortPermutationBuffer.GetBufferView(), rungeKutta.tempVelocityBuffer.GetBufferView(), data.tempBuffer3.GetBufferView());
			GpuSort<Float3>::ApplyPermutation(computeShaders.computeType, data.sortPermutationBuffer.GetBufferView(), rungeKutta.kp1Buffer.GetBufferView(), data.tempBuffer4.GetBufferView());
			GpuSort<Float3>::ApplyPermutation(computeShaders.computeType, data.sortPermutationBuffer.GetBufferView(), rungeKutta.kv1Buffer.GetBufferView(), data.tempBuffer5.GetBufferView());
			GpuSort<Float3>::ApplyPermutation(computeShaders.computeType, data.sortPermutationBuffer.GetBufferView(), currentPositionBufferView, data.positionBuffer.GetBufferView());
			GpuSort<Float3>::ApplyPermutation(computeShaders.computeType, data.sortPermutationBuffer.GetBufferView(), currentVelocityBufferView, data.velocityBuffer.GetBufferView());
			Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType);

			pTempPositionBufferView = &data.tempBuffer2.GetBufferView();
			pTempVelocityBufferView = &data.tempBuffer3.GetBufferView();
			pKp1BufferView = &data.tempBuffer4.GetBufferView();
			pKv1BufferView = &data.tempBuffer5.GetBufferView();
		}

		// Second Runge-Kutta step:
		ComputeDensities(computeShaders, data.densityBuffer.GetBufferView(), *pTempPositionBufferView, data.startIndexBuffer.GetBufferView(), data.cellKeyBuffer.GetBufferView(), statePropertiesIndex);
		Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType);
		ComputeNormalsAndCurvatures(computeShaders, data.normalBuffer.GetBufferView(), data.curvatureBuffer.GetBufferView(), data.densityBuffer.GetBufferView(), *pTempPositionBufferView, data.startIndexBuffer.GetBufferView(), data.cellKeyBuffer.GetBufferView(), statePropertiesIndex);
		Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType);
		ComputeForceDensities(computeShaders, data.forceDensityBuffer.GetBufferView(), data.densityBuffer.GetBufferView(), *pTempPositionBufferView, *pTempVelocityBufferView, data.normalBuffer.GetBufferView(), data.curvatureBuffer.GetBufferView(), data.startIndexBuffer.GetBufferView(), data.cellKeyBuffer.GetBufferView(), statePropertiesIndex);
		Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType);
		ComputeRungeKutta2Step2(computeShaders, data.forceDensityBuffer.GetBufferView(), data.densityBuffer.GetBufferView(), *pKp1BufferView, *pKv1BufferView, *pTempVelocityBufferView, data.positionBuffer.GetBufferView(), data.velocityBuffer.GetBufferView());
		Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType);

		// Resolve boundary collisions:
		ComputeBoundaryCollisions(computeShaders, data.positionBuffer.GetBufferView(), data.velocityBuffer.GetBufferView());
	}



	// Field computations:
	void SphFluid3dGpuSolver::ComputeCellKeys(ComputeShaders& computeShaders, const BufferView<uint32_t>& cellKeyBufferView, const BufferView<Float3>& positionBufferView, size_t propertiesIndex)
	{
		Uint3 threadCount(cellKeyBufferView.GetCount(), 1, 1);
		ShaderProperties& shaderProperties = computeShaders.cellKeysProperties[propertiesIndex];
		shaderProperties.SetBuffer("cellKeyBuffer", cellKeyBufferView.GetBuffer());
		shaderProperties.SetBuffer("positionBuffer", positionBufferView.GetBuffer());
		Compute::RecordComputeShader(computeShaders.computeType, computeShaders.cellKeysComputeShader, shaderProperties, threadCount);
	}
	void SphFluid3dGpuSolver::ComputeStartIndices(ComputeShaders& computeShaders, const BufferView<uint32_t>& startIndexBufferView, const BufferView<uint32_t>& cellKeyBufferView, size_t propertiesIndex)
	{
		// Reset start index buffer:
		{
			Uint3 threadCount(startIndexBufferView.GetCount(), 1, 1);	// reset all possible start indices.
			ShaderProperties& shaderProperties = computeShaders.startIndicesResetProperties[propertiesIndex];
			shaderProperties.SetBuffer("startIndexBuffer", startIndexBufferView.GetBuffer());
			Compute::RecordComputeShader(computeShaders.computeType, computeShaders.startIndicesResetComputeShader, shaderProperties, threadCount);
		}
		// Compute start indices:
		Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType);
		{
			Uint3 threadCount(cellKeyBufferView.GetCount(), 1, 1);	// start indices only needed for each particle.
			ShaderProperties& shaderProperties = computeShaders.startIndicesProperties[propertiesIndex];
			shaderProperties.SetBuffer("startIndexBuffer", startIndexBufferView.GetBuffer());
			shaderProperties.SetBuffer("cellKeyBuffer", cellKeyBufferView.GetBuffer());
			Compute::RecordComputeShader(computeShaders.computeType, computeShaders.startIndicesComputeShader, shaderProperties, threadCount);
		}
	}
	void SphFluid3dGpuSolver::ComputeDensities(ComputeShaders& computeShaders, const BufferView<float>& densityBufferView, const BufferView<Float3>& positionBufferView, const BufferView<uint32_t>& startIndexBufferView, const BufferView<uint32_t>& cellKeyBufferView, size_t propertiesIndex)
	{
		Uint3 threadCount(densityBufferView.GetCount(), 1, 1);
		ShaderProperties& shaderProperties = computeShaders.densityProperties[propertiesIndex];
		shaderProperties.SetBuffer("densityBuffer", densityBufferView.GetBuffer());
		shaderProperties.SetBuffer("positionBuffer", positionBufferView.GetBuffer());
		shaderProperties.SetBuffer("startIndexBuffer", startIndexBufferView.GetBuffer());
		shaderProperties.SetBuffer("cellKeyBuffer", cellKeyBufferView.GetBuffer());
		Compute::RecordComputeShader(computeShaders.computeType, computeShaders.densityComputeShader, shaderProperties, threadCount);
	}
	void SphFluid3dGpuSolver::ComputeNormalsAndCurvatures(ComputeShaders& computeShaders, const BufferView<Float3>& normalBufferView, const BufferView<float>& curvatureBufferView, const BufferView<float>& densityBufferView, const BufferView<Float3>& positionBufferView, const BufferView<uint32_t>& startIndexBufferView, const BufferView<uint32_t>& cellKeyBufferView, size_t propertiesIndex)
	{
		Uint3 threadCount(normalBufferView.GetCount(), 1, 1);
		ShaderProperties& shaderProperties = computeShaders.normalAndCurvatureProperties[propertiesIndex];
		shaderProperties.SetBuffer("normalBuffer", normalBufferView.GetBuffer());
		shaderProperties.SetBuffer("curvatureBuffer", curvatureBufferView.GetBuffer());
		shaderProperties.SetBuffer("densityBuffer", densityBufferView.GetBuffer());
		shaderProperties.SetBuffer("positionBuffer", positionBufferView.GetBuffer());
		shaderProperties.SetBuffer("startIndexBuffer", startIndexBufferView.GetBuffer());
		shaderProperties.SetBuffer("cellKeyBuffer", cellKeyBufferView.GetBuffer());
		Compute::RecordComputeShader(computeShaders.computeType, computeShaders.normalAndCurvatureComputeShader, shaderProperties, threadCount);
	}
	void SphFluid3dGpuSolver::ComputeForceDensities(ComputeShaders& computeShaders, const BufferView<Float3>& forceDensityBufferView, const BufferView<float>& densityBufferView, const BufferView<Float3>& positionBufferView, const BufferView<Float3>& velocityBufferView, const BufferView<Float3>& normalBufferView, const BufferView<float>& curvatureBufferView, const BufferView<uint32_t>& startIndexBufferView, const BufferView<uint32_t>& cellKeyBufferView, size_t propertiesIndex)
	{
		Uint3 threadCount(forceDensityBufferView.GetCount(), 1, 1);
		ShaderProperties& shaderProperties = computeShaders.forceDensityProperties[propertiesIndex];
		shaderProperties.SetBuffer("cellKeyBuffer", cellKeyBufferView.GetBuffer());
		shaderProperties.SetBuffer("startIndexBuffer", startIndexBufferView.GetBuffer());
		shaderProperties.SetBuffer("positionBuffer", positionBufferView.GetBuffer());
		shaderProperties.SetBuffer("velocityBuffer", velocityBufferView.GetBuffer());
		shaderProperties.SetBuffer("densityBuffer", densityBufferView.GetBuffer());
		shaderProperties.SetBuffer("normalBuffer", normalBufferView.GetBuffer());
		shaderProperties.SetBuffer("curvatureBuffer", curvatureBufferView.GetBuffer());
		shaderProperties.SetBuffer("forceDensityBuffer", forceDensityBufferView.GetBuffer());
		Compute::RecordComputeShader(computeShaders.computeType, computeShaders.forceDensityComputeShader, shaderProperties, threadCount);
	}
	void SphFluid3dGpuSolver::ComputeRungeKutta2Step1(ComputeShaders& computeShaders, const BufferView<Float3>& forceDensityBufferView, const BufferView<float>& densityBufferView, const BufferView<Float3>& positionBufferView, const BufferView<Float3>& velocityBufferView, const BufferView<Float3>& kp1BufferView, const BufferView<Float3>& kv1BufferView, const BufferView<Float3>& tempPositionBufferView, const BufferView<Float3>& tempVelocityBufferView)
	{
		Uint3 threadCount(positionBufferView.GetCount(), 1, 1);
		ShaderProperties& shaderProperties = computeShaders.rungeKutta2StepProperties[0];
		shaderProperties.SetBuffer("forceDensityBuffer", forceDensityBufferView.GetBuffer());
		shaderProperties.SetBuffer("densityBuffer", densityBufferView.GetBuffer());
		shaderProperties.SetBuffer("positionBuffer", positionBufferView.GetBuffer());
		shaderProperties.SetBuffer("velocityBuffer", velocityBufferView.GetBuffer());
		shaderProperties.SetBuffer("kp1Buffer", kp1BufferView.GetBuffer());
		shaderProperties.SetBuffer("kv1Buffer", kv1BufferView.GetBuffer());
		shaderProperties.SetBuffer("tempPositionBuffer", tempPositionBufferView.GetBuffer());
		shaderProperties.SetBuffer("tempVelocityBuffer", tempVelocityBufferView.GetBuffer());
		Compute::RecordComputeShader(computeShaders.computeType, computeShaders.rungeKutta2Step1ComputeShader, shaderProperties, threadCount);
	}
	void SphFluid3dGpuSolver::ComputeRungeKutta2Step2(ComputeShaders& computeShaders, const BufferView<Float3>& forceDensityBufferView, const BufferView<float>& densityBufferView, const BufferView<Float3>& kp1BufferView, const BufferView<Float3>& kv1BufferView, const BufferView<Float3>& tempVelocityBufferView, const BufferView<Float3>& positionBufferView, const BufferView<Float3>& velocityBufferView)
	{
		Uint3 threadCount(positionBufferView.GetCount(), 1, 1);
		ShaderProperties& shaderProperties = computeShaders.rungeKutta2StepProperties[1];
		shaderProperties.SetBuffer("forceDensityBuffer", forceDensityBufferView.GetBuffer());
		shaderProperties.SetBuffer("densityBuffer", densityBufferView.GetBuffer());
		shaderProperties.SetBuffer("kp1Buffer", kp1BufferView.GetBuffer());
		shaderProperties.SetBuffer("kv1Buffer", kv1BufferView.GetBuffer());
		shaderProperties.SetBuffer("tempVelocityBuffer", tempVelocityBufferView.GetBuffer());
		shaderProperties.SetBuffer("positionBuffer", positionBufferView.GetBuffer());
		shaderProperties.SetBuffer("velocityBuffer", velocityBufferView.GetBuffer());
		Compute::RecordComputeShader(computeShaders.computeType, computeShaders.rungeKutta2Step2ComputeShader, shaderProperties, threadCount);
	}
	void SphFluid3dGpuSolver::ComputeBoundaryCollisions(ComputeShaders& computeShaders, const BufferView<Float3>& positionBufferView, const BufferView<Float3>& velocityBufferView)
	{
		Uint3 threadCount(positionBufferView.GetCount(), 1, 1);
		ShaderProperties& shaderProperties = computeShaders.boundaryCollisionsProperties;
		shaderProperties.SetBuffer("positionBuffer", positionBufferView.GetBuffer());
		shaderProperties.SetBuffer("velocityBuffer", velocityBufferView.GetBuffer());
		Compute::RecordComputeShader(computeShaders.computeType, computeShaders.boundaryCollisionsComputeShader, shaderProperties, threadCount);
	}
}