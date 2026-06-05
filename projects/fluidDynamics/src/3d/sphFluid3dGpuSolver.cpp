#include "sphFluid3dGpuSolver.h"
#include "logger.h"



namespace fluidDynamics
{
	// Data struct:
	int SphFluid3dGpuSolver::Data::ParticleCount()
	{
		return positionBuffer.IsValid() ? positionBuffer.GetCount() : 0;
	}
	void SphFluid3dGpuSolver::Data::Reallocate(int particleCount, float initialDistributionRadius, ComputeShaders& computeShaders)
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
	int SphFluid3dGpuSolver::RungeKutta::ParticleCount()
	{
		return kp1Buffer.IsValid() ? kp1Buffer.GetCount() : 0;
	}
	void SphFluid3dGpuSolver::RungeKutta::Reallocate(int particleCount, ComputeShaders& computeShaders)
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
		ShaderProperties shaderProperties = Compute::RecordComputeShader(computeShaders.computeType, computeShaders.resetRungeKuttaComputeShader, threadCount, computeShaders.sessionID);
		shaderProperties.SetBuffer("kp1Buffer", kp1Buffer.GetBuffer());
		shaderProperties.SetBuffer("kv1Buffer", kv1Buffer.GetBuffer());
		shaderProperties.SetBuffer("tempPositionBuffer", tempPositionBuffer.GetBuffer());
		shaderProperties.SetBuffer("tempVelocityBuffer", tempVelocityBuffer.GetBuffer());
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
	}
	void SphFluid3dGpuSolver::ComputeShaders::SetHashGridSize(int hashGridSize)
	{
		cellKeysComputeShader.SetValue("Values", "hashGridSize", hashGridSize);
		densityComputeShader.SetValue("Values", "hashGridSize", hashGridSize);
		normalAndCurvatureComputeShader.SetValue("Values", "hashGridSize", hashGridSize);
		forceDensityComputeShader.SetValue("Values", "hashGridSize", hashGridSize);
	}
	void SphFluid3dGpuSolver::ComputeShaders::SetMass(float mass)
	{
		densityComputeShader.SetValue("Values", "mass", mass);
		normalAndCurvatureComputeShader.SetValue("Values", "mass", mass);
		forceDensityComputeShader.SetValue("Values", "mass", mass);
	}
	void SphFluid3dGpuSolver::ComputeShaders::SetViscosity(float viscosity)
	{
		forceDensityComputeShader.SetValue("Values", "viscosity", viscosity);
	}
	void SphFluid3dGpuSolver::ComputeShaders::SetSurfaceTension(float surfaceTension)
	{
		forceDensityComputeShader.SetValue("Values", "surfaceTension", surfaceTension);
	}
	void SphFluid3dGpuSolver::ComputeShaders::SetCollisionDampening(float collisionDampening)
	{
		boundaryCollisionsComputeShader.SetValue("Values", "collisionDampening", collisionDampening);
	}
	void SphFluid3dGpuSolver::ComputeShaders::SetTargetDensity(float targetDensity)
	{
		forceDensityComputeShader.SetValue("Values", "targetDensity", targetDensity);
	}
	void SphFluid3dGpuSolver::ComputeShaders::SetPressureMultiplier(float pressureMultiplier)
	{
		forceDensityComputeShader.SetValue("Values", "pressureMultiplier", pressureMultiplier);
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
	void SphFluid3dGpuSolver::ComputeShaders::SetFluidBounds(const Bounds& bounds)
	{
		boundaryCollisionsComputeShader.SetValue("Values", "min", bounds.GetMin());
		boundaryCollisionsComputeShader.SetValue("Values", "max", bounds.GetMax());
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
	void SphFluid3dGpuSolver::TimeStepRungeKutta2(float dt, const Settings& settings, Data& data, ComputeShaders& computeShaders, RungeKutta& rungeKutta)
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
			GpuSort<Float3>::ApplyPermutation(computeShaders.computeType, data.sortPermutationBuffer.GetBufferView(), data.positionBuffer.GetBufferView(), data.tempBuffer0.GetBufferView(), computeShaders.sessionID);
			GpuSort<Float3>::ApplyPermutation(computeShaders.computeType, data.sortPermutationBuffer.GetBufferView(), data.velocityBuffer.GetBufferView(), data.tempBuffer1.GetBufferView(), computeShaders.sessionID);
			Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType, computeShaders.sessionID);
		}

		// First Runge-Kutta step:
		BufferView<Float3>& currentPositionBufferView = settings.useHashGridOptimization ? data.tempBuffer0.GetBufferView() : data.positionBuffer.GetBufferView();
		BufferView<Float3>& currentVelocityBufferView = settings.useHashGridOptimization ? data.tempBuffer1.GetBufferView() : data.velocityBuffer.GetBufferView();
		ComputeDensities(computeShaders, data.densityBuffer.GetBufferView(), currentPositionBufferView, data.startIndexBuffer.GetBufferView(), data.cellKeyBuffer.GetBufferView());
		Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType, computeShaders.sessionID);
		ComputeNormalsAndCurvatures(computeShaders, data.normalBuffer.GetBufferView(), data.curvatureBuffer.GetBufferView(), data.densityBuffer.GetBufferView(), currentPositionBufferView, data.startIndexBuffer.GetBufferView(), data.cellKeyBuffer.GetBufferView());
		Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType, computeShaders.sessionID);
		ComputeForceDensities(computeShaders, data.forceDensityBuffer.GetBufferView(), data.densityBuffer.GetBufferView(), currentPositionBufferView, currentVelocityBufferView, data.normalBuffer.GetBufferView(), data.curvatureBuffer.GetBufferView(), data.startIndexBuffer.GetBufferView(), data.cellKeyBuffer.GetBufferView());
		Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType, computeShaders.sessionID);
		ComputeRungeKutta2Step1(computeShaders, dt, data.forceDensityBuffer.GetBufferView(), data.densityBuffer.GetBufferView(), currentPositionBufferView, currentVelocityBufferView, rungeKutta.kp1Buffer.GetBufferView(), rungeKutta.kv1Buffer.GetBufferView(), rungeKutta.tempPositionBuffer.GetBufferView(), rungeKutta.tempVelocityBuffer.GetBufferView());
		Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType, computeShaders.sessionID);

		// Update hash grid for fast nearest neighbor particle look up:
		BufferView<Float3>* pTempPositionBufferView = &rungeKutta.tempPositionBuffer.GetBufferView();
		BufferView<Float3>* pTempVelocityBufferView = &rungeKutta.tempVelocityBuffer.GetBufferView();
		BufferView<Float3>* pKp1BufferView = &rungeKutta.kp1Buffer.GetBufferView();
		BufferView<Float3>* pKv1BufferView = &rungeKutta.kv1Buffer.GetBufferView();
		if (settings.useHashGridOptimization)
		{
			ComputeCellKeys(computeShaders, data.cellKeyBuffer.GetBufferView(), rungeKutta.tempPositionBuffer.GetBufferView());
			Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType, computeShaders.sessionID);

			GpuSort<uint32_t>::SortPermutation(computeShaders.computeType, data.cellKeyBuffer.GetBufferView(), data.sortPermutationBuffer.GetBufferView(), computeShaders.sessionID);
			Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType, computeShaders.sessionID);

			ComputeStartIndices(computeShaders, data.startIndexBuffer.GetBufferView(), data.cellKeyBuffer.GetBufferView());
			GpuSort<Float3>::ApplyPermutation(computeShaders.computeType, data.sortPermutationBuffer.GetBufferView(), rungeKutta.tempPositionBuffer.GetBufferView(), data.tempBuffer2.GetBufferView(), computeShaders.sessionID);
			GpuSort<Float3>::ApplyPermutation(computeShaders.computeType, data.sortPermutationBuffer.GetBufferView(), rungeKutta.tempVelocityBuffer.GetBufferView(), data.tempBuffer3.GetBufferView(), computeShaders.sessionID);
			GpuSort<Float3>::ApplyPermutation(computeShaders.computeType, data.sortPermutationBuffer.GetBufferView(), rungeKutta.kp1Buffer.GetBufferView(), data.tempBuffer4.GetBufferView(), computeShaders.sessionID);
			GpuSort<Float3>::ApplyPermutation(computeShaders.computeType, data.sortPermutationBuffer.GetBufferView(), rungeKutta.kv1Buffer.GetBufferView(), data.tempBuffer5.GetBufferView(), computeShaders.sessionID);
			GpuSort<Float3>::ApplyPermutation(computeShaders.computeType, data.sortPermutationBuffer.GetBufferView(), currentPositionBufferView, data.positionBuffer.GetBufferView(), computeShaders.sessionID);
			GpuSort<Float3>::ApplyPermutation(computeShaders.computeType, data.sortPermutationBuffer.GetBufferView(), currentVelocityBufferView, data.velocityBuffer.GetBufferView(), computeShaders.sessionID);
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
	void SphFluid3dGpuSolver::ComputeDensities(ComputeShaders& computeShaders, const BufferView<float>& densityBufferView, const BufferView<Float3>& positionBufferView, const BufferView<uint32_t>& startIndexBufferView, const BufferView<uint32_t>& cellKeyBufferView)
	{
		Uint3 threadCount(densityBufferView.GetCount(), 1, 1);
		ShaderProperties shaderProperties = Compute::RecordComputeShader(computeShaders.computeType, computeShaders.densityComputeShader, threadCount, computeShaders.sessionID);
		shaderProperties.SetBuffer("densityBuffer", densityBufferView.GetBuffer());
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
	void SphFluid3dGpuSolver::ComputeForceDensities(ComputeShaders& computeShaders, const BufferView<Float3>& forceDensityBufferView, const BufferView<float>& densityBufferView, const BufferView<Float3>& positionBufferView, const BufferView<Float3>& velocityBufferView, const BufferView<Float3>& normalBufferView, const BufferView<float>& curvatureBufferView, const BufferView<uint32_t>& startIndexBufferView, const BufferView<uint32_t>& cellKeyBufferView)
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
	void SphFluid3dGpuSolver::ComputeRungeKutta2Step2(ComputeShaders& computeShaders, float dt, const BufferView<Float3>& forceDensityBufferView, const BufferView<float>& densityBufferView, const BufferView<Float3>& kp1BufferView, const BufferView<Float3>& kv1BufferView, const BufferView<Float3>& tempVelocityBufferView, const BufferView<Float3>& positionBufferView, const BufferView<Float3>& velocityBufferView)
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
	void SphFluid3dGpuSolver::ComputeBoundaryCollisions(ComputeShaders& computeShaders, const BufferView<Float3>& positionBufferView, const BufferView<Float3>& velocityBufferView)
	{
		Uint3 threadCount(positionBufferView.GetCount(), 1, 1);
		ShaderProperties shaderProperties = Compute::RecordComputeShader(computeShaders.computeType, computeShaders.boundaryCollisionsComputeShader, threadCount, computeShaders.sessionID);
		shaderProperties.SetBuffer("positionBuffer", positionBufferView.GetBuffer());
		shaderProperties.SetBuffer("velocityBuffer", velocityBufferView.GetBuffer());
	}
}