#include "sphFluid2dGpuSolver.h"
#include "logger.h"



namespace fluidDynamics
{
	// Data struct:
	SphFluid2dGpuSolver::Data::Data()
	{
		std::filesystem::path directoryPath = (std::filesystem::path(PROJECT_SHADERS_DIR) / "bin").make_preferred();
		resetComputeShader = ComputeShader("resetData2d", directoryPath / "resetData2d.comp.spv");
		resetProperties = ShaderProperties(resetComputeShader);
	}
	int SphFluid2dGpuSolver::Data::ParticleCount()
	{
		return positionBuffer.IsValid() ? positionBuffer.GetCount() : 0;
	}
	void SphFluid2dGpuSolver::Data::Reallocate(int particleCount, float initialDistributionRadius, ComputeType computeType)
	{
		int hashGridSize = math::NextPrimeAbove(2 * particleCount);

		if (particleCount != ParticleCount())
		{
			// Reallocate:
			cellKeyBuffer = BufferTyped<uint32_t>((uint32_t)particleCount, "cellKeyBuffer", BufferUsage::storage);
			startIndexBuffer = BufferTyped<uint32_t>((uint32_t)hashGridSize, "startIndexBuffer", BufferUsage::storage);
			sortPermutationBuffer = BufferTyped<uint32_t>((uint32_t)particleCount, "sortPermutationBuffer", BufferUsage::storage);
			positionBuffer = BufferTyped<Float2>((uint32_t)particleCount, "positionBuffer", BufferUsage::storage);
			velocityBuffer = BufferTyped<Float2>((uint32_t)particleCount, "velocityBuffer", BufferUsage::storage);
			densityBuffer = BufferTyped<float>((uint32_t)particleCount, "densityBuffer", BufferUsage::storage);
			normalBuffer = BufferTyped<Float2>((uint32_t)particleCount, "normalBuffer", BufferUsage::storage);
			curvatureBuffer = BufferTyped<float>((uint32_t)particleCount, "curvatureBuffer", BufferUsage::storage);
			forceDensityBuffer = BufferTyped<Float2>((uint32_t)particleCount, "forceDensityBuffer", BufferUsage::storage);
			tempBuffer0 = BufferTyped<Float2>((uint32_t)particleCount, "tempBuffer0", BufferUsage::storage);
			tempBuffer1 = BufferTyped<Float2>((uint32_t)particleCount, "tempBuffer1", BufferUsage::storage);
			tempBuffer2 = BufferTyped<Float2>((uint32_t)particleCount, "tempBuffer2", BufferUsage::storage);
			tempBuffer3 = BufferTyped<Float2>((uint32_t)particleCount, "tempBuffer3", BufferUsage::storage);
			tempBuffer4 = BufferTyped<Float2>((uint32_t)particleCount, "tempBuffer4", BufferUsage::storage);
			tempBuffer5 = BufferTyped<Float2>((uint32_t)particleCount, "tempBuffer5", BufferUsage::storage);
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
	SphFluid2dGpuSolver::RungeKutta::RungeKutta()
	{
		std::filesystem::path directoryPath = (std::filesystem::path(PROJECT_SHADERS_DIR) / "bin").make_preferred();
		resetComputeShader = ComputeShader("resetRungeKutta2d", directoryPath / "resetRungeKutta2d.comp.spv");
		resetProperties = ShaderProperties(resetComputeShader);
	}
	int SphFluid2dGpuSolver::RungeKutta::ParticleCount()
	{
		return kp1Buffer.IsValid() ? kp1Buffer.GetCount() : 0;
	}
	void SphFluid2dGpuSolver::RungeKutta::Reallocate(int particleCount, ComputeType computeType)
	{
		if (particleCount != ParticleCount())
		{
			// Reallocate:
			kp1Buffer = BufferTyped<Float2>((uint32_t)particleCount, "kp1Buffer", BufferUsage::storage);
			kv1Buffer = BufferTyped<Float2>((uint32_t)particleCount, "kv1Buffer", BufferUsage::storage);
			tempPositionBuffer = BufferTyped<Float2>((uint32_t)particleCount, "tempPositionBuffer", BufferUsage::storage);
			tempVelocityBuffer = BufferTyped<Float2>((uint32_t)particleCount, "tempVelocityBuffer", BufferUsage::storage);
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
	SphFluid2dGpuSolver::ComputeShaders::ComputeShaders()
	{
		computeType = ComputeType::preRender;

		// Load compute shaders:
		std::filesystem::path directoryPath = (std::filesystem::path(PROJECT_SHADERS_DIR) / "bin").make_preferred();
		cellKeysComputeShader = ComputeShader("cellKeys2d", directoryPath / "cellKeys2d.comp.spv");
		startIndicesResetComputeShader = ComputeShader("startIndicesReset2d", directoryPath / "startIndicesReset2d.comp.spv");
		startIndicesComputeShader = ComputeShader("startIndices2d", directoryPath / "startIndices2d.comp.spv");
		densityComputeShader = ComputeShader("density2d", directoryPath / "density2d.comp.spv");
		normalAndCurvatureComputeShader = ComputeShader("normalAndCurvature2d", directoryPath / "normalAndCurvature2d.comp.spv");
		forceDensityComputeShader = ComputeShader("forceDensity2d", directoryPath / "forceDensity2d.comp.spv");
		rungeKutta2Step1ComputeShader = ComputeShader("rungeKutta2Step1_2d", directoryPath / "rungeKutta2Step1_2d.comp.spv");
		rungeKutta2Step2ComputeShader = ComputeShader("rungeKutta2Step2_2d", directoryPath / "rungeKutta2Step2_2d.comp.spv");
		boundaryCollisionsComputeShader = ComputeShader("boundaryCollisions2d", directoryPath / "boundaryCollisions2d.comp.spv");

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
	void SphFluid2dGpuSolver::ComputeShaders::SetUseHashGridOptimization(bool useHashGridOptimization)
	{
        for (size_t i = 0; i < rungeKuttaStepCount; i++)
        {
            densityProperties[i].SetValue("Values", "useHashGridOptimization", (int)useHashGridOptimization);
            normalAndCurvatureProperties[i].SetValue("Values", "useHashGridOptimization", (int)useHashGridOptimization);
            forceDensityProperties[i].SetValue("Values", "useHashGridOptimization", (int)useHashGridOptimization);
        }
	}
	void SphFluid2dGpuSolver::ComputeShaders::SetEffectRadius(float effectRadius)
	{
        for (size_t i = 0; i < rungeKuttaStepCount; i++)
        {
            cellKeysProperties[i].SetValue("Values", "effectRadius", effectRadius);
            densityProperties[i].SetValue("Values", "effectRadius", effectRadius);
            normalAndCurvatureProperties[i].SetValue("Values", "effectRadius", effectRadius);
            forceDensityProperties[i].SetValue("Values", "effectRadius", effectRadius);
        }
	}
	void SphFluid2dGpuSolver::ComputeShaders::SetHashGridSize(int hashGridSize)
	{
        for (size_t i = 0; i < rungeKuttaStepCount; i++)
        {
            cellKeysProperties[i].SetValue("Values", "hashGridSize", hashGridSize);
            densityProperties[i].SetValue("Values", "hashGridSize", hashGridSize);
            normalAndCurvatureProperties[i].SetValue("Values", "hashGridSize", hashGridSize);
            forceDensityProperties[i].SetValue("Values", "hashGridSize", hashGridSize);
        }
	}
	void SphFluid2dGpuSolver::ComputeShaders::SetMass(float mass)
	{
        for (size_t i = 0; i < rungeKuttaStepCount; i++)
        {
            densityProperties[i].SetValue("Values", "mass", mass);
            normalAndCurvatureProperties[i].SetValue("Values", "mass", mass);
            forceDensityProperties[i].SetValue("Values", "mass", mass);
        }
	}
	void SphFluid2dGpuSolver::ComputeShaders::SetViscosity(float viscosity)
	{
        for (size_t i = 0; i < rungeKuttaStepCount; i++)
			forceDensityProperties[i].SetValue("Values", "viscosity", viscosity);
	}
	void SphFluid2dGpuSolver::ComputeShaders::SetSurfaceTension(float surfaceTension)
	{
        for (size_t i = 0; i < rungeKuttaStepCount; i++)
            forceDensityProperties[i].SetValue("Values", "surfaceTension", surfaceTension);
	}
	void SphFluid2dGpuSolver::ComputeShaders::SetCollisionDampening(float collisionDampening)
	{
		boundaryCollisionsProperties.SetValue("Values", "collisionDampening", collisionDampening);
	}
	void SphFluid2dGpuSolver::ComputeShaders::SetTargetDensity(float targetDensity)
	{
        for (size_t i = 0; i < rungeKuttaStepCount; i++)
            forceDensityProperties[i].SetValue("Values", "targetDensity", targetDensity);
	}
	void SphFluid2dGpuSolver::ComputeShaders::SetPressureMultiplier(float pressureMultiplier)
	{
        for (size_t i = 0; i < rungeKuttaStepCount; i++)
			forceDensityProperties[i].SetValue("Values", "pressureMultiplier", pressureMultiplier);
	}
	void SphFluid2dGpuSolver::ComputeShaders::SetGravity(float gravity)
	{
        for (size_t i = 0; i < rungeKuttaStepCount; i++)
			forceDensityProperties[i].SetValue("Values", "gravity", gravity);
	}
	void SphFluid2dGpuSolver::ComputeShaders::SetMaxVelocity(float maxVelocity)
	{
        for (size_t i = 0; i < rungeKuttaStepCount; i++)
            rungeKutta2StepProperties[i].SetValue("Values", "maxVelocity", maxVelocity);
	}
	void SphFluid2dGpuSolver::ComputeShaders::SetFluidBounds(const Bounds& bounds)
	{
		boundaryCollisionsProperties.SetValue("Values", "min", bounds.GetMin());
		boundaryCollisionsProperties.SetValue("Values", "max", bounds.GetMax());
	}
	void SphFluid2dGpuSolver::ComputeShaders::SetAttractorRadius(float attractorRadius)
	{
        for (size_t i = 0; i < rungeKuttaStepCount; i++)
			forceDensityProperties[i].SetValue("Values", "attractorRadius", attractorRadius);
	}
	void SphFluid2dGpuSolver::ComputeShaders::SetAttractorStrength(float attractorStrength)
	{
        for (size_t i = 0; i < rungeKuttaStepCount; i++)
			forceDensityProperties[i].SetValue("Values", "attractorStrength", attractorStrength);
	}
	void SphFluid2dGpuSolver::ComputeShaders::SetAttractorState(int attractorState)
	{
        for (size_t i = 0; i < rungeKuttaStepCount; i++)
			forceDensityProperties[i].SetValue("Values", "attractorState", attractorState);
	}
	void SphFluid2dGpuSolver::ComputeShaders::SetAttractorPoint(const Float2& attractorPoint)
	{
        for (size_t i = 0; i < rungeKuttaStepCount; i++)
			forceDensityProperties[i].SetValue("Values", "attractorPoint", attractorPoint);
	}




	// Public methods:
	// Ordinary Differential Equation Solvers:
	void SphFluid2dGpuSolver::TimeStepRungeKutta2(float dt, const Settings& settings, Data& data, ComputeShaders& computeShaders, const Attractor& attractor, RungeKutta& rungeKutta)
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
			GpuSort<Float2>::ApplyPermutation(computeShaders.computeType, data.sortPermutationBuffer.GetBufferView(), data.positionBuffer.GetBufferView(), data.tempBuffer0.GetBufferView());
			GpuSort<Float2>::ApplyPermutation(computeShaders.computeType, data.sortPermutationBuffer.GetBufferView(), data.velocityBuffer.GetBufferView(), data.tempBuffer1.GetBufferView());
			Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType);
		}

		// First Runge-Kutta step:
		BufferView<Float2>& currentPositionBufferView = settings.useHashGridOptimization ? data.tempBuffer0.GetBufferView() : data.positionBuffer.GetBufferView();
		BufferView<Float2>& currentVelocityBufferView = settings.useHashGridOptimization ? data.tempBuffer1.GetBufferView() : data.velocityBuffer.GetBufferView();
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
		BufferView<Float2>* pTempPositionBufferView = &rungeKutta.tempPositionBuffer.GetBufferView();
		BufferView<Float2>* pTempVelocityBufferView = &rungeKutta.tempVelocityBuffer.GetBufferView();
		BufferView<Float2>* pKp1BufferView = &rungeKutta.kp1Buffer.GetBufferView();
		BufferView<Float2>* pKv1BufferView = &rungeKutta.kv1Buffer.GetBufferView();
		if (settings.useHashGridOptimization)
		{
			ComputeCellKeys(computeShaders, data.cellKeyBuffer.GetBufferView(), rungeKutta.tempPositionBuffer.GetBufferView(), statePropertiesIndex);
			Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType);

			GpuSort<uint32_t>::SortPermutation(computeShaders.computeType, data.cellKeyBuffer.GetBufferView(), data.sortPermutationBuffer.GetBufferView());
			Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType);

			ComputeStartIndices(computeShaders, data.startIndexBuffer.GetBufferView(), data.cellKeyBuffer.GetBufferView(), statePropertiesIndex);
			GpuSort<Float2>::ApplyPermutation(computeShaders.computeType, data.sortPermutationBuffer.GetBufferView(), rungeKutta.tempPositionBuffer.GetBufferView(), data.tempBuffer2.GetBufferView());
			GpuSort<Float2>::ApplyPermutation(computeShaders.computeType, data.sortPermutationBuffer.GetBufferView(), rungeKutta.tempVelocityBuffer.GetBufferView(), data.tempBuffer3.GetBufferView());
			GpuSort<Float2>::ApplyPermutation(computeShaders.computeType, data.sortPermutationBuffer.GetBufferView(), rungeKutta.kp1Buffer.GetBufferView(), data.tempBuffer4.GetBufferView());
			GpuSort<Float2>::ApplyPermutation(computeShaders.computeType, data.sortPermutationBuffer.GetBufferView(), rungeKutta.kv1Buffer.GetBufferView(), data.tempBuffer5.GetBufferView());
			GpuSort<Float2>::ApplyPermutation(computeShaders.computeType, data.sortPermutationBuffer.GetBufferView(), currentPositionBufferView, data.positionBuffer.GetBufferView());
			GpuSort<Float2>::ApplyPermutation(computeShaders.computeType, data.sortPermutationBuffer.GetBufferView(), currentVelocityBufferView, data.velocityBuffer.GetBufferView());
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
	void SphFluid2dGpuSolver::ComputeCellKeys(ComputeShaders& computeShaders, const BufferView<uint32_t>& cellKeyBufferView, const BufferView<Float2>& positionBufferView, size_t propertiesIndex)
	{
		Uint3 threadCount(cellKeyBufferView.GetCount(), 1, 1);
		ShaderProperties& shaderProperties = computeShaders.cellKeysProperties[propertiesIndex];
		shaderProperties.SetBuffer("cellKeyBuffer", cellKeyBufferView.GetBuffer());
		shaderProperties.SetBuffer("positionBuffer", positionBufferView.GetBuffer());
		Compute::RecordComputeShader(computeShaders.computeType, computeShaders.cellKeysComputeShader, shaderProperties, threadCount);
	}
	void SphFluid2dGpuSolver::ComputeStartIndices(ComputeShaders& computeShaders, const BufferView<uint32_t>& startIndexBufferView, const BufferView<uint32_t>& cellKeyBufferView, size_t propertiesIndex)
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
	void SphFluid2dGpuSolver::ComputeDensities(ComputeShaders& computeShaders, const BufferView<float>& densityBufferView, const BufferView<Float2>& positionBufferView, const BufferView<uint32_t>& startIndexBufferView, const BufferView<uint32_t>& cellKeyBufferView, size_t propertiesIndex)
	{
		Uint3 threadCount(densityBufferView.GetCount(), 1, 1);
		ShaderProperties& shaderProperties = computeShaders.densityProperties[propertiesIndex];
		shaderProperties.SetBuffer("densityBuffer", densityBufferView.GetBuffer());
		shaderProperties.SetBuffer("positionBuffer", positionBufferView.GetBuffer());
		shaderProperties.SetBuffer("startIndexBuffer", startIndexBufferView.GetBuffer());
		shaderProperties.SetBuffer("cellKeyBuffer", cellKeyBufferView.GetBuffer());
		Compute::RecordComputeShader(computeShaders.computeType, computeShaders.densityComputeShader, shaderProperties, threadCount);
	}
	void SphFluid2dGpuSolver::ComputeNormalsAndCurvatures(ComputeShaders& computeShaders, const BufferView<Float2>& normalBufferView, const BufferView<float>& curvatureBufferView, const BufferView<float>& densityBufferView, const BufferView<Float2>& positionBufferView, const BufferView<uint32_t>& startIndexBufferView, const BufferView<uint32_t>& cellKeyBufferView, size_t propertiesIndex)
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
	void SphFluid2dGpuSolver::ComputeForceDensities(ComputeShaders& computeShaders, const BufferView<Float2>& forceDensityBufferView, const BufferView<float>& densityBufferView, const BufferView<Float2>& positionBufferView, const BufferView<Float2>& velocityBufferView, const BufferView<Float2>& normalBufferView, const BufferView<float>& curvatureBufferView, const BufferView<uint32_t>& startIndexBufferView, const BufferView<uint32_t>& cellKeyBufferView, size_t propertiesIndex)
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
	void SphFluid2dGpuSolver::ComputeRungeKutta2Step1(ComputeShaders& computeShaders, const BufferView<Float2>& forceDensityBufferView, const BufferView<float>& densityBufferView, const BufferView<Float2>& positionBufferView, const BufferView<Float2>& velocityBufferView, const BufferView<Float2>& kp1BufferView, const BufferView<Float2>& kv1BufferView, const BufferView<Float2>& tempPositionBufferView, const BufferView<Float2>& tempVelocityBufferView)
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
	void SphFluid2dGpuSolver::ComputeRungeKutta2Step2(ComputeShaders& computeShaders, const BufferView<Float2>& forceDensityBufferView, const BufferView<float>& densityBufferView, const BufferView<Float2>& kp1BufferView, const BufferView<Float2>& kv1BufferView, const BufferView<Float2>& tempVelocityBufferView, const BufferView<Float2>& positionBufferView, const BufferView<Float2>& velocityBufferView)
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
	void SphFluid2dGpuSolver::ComputeBoundaryCollisions(ComputeShaders& computeShaders, const BufferView<Float2>& positionBufferView, const BufferView<Float2>& velocityBufferView)
	{
		Uint3 threadCount(positionBufferView.GetCount(), 1, 1);
		ShaderProperties& shaderProperties = computeShaders.boundaryCollisionsProperties;
		shaderProperties.SetBuffer("positionBuffer", positionBufferView.GetBuffer());
		shaderProperties.SetBuffer("velocityBuffer", velocityBufferView.GetBuffer());
		Compute::RecordComputeShader(computeShaders.computeType, computeShaders.boundaryCollisionsComputeShader, shaderProperties, threadCount);
	}
}