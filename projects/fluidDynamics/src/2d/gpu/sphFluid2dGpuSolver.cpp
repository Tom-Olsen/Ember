#include "sphFluid2dGpuSolver.h"
#include "logger.h"



namespace fluidDynamics
{
	// Data struct:
	int SphFluid2dGpuSolver::Data::ParticleCount()
	{
		return positionBuffer.IsValid() ? positionBuffer.GetCount() : 0;
	}
	void SphFluid2dGpuSolver::Data::Reallocate(int particleCount, float initialDistributionRadius, ComputeShaders& computeShaders)
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
		ShaderProperties shaderProperties = Compute::RecordComputeShader(computeShaders.computeType, computeShaders.resetDataComputeShader, threadCount, computeShaders.sessionID);
		shaderProperties.SetValue("CallValues", "hashGridSize", hashGridSize);
		shaderProperties.SetValue("CallValues", "initialDistributionRadius", initialDistributionRadius);
		shaderProperties.SetBuffer("cellKeyBuffer", cellKeyBuffer.GetBuffer());
		shaderProperties.SetBuffer("startIndexBuffer", startIndexBuffer.GetBuffer());
		shaderProperties.SetBuffer("sortPermutationBuffer", sortPermutationBuffer.GetBuffer());
		shaderProperties.SetBuffer("positionBuffer", positionBuffer.GetBuffer());
		shaderProperties.SetBuffer("velocityBuffer", velocityBuffer.GetBuffer());
		shaderProperties.SetBuffer("densityBuffer", densityBuffer.GetBuffer());
		shaderProperties.SetBuffer("normalBuffer", normalBuffer.GetBuffer());
		shaderProperties.SetBuffer("curvatureBuffer", curvatureBuffer.GetBuffer());
		shaderProperties.SetBuffer("forceDensityBuffer", forceDensityBuffer.GetBuffer());
		shaderProperties.SetBuffer("tempBuffer0", tempBuffer0.GetBuffer());
		shaderProperties.SetBuffer("tempBuffer1", tempBuffer1.GetBuffer());
		shaderProperties.SetBuffer("tempBuffer2", tempBuffer2.GetBuffer());
		shaderProperties.SetBuffer("tempBuffer3", tempBuffer3.GetBuffer());
		shaderProperties.SetBuffer("tempBuffer4", tempBuffer4.GetBuffer());
		shaderProperties.SetBuffer("tempBuffer5", tempBuffer5.GetBuffer());
	}



	// RungeKutta struct:
	int SphFluid2dGpuSolver::RungeKutta::ParticleCount()
	{
		return kp1Buffer.IsValid() ? kp1Buffer.GetCount() : 0;
	}
	void SphFluid2dGpuSolver::RungeKutta::Reallocate(int particleCount, ComputeShaders& computeShaders)
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
		ShaderProperties shaderProperties = Compute::RecordComputeShader(computeShaders.computeType, computeShaders.resetRungeKuttaComputeShader, threadCount, computeShaders.sessionID);
		shaderProperties.SetBuffer("kp1Buffer", kp1Buffer.GetBuffer());
		shaderProperties.SetBuffer("kv1Buffer", kv1Buffer.GetBuffer());
		shaderProperties.SetBuffer("tempPositionBuffer", tempPositionBuffer.GetBuffer());
		shaderProperties.SetBuffer("tempVelocityBuffer", tempVelocityBuffer.GetBuffer());
	}



	// ComputeShaders struct:
	SphFluid2dGpuSolver::ComputeShaders::ComputeShaders()
	{
		computeType = ComputeType::physics;
		sessionID = -1;

		// Load compute shaders:
		std::filesystem::path directoryPath = (std::filesystem::path(PROJECT_SHADERS_DIR) / "bin").make_preferred();
		resetDataComputeShader = ComputeShader("resetData2d", directoryPath / "resetData2d.comp.spv");
		resetRungeKuttaComputeShader = ComputeShader("resetRungeKutta2d", directoryPath / "resetRungeKutta2d.comp.spv");
		cellKeysComputeShader = ComputeShader("cellKeys2d", directoryPath / "cellKeys2d.comp.spv");
		startIndicesResetComputeShader = ComputeShader("startIndicesReset2d", directoryPath / "startIndicesReset2d.comp.spv");
		startIndicesComputeShader = ComputeShader("startIndices2d", directoryPath / "startIndices2d.comp.spv");
		densityComputeShader = ComputeShader("density2d", directoryPath / "density2d.comp.spv");
		normalAndCurvatureComputeShader = ComputeShader("normalAndCurvature2d", directoryPath / "normalAndCurvature2d.comp.spv");
		forceDensityComputeShader = ComputeShader("forceDensity2d", directoryPath / "forceDensity2d.comp.spv");
		rungeKutta2Step1ComputeShader = ComputeShader("rungeKutta2Step1_2d", directoryPath / "rungeKutta2Step1_2d.comp.spv");
		rungeKutta2Step2ComputeShader = ComputeShader("rungeKutta2Step2_2d", directoryPath / "rungeKutta2Step2_2d.comp.spv");
		boundaryCollisionsComputeShader = ComputeShader("boundaryCollisions2d", directoryPath / "boundaryCollisions2d.comp.spv");
	}
	void SphFluid2dGpuSolver::ComputeShaders::SetUseHashGridOptimization(bool useHashGridOptimization)
	{
		densityComputeShader.SetValue("Values", "useHashGridOptimization", (int)useHashGridOptimization);
		normalAndCurvatureComputeShader.SetValue("Values", "useHashGridOptimization", (int)useHashGridOptimization);
		forceDensityComputeShader.SetValue("Values", "useHashGridOptimization", (int)useHashGridOptimization);
	}
	void SphFluid2dGpuSolver::ComputeShaders::SetEffectRadius(float effectRadius)
	{
		cellKeysComputeShader.SetValue("Values", "effectRadius", effectRadius);
		densityComputeShader.SetValue("Values", "effectRadius", effectRadius);
		normalAndCurvatureComputeShader.SetValue("Values", "effectRadius", effectRadius);
		forceDensityComputeShader.SetValue("Values", "effectRadius", effectRadius);
	}
	void SphFluid2dGpuSolver::ComputeShaders::SetHashGridSize(int hashGridSize)
	{
		cellKeysComputeShader.SetValue("Values", "hashGridSize", hashGridSize);
		densityComputeShader.SetValue("Values", "hashGridSize", hashGridSize);
		normalAndCurvatureComputeShader.SetValue("Values", "hashGridSize", hashGridSize);
		forceDensityComputeShader.SetValue("Values", "hashGridSize", hashGridSize);
	}
	void SphFluid2dGpuSolver::ComputeShaders::SetMass(float mass)
	{
		densityComputeShader.SetValue("Values", "mass", mass);
		normalAndCurvatureComputeShader.SetValue("Values", "mass", mass);
		forceDensityComputeShader.SetValue("Values", "mass", mass);
	}
	void SphFluid2dGpuSolver::ComputeShaders::SetViscosity(float viscosity)
	{
		forceDensityComputeShader.SetValue("Values", "viscosity", viscosity);
	}
	void SphFluid2dGpuSolver::ComputeShaders::SetSurfaceTension(float surfaceTension)
	{
		forceDensityComputeShader.SetValue("Values", "surfaceTension", surfaceTension);
	}
	void SphFluid2dGpuSolver::ComputeShaders::SetCollisionDampening(float collisionDampening)
	{
		boundaryCollisionsComputeShader.SetValue("Values", "collisionDampening", collisionDampening);
	}
	void SphFluid2dGpuSolver::ComputeShaders::SetTargetDensity(float targetDensity)
	{
		forceDensityComputeShader.SetValue("Values", "targetDensity", targetDensity);
	}
	void SphFluid2dGpuSolver::ComputeShaders::SetPressureMultiplier(float pressureMultiplier)
	{
		forceDensityComputeShader.SetValue("Values", "pressureMultiplier", pressureMultiplier);
	}
	void SphFluid2dGpuSolver::ComputeShaders::SetGravity(float gravity)
	{
		forceDensityComputeShader.SetValue("Values", "gravity", gravity);
	}
	void SphFluid2dGpuSolver::ComputeShaders::SetMaxVelocity(float maxVelocity)
	{
		rungeKutta2Step1ComputeShader.SetValue("ShaderValues", "maxVelocity", maxVelocity);
		rungeKutta2Step2ComputeShader.SetValue("ShaderValues", "maxVelocity", maxVelocity);
	}
	void SphFluid2dGpuSolver::ComputeShaders::SetFluidBounds(const Bounds& bounds)
	{
		boundaryCollisionsComputeShader.SetValue("Values", "min", bounds.GetMin());
		boundaryCollisionsComputeShader.SetValue("Values", "max", bounds.GetMax());
	}
	void SphFluid2dGpuSolver::ComputeShaders::SetAttractorRadius(float attractorRadius)
	{
		forceDensityComputeShader.SetValue("Values", "attractorRadius", attractorRadius);
	}
	void SphFluid2dGpuSolver::ComputeShaders::SetAttractorStrength(float attractorStrength)
	{
		forceDensityComputeShader.SetValue("Values", "attractorStrength", attractorStrength);
	}
	void SphFluid2dGpuSolver::ComputeShaders::SetAttractorState(int attractorState)
	{
		forceDensityComputeShader.SetValue("Values", "attractorState", attractorState);
	}
	void SphFluid2dGpuSolver::ComputeShaders::SetAttractorPoint(const Float2& attractorPoint)
	{
		forceDensityComputeShader.SetValue("Values", "attractorPoint", attractorPoint);
	}




	// Public methods:
	// Ordinary Differential Equation Solvers:
	void SphFluid2dGpuSolver::TimeStepRungeKutta2(float dt, const Settings& settings, Data& data, ComputeShaders& computeShaders, const Attractor& attractor, RungeKutta& rungeKutta)
	{
		// Update hash grid for fast nearest neighbor particle look up:
		Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType, computeShaders.sessionID);
		if (settings.useHashGridOptimization)
		{
			ComputeCellKeys(computeShaders, data.cellKeyBuffer.GetBufferView(), data.positionBuffer.GetBufferView());
			Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType, computeShaders.sessionID);

			GpuSort<uint32_t>::SortPermutation(computeShaders.computeType, data.cellKeyBuffer.GetBufferView(), data.sortPermutationBuffer.GetBufferView(), computeShaders.sessionID);
			Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType, computeShaders.sessionID);

			ComputeStartIndices(computeShaders, data.startIndexBuffer.GetBufferView(), data.cellKeyBuffer.GetBufferView());
			GpuSort<Float2>::ApplyPermutation(computeShaders.computeType, data.sortPermutationBuffer.GetBufferView(), data.positionBuffer.GetBufferView(), data.tempBuffer0.GetBufferView(), computeShaders.sessionID);
			GpuSort<Float2>::ApplyPermutation(computeShaders.computeType, data.sortPermutationBuffer.GetBufferView(), data.velocityBuffer.GetBufferView(), data.tempBuffer1.GetBufferView(), computeShaders.sessionID);
			Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType, computeShaders.sessionID);
		}

		// First Runge-Kutta step:
		BufferView<Float2>& currentPositionBufferView = settings.useHashGridOptimization ? data.tempBuffer0.GetBufferView() : data.positionBuffer.GetBufferView();
		BufferView<Float2>& currentVelocityBufferView = settings.useHashGridOptimization ? data.tempBuffer1.GetBufferView() : data.velocityBuffer.GetBufferView();
		ComputeDensities(computeShaders, data.densityBuffer.GetBufferView(), currentPositionBufferView, data.startIndexBuffer.GetBufferView(), data.cellKeyBuffer.GetBufferView());
		Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType, computeShaders.sessionID);
		ComputeNormalsAndCurvatures(computeShaders, data.normalBuffer.GetBufferView(), data.curvatureBuffer.GetBufferView(), data.densityBuffer.GetBufferView(), currentPositionBufferView, data.startIndexBuffer.GetBufferView(), data.cellKeyBuffer.GetBufferView());
		Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType, computeShaders.sessionID);
		ComputeForceDensities(computeShaders, data.forceDensityBuffer.GetBufferView(), data.densityBuffer.GetBufferView(), currentPositionBufferView, currentVelocityBufferView, data.normalBuffer.GetBufferView(), data.curvatureBuffer.GetBufferView(), data.startIndexBuffer.GetBufferView(), data.cellKeyBuffer.GetBufferView());
		Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType, computeShaders.sessionID);
		ComputeRungeKutta2Step1(computeShaders, dt, data.forceDensityBuffer.GetBufferView(), data.densityBuffer.GetBufferView(), currentPositionBufferView, currentVelocityBufferView, rungeKutta.kp1Buffer.GetBufferView(), rungeKutta.kv1Buffer.GetBufferView(), rungeKutta.tempPositionBuffer.GetBufferView(), rungeKutta.tempVelocityBuffer.GetBufferView());
		Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType, computeShaders.sessionID);

		// Update hash grid for fast nearest neighbor particle look up:
		BufferView<Float2>* pTempPositionBufferView = &rungeKutta.tempPositionBuffer.GetBufferView();
		BufferView<Float2>* pTempVelocityBufferView = &rungeKutta.tempVelocityBuffer.GetBufferView();
		BufferView<Float2>* pKp1BufferView = &rungeKutta.kp1Buffer.GetBufferView();
		BufferView<Float2>* pKv1BufferView = &rungeKutta.kv1Buffer.GetBufferView();
		if (settings.useHashGridOptimization)
		{
			ComputeCellKeys(computeShaders, data.cellKeyBuffer.GetBufferView(), rungeKutta.tempPositionBuffer.GetBufferView());
			Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType, computeShaders.sessionID);

			GpuSort<uint32_t>::SortPermutation(computeShaders.computeType, data.cellKeyBuffer.GetBufferView(), data.sortPermutationBuffer.GetBufferView(), computeShaders.sessionID);
			Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType, computeShaders.sessionID);

			ComputeStartIndices(computeShaders, data.startIndexBuffer.GetBufferView(), data.cellKeyBuffer.GetBufferView());
			GpuSort<Float2>::ApplyPermutation(computeShaders.computeType, data.sortPermutationBuffer.GetBufferView(), rungeKutta.tempPositionBuffer.GetBufferView(), data.tempBuffer2.GetBufferView(), computeShaders.sessionID);
			GpuSort<Float2>::ApplyPermutation(computeShaders.computeType, data.sortPermutationBuffer.GetBufferView(), rungeKutta.tempVelocityBuffer.GetBufferView(), data.tempBuffer3.GetBufferView(), computeShaders.sessionID);
			GpuSort<Float2>::ApplyPermutation(computeShaders.computeType, data.sortPermutationBuffer.GetBufferView(), rungeKutta.kp1Buffer.GetBufferView(), data.tempBuffer4.GetBufferView(), computeShaders.sessionID);
			GpuSort<Float2>::ApplyPermutation(computeShaders.computeType, data.sortPermutationBuffer.GetBufferView(), rungeKutta.kv1Buffer.GetBufferView(), data.tempBuffer5.GetBufferView(), computeShaders.sessionID);
			GpuSort<Float2>::ApplyPermutation(computeShaders.computeType, data.sortPermutationBuffer.GetBufferView(), currentPositionBufferView, data.positionBuffer.GetBufferView(), computeShaders.sessionID);
			GpuSort<Float2>::ApplyPermutation(computeShaders.computeType, data.sortPermutationBuffer.GetBufferView(), currentVelocityBufferView, data.velocityBuffer.GetBufferView(), computeShaders.sessionID);
			Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType, computeShaders.sessionID);

			pTempPositionBufferView = &data.tempBuffer2.GetBufferView();
			pTempVelocityBufferView = &data.tempBuffer3.GetBufferView();
			pKp1BufferView = &data.tempBuffer4.GetBufferView();
			pKv1BufferView = &data.tempBuffer5.GetBufferView();
		}

		// Second Runge-Kutta step:
		ComputeDensities(computeShaders, data.densityBuffer.GetBufferView(), *pTempPositionBufferView, data.startIndexBuffer.GetBufferView(), data.cellKeyBuffer.GetBufferView());
		Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType, computeShaders.sessionID);
		ComputeNormalsAndCurvatures(computeShaders, data.normalBuffer.GetBufferView(), data.curvatureBuffer.GetBufferView(), data.densityBuffer.GetBufferView(), *pTempPositionBufferView, data.startIndexBuffer.GetBufferView(), data.cellKeyBuffer.GetBufferView());
		Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType, computeShaders.sessionID);
		ComputeForceDensities(computeShaders, data.forceDensityBuffer.GetBufferView(), data.densityBuffer.GetBufferView(), *pTempPositionBufferView, *pTempVelocityBufferView, data.normalBuffer.GetBufferView(), data.curvatureBuffer.GetBufferView(), data.startIndexBuffer.GetBufferView(), data.cellKeyBuffer.GetBufferView());
		Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType, computeShaders.sessionID);
		ComputeRungeKutta2Step2(computeShaders, dt, data.forceDensityBuffer.GetBufferView(), data.densityBuffer.GetBufferView(), *pKp1BufferView, *pKv1BufferView, *pTempVelocityBufferView, data.positionBuffer.GetBufferView(), data.velocityBuffer.GetBufferView());
		Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType, computeShaders.sessionID);

		// Resolve boundary collisions:
		ComputeBoundaryCollisions(computeShaders, data.positionBuffer.GetBufferView(), data.velocityBuffer.GetBufferView());
	}



	// Field computations:
	void SphFluid2dGpuSolver::ComputeCellKeys(ComputeShaders& computeShaders, const BufferView<uint32_t>& cellKeyBufferView, const BufferView<Float2>& positionBufferView)
	{
		Uint3 threadCount(cellKeyBufferView.GetCount(), 1, 1);
		ShaderProperties shaderProperties = Compute::RecordComputeShader(computeShaders.computeType, computeShaders.cellKeysComputeShader, threadCount, computeShaders.sessionID);
		shaderProperties.SetBuffer("cellKeyBuffer", cellKeyBufferView.GetBuffer());
		shaderProperties.SetBuffer("positionBuffer", positionBufferView.GetBuffer());
	}
	void SphFluid2dGpuSolver::ComputeStartIndices(ComputeShaders& computeShaders, const BufferView<uint32_t>& startIndexBufferView, const BufferView<uint32_t>& cellKeyBufferView)
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
	void SphFluid2dGpuSolver::ComputeDensities(ComputeShaders& computeShaders, const BufferView<float>& densityBufferView, const BufferView<Float2>& positionBufferView, const BufferView<uint32_t>& startIndexBufferView, const BufferView<uint32_t>& cellKeyBufferView)
	{
		Uint3 threadCount(densityBufferView.GetCount(), 1, 1);
		ShaderProperties shaderProperties = Compute::RecordComputeShader(computeShaders.computeType, computeShaders.densityComputeShader, threadCount, computeShaders.sessionID);
		shaderProperties.SetBuffer("densityBuffer", densityBufferView.GetBuffer());
		shaderProperties.SetBuffer("positionBuffer", positionBufferView.GetBuffer());
		shaderProperties.SetBuffer("startIndexBuffer", startIndexBufferView.GetBuffer());
		shaderProperties.SetBuffer("cellKeyBuffer", cellKeyBufferView.GetBuffer());
	}
	void SphFluid2dGpuSolver::ComputeNormalsAndCurvatures(ComputeShaders& computeShaders, const BufferView<Float2>& normalBufferView, const BufferView<float>& curvatureBufferView, const BufferView<float>& densityBufferView, const BufferView<Float2>& positionBufferView, const BufferView<uint32_t>& startIndexBufferView, const BufferView<uint32_t>& cellKeyBufferView)
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
	void SphFluid2dGpuSolver::ComputeForceDensities(ComputeShaders& computeShaders, const BufferView<Float2>& forceDensityBufferView, const BufferView<float>& densityBufferView, const BufferView<Float2>& positionBufferView, const BufferView<Float2>& velocityBufferView, const BufferView<Float2>& normalBufferView, const BufferView<float>& curvatureBufferView, const BufferView<uint32_t>& startIndexBufferView, const BufferView<uint32_t>& cellKeyBufferView)
	{
		Uint3 threadCount(forceDensityBufferView.GetCount(), 1, 1);
		ShaderProperties shaderProperties = Compute::RecordComputeShader(computeShaders.computeType, computeShaders.forceDensityComputeShader, threadCount, computeShaders.sessionID);
		shaderProperties.SetBuffer("cellKeyBuffer", cellKeyBufferView.GetBuffer());
		shaderProperties.SetBuffer("startIndexBuffer", startIndexBufferView.GetBuffer());
		shaderProperties.SetBuffer("positionBuffer", positionBufferView.GetBuffer());
		shaderProperties.SetBuffer("velocityBuffer", velocityBufferView.GetBuffer());
		shaderProperties.SetBuffer("densityBuffer", densityBufferView.GetBuffer());
		shaderProperties.SetBuffer("normalBuffer", normalBufferView.GetBuffer());
		shaderProperties.SetBuffer("curvatureBuffer", curvatureBufferView.GetBuffer());
		shaderProperties.SetBuffer("forceDensityBuffer", forceDensityBufferView.GetBuffer());
	}
	void SphFluid2dGpuSolver::ComputeRungeKutta2Step1(ComputeShaders& computeShaders, float dt, const BufferView<Float2>& forceDensityBufferView, const BufferView<float>& densityBufferView, const BufferView<Float2>& positionBufferView, const BufferView<Float2>& velocityBufferView, const BufferView<Float2>& kp1BufferView, const BufferView<Float2>& kv1BufferView, const BufferView<Float2>& tempPositionBufferView, const BufferView<Float2>& tempVelocityBufferView)
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
	void SphFluid2dGpuSolver::ComputeRungeKutta2Step2(ComputeShaders& computeShaders, float dt, const BufferView<Float2>& forceDensityBufferView, const BufferView<float>& densityBufferView, const BufferView<Float2>& kp1BufferView, const BufferView<Float2>& kv1BufferView, const BufferView<Float2>& tempVelocityBufferView, const BufferView<Float2>& positionBufferView, const BufferView<Float2>& velocityBufferView)
	{
		Uint3 threadCount(positionBufferView.GetCount(), 1, 1);
		ShaderProperties shaderProperties = Compute::RecordComputeShader(computeShaders.computeType, computeShaders.rungeKutta2Step2ComputeShader, threadCount, computeShaders.sessionID);
		shaderProperties.SetValue("CallValues", "dt", dt);
		shaderProperties.SetBuffer("forceDensityBuffer", forceDensityBufferView.GetBuffer());
		shaderProperties.SetBuffer("densityBuffer", densityBufferView.GetBuffer());
		shaderProperties.SetBuffer("kp1Buffer", kp1BufferView.GetBuffer());
		shaderProperties.SetBuffer("kv1Buffer", kv1BufferView.GetBuffer());
		shaderProperties.SetBuffer("tempVelocityBuffer", tempVelocityBufferView.GetBuffer());
		shaderProperties.SetBuffer("positionBuffer", positionBufferView.GetBuffer());
		shaderProperties.SetBuffer("velocityBuffer", velocityBufferView.GetBuffer());
	}
	void SphFluid2dGpuSolver::ComputeBoundaryCollisions(ComputeShaders& computeShaders, const BufferView<Float2>& positionBufferView, const BufferView<Float2>& velocityBufferView)
	{
		Uint3 threadCount(positionBufferView.GetCount(), 1, 1);
		ShaderProperties shaderProperties = Compute::RecordComputeShader(computeShaders.computeType, computeShaders.boundaryCollisionsComputeShader, threadCount, computeShaders.sessionID);
		shaderProperties.SetBuffer("positionBuffer", positionBufferView.GetBuffer());
		shaderProperties.SetBuffer("velocityBuffer", velocityBufferView.GetBuffer());
	}
}