#include "sphFluid2dGpuSolver.h"



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
	void SphFluid2dGpuSolver::Data::Reallocate(int particleCount, float initialDistributionRadius)
	{
		if (particleCount != ParticleCount())
		{
			// Reallocate:
			int hashGridSize = math::NextPrimeAbove(2 * particleCount);
			cellKeyBuffer = BufferTyped<int>((uint32_t)particleCount, "cellKeyBuffer", BufferUsage::storage);
			startIndexBuffer = BufferTyped<int>((uint32_t)hashGridSize, "startIndexBuffer", BufferUsage::storage);
			sortPermutationBuffer = BufferTyped<uint32_t>((uint32_t)particleCount, "sortPermutationBuffer", BufferUsage::storage);
			positionBuffer = BufferTyped<Float2>((uint32_t)particleCount, "positionBuffer", BufferUsage::storage);
			velocityBuffer = BufferTyped<Float2>((uint32_t)particleCount, "velocityBuffer", BufferUsage::storage);
			densityBuffer = BufferTyped<float>((uint32_t)particleCount, "densityBuffer", BufferUsage::storage);
			normalBuffer = BufferTyped<Float2>((uint32_t)particleCount, "normalBuffer", BufferUsage::storage);
			curvatureBuffer = BufferTyped<Float2>((uint32_t)particleCount, "curvatureBuffer", BufferUsage::storage);
			forceDensityBuffer = BufferTyped<Float2>((uint32_t)particleCount, "forceDensityBuffer", BufferUsage::storage);
			tempBuffer0 = BufferTyped<Float2>((uint32_t)particleCount, "tempBuffer0", BufferUsage::storage);
			tempBuffer1 = BufferTyped<Float2>((uint32_t)particleCount, "tempBuffer1", BufferUsage::storage);
			tempBuffer2 = BufferTyped<Float2>((uint32_t)particleCount, "tempBuffer2", BufferUsage::storage);
			tempBuffer3 = BufferTyped<Float2>((uint32_t)particleCount, "tempBuffer3", BufferUsage::storage);
			tempBuffer4 = BufferTyped<Float2>((uint32_t)particleCount, "tempBuffer4", BufferUsage::storage);
			tempBuffer5 = BufferTyped<Float2>((uint32_t)particleCount, "tempBuffer5", BufferUsage::storage);
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
			Compute::PreRender::RecordComputeShader(resetComputeShader, resetProperties, threadCount);
		}
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
	void SphFluid2dGpuSolver::RungeKutta::Reallocate(int particleCount)
	{
		if (particleCount != ParticleCount())
		{
			// Reallocate:
			kp1Buffer = BufferTyped<Float2>((uint32_t)particleCount, "kp1Buffer", BufferUsage::storage);
			kv1Buffer = BufferTyped<Float2>((uint32_t)particleCount, "kv1Buffer", BufferUsage::storage);
			tempPositionBuffer = BufferTyped<Float2>((uint32_t)particleCount, "tempPositionBuffer", BufferUsage::storage);
			tempVelocityBuffer = BufferTyped<Float2>((uint32_t)particleCount, "tempVelocityBuffer", BufferUsage::storage);
			// Reset data:
			Uint3 threadCount(kp1Buffer.GetCount(), 1, 1);
			resetProperties.SetBuffer("kp1Buffer", kp1Buffer.GetBuffer());
			resetProperties.SetBuffer("kv1Buffer", kv1Buffer.GetBuffer());
			resetProperties.SetBuffer("tempPositionBuffer", tempPositionBuffer.GetBuffer());
			resetProperties.SetBuffer("tempVelocityBuffer", tempVelocityBuffer.GetBuffer());
			Compute::PreRender::RecordComputeShader(resetComputeShader, resetProperties, threadCount);
		}
	}



	// ComputeShaders struct:
	SphFluid2dGpuSolver::ComputeShaders::ComputeShaders()
	{
		// Load compute shaders:
		std::filesystem::path directoryPath = (std::filesystem::path(PROJECT_SHADERS_DIR) / "bin").make_preferred();
		cellKeysComputeShader = ComputeShader("cellKeys2d", directoryPath / "cellKeys2d.comp.spv");
		startIndicesComputeShader = ComputeShader("startIndices2d", directoryPath / "startIndices2d.comp.spv");
		densityComputeShader = ComputeShader("density2d", directoryPath / "density2d.comp.spv");
		normalAndCurvatureComputeShader = ComputeShader("normalAndCurvature2d", directoryPath / "normalAndCurvature2d.comp.spv");
		forceDensityComputeShader = ComputeShader("forceDensity2d", directoryPath / "forceDensity2d.comp.spv");
		rungeKutta2Step1ComputeShader = ComputeShader("rungeKutta2Step1_2d", directoryPath / "rungeKutta2Step1_2d.comp.spv");
		rungeKutta2Step2ComputeShader = ComputeShader("rungeKutta2Step2_2d", directoryPath / "rungeKutta2Step2_2d.comp.spv");
		boundaryCollisionsComputeShader = ComputeShader("boundaryCollisions2d", directoryPath / "boundaryCollisions2d.comp.spv");

		// Initialize shader properties:
		cellKeysProperties = ShaderProperties(cellKeysComputeShader);
		startIndicesProperties = ShaderProperties(startIndicesComputeShader);
		densityProperties = ShaderProperties(densityComputeShader);
		normalAndCurvatureProperties = ShaderProperties(normalAndCurvatureComputeShader);
		forceDensityProperties = ShaderProperties(forceDensityComputeShader);
		rungeKutta2Step1Properties = ShaderProperties(rungeKutta2Step1ComputeShader);
		rungeKutta2Step2Properties = ShaderProperties(rungeKutta2Step2ComputeShader);
		boundaryCollisionsProperties = ShaderProperties(boundaryCollisionsComputeShader);
	}
	void SphFluid2dGpuSolver::ComputeShaders::SetUseHashGridOptimization(bool useHashGridOptimization)
	{
		densityProperties.SetValue("Values", "useHashGridOptimization", (int)useHashGridOptimization);
		normalAndCurvatureProperties.SetValue("Values", "useHashGridOptimization", (int)useHashGridOptimization);
		forceDensityProperties.SetValue("Values", "useHashGridOptimization", (int)useHashGridOptimization);
	}
	void SphFluid2dGpuSolver::ComputeShaders::SetEffectRadius(float effectRadius)
	{
		cellKeysProperties.SetValue("Values", "effectRadius", effectRadius);
		densityProperties.SetValue("Values", "effectRadius", effectRadius);
		normalAndCurvatureProperties.SetValue("Values", "effectRadius", effectRadius);
		forceDensityProperties.SetValue("Values", "effectRadius", effectRadius);
	}
	void SphFluid2dGpuSolver::ComputeShaders::SetHashGridSize(int hashGridSize)
	{
		cellKeysProperties.SetValue("Values", "hashGridSize", hashGridSize);
		densityProperties.SetValue("Values", "hashGridSize", hashGridSize);
		normalAndCurvatureProperties.SetValue("Values", "hashGridSize", hashGridSize);
		forceDensityProperties.SetValue("Values", "hashGridSize", hashGridSize);
	}
	void SphFluid2dGpuSolver::ComputeShaders::SetMass(float mass)
	{
		densityProperties.SetValue("Values", "mass", mass);
		normalAndCurvatureProperties.SetValue("Values", "mass", mass);
		forceDensityProperties.SetValue("Values", "mass", mass);
	}
	void SphFluid2dGpuSolver::ComputeShaders::SetViscosity(float viscosity)
	{
		forceDensityProperties.SetValue("Values", "viscosity", viscosity);
	}
	void SphFluid2dGpuSolver::ComputeShaders::SetSurfaceTension(float surfaceTension)
	{
		forceDensityProperties.SetValue("Values", "surfaceTension", surfaceTension);
	}
	void SphFluid2dGpuSolver::ComputeShaders::SetCollisionDampening(float collisionDampening)
	{
		boundaryCollisionsProperties.SetValue("Values", "collisionDampening", collisionDampening);
	}
	void SphFluid2dGpuSolver::ComputeShaders::SetTargetDensity(float targetDensity)
	{
		forceDensityProperties.SetValue("Values", "targetDensity", targetDensity);
	}
	void SphFluid2dGpuSolver::ComputeShaders::SetPressureMultiplier(float pressureMultiplier)
	{
		forceDensityProperties.SetValue("Values", "pressureMultiplier", pressureMultiplier);
	}
	void SphFluid2dGpuSolver::ComputeShaders::SetGravity(float gravity)
	{
		forceDensityProperties.SetValue("Values", "gravity", gravity);
	}
	void SphFluid2dGpuSolver::ComputeShaders::SetMaxVelocity(float maxVelocity)
	{
		rungeKutta2Step1Properties.SetValue("Values", "maxVelocity", maxVelocity);
		rungeKutta2Step2Properties.SetValue("Values", "maxVelocity", maxVelocity);
	}
	void SphFluid2dGpuSolver::ComputeShaders::SetFluidBounds(const Bounds& bounds)
	{
		boundaryCollisionsProperties.SetValue("Values", "min", bounds.GetMin());
		boundaryCollisionsProperties.SetValue("Values", "max", bounds.GetMax());
	}
	void SphFluid2dGpuSolver::ComputeShaders::SetAttractorRadius(float attractorRadius)
	{
		forceDensityProperties.SetValue("Values", "attractorRadius", attractorRadius);
	}
	void SphFluid2dGpuSolver::ComputeShaders::SetAttractorStrength(float attractorStrength)
	{
		forceDensityProperties.SetValue("Values", "attractorStrength", attractorStrength);
	}
	void SphFluid2dGpuSolver::ComputeShaders::SetAttractorState(int attractorState)
	{
		forceDensityProperties.SetValue("Values", "attractorState", attractorState);
	}
	void SphFluid2dGpuSolver::ComputeShaders::SetAttractorPoint(const Float2& attractorPoint)
	{
		forceDensityProperties.SetValue("Values", "attractorPoint", attractorPoint);
	}




	// Public methods:
	// Ordinary Differential Equation Solvers:
	void SphFluid2dGpuSolver::TimeStepRungeKutta2(float dt, const Settings& settings, Data& data, ComputeShaders& computeShaders, const Attractor& attractor, RungeKutta& rungeKutta)
	{
		computeShaders.rungeKutta2Step1Properties.SetValue("Values", "dt", dt);
		computeShaders.rungeKutta2Step2Properties.SetValue("Values", "dt", dt);

		// Update hash grid for fast nearest neighbor particle look up:
		if (settings.useHashGridOptimization)
		{
			ComputeCellKeys(computeShaders, data.cellKeyBuffer.GetBufferView(), data.positionBuffer.GetBufferView());
			Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType);

			GpuSort<int>::SortPermutation(ComputeType::preRender, data.cellKeyBuffer.GetBufferView(), data.sortPermutationBuffer.GetBufferView());
			Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType);

			ComputeStartIndices(computeShaders, data.startIndexBuffer.GetBufferView(), data.cellKeyBuffer.GetBufferView());
			GpuSort<Float2>::ApplyPermutation(ComputeType::preRender, data.sortPermutationBuffer.GetBufferView(), data.positionBuffer.GetBufferView(), data.tempBuffer0.GetBufferView());
			GpuSort<Float2>::ApplyPermutation(ComputeType::preRender, data.sortPermutationBuffer.GetBufferView(), data.velocityBuffer.GetBufferView(), data.tempBuffer1.GetBufferView());
			Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType);

			std::swap(data.positionBuffer, data.tempBuffer0);
			std::swap(data.velocityBuffer, data.tempBuffer1);
		}

		// First Runte-Kutta step:
		ComputeDensities(computeShaders, data.densityBuffer.GetBufferView(), data.positionBuffer.GetBufferView(), data.startIndexBuffer.GetBufferView(), data.cellKeyBuffer.GetBufferView());
		Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType);
		ComputeNormalsAndCurvatures(computeShaders, data.normalBuffer.GetBufferView(), data.curvatureBuffer.GetBufferView(), data.densityBuffer.GetBufferView(), data.positionBuffer.GetBufferView(), data.startIndexBuffer.GetBufferView(), data.cellKeyBuffer.GetBufferView());
		Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType);
		ComputeForceDensities(computeShaders, data.forceDensityBuffer.GetBufferView(), data.densityBuffer.GetBufferView(), data.positionBuffer.GetBufferView(), data.velocityBuffer.GetBufferView(), data.normalBuffer.GetBufferView(), data.curvatureBuffer.GetBufferView(), data.startIndexBuffer.GetBufferView(), data.cellKeyBuffer.GetBufferView());
		Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType);
		ComputeRungeKutta2Step1(computeShaders, data.forceDensityBuffer.GetBufferView(), data.densityBuffer.GetBufferView(), data.positionBuffer.GetBufferView(), data.velocityBuffer.GetBufferView(), rungeKutta.kp1Buffer.GetBufferView(), rungeKutta.kv1Buffer.GetBufferView(), rungeKutta.tempPositionBuffer.GetBufferView(), rungeKutta.tempVelocityBuffer.GetBufferView());
		Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType);

		// Update hash grid for fast nearest neighbor particle look up:
		if (settings.useHashGridOptimization)
		{
			ComputeCellKeys(computeShaders, data.cellKeyBuffer.GetBufferView(), rungeKutta.tempPositionBuffer.GetBufferView());
			Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType);

			GpuSort<int>::SortPermutation(ComputeType::preRender, data.cellKeyBuffer.GetBufferView(), data.sortPermutationBuffer.GetBufferView());
			Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType);

			ComputeStartIndices(computeShaders, data.startIndexBuffer.GetBufferView(), data.cellKeyBuffer.GetBufferView());
			GpuSort<Float2>::ApplyPermutation(ComputeType::preRender, data.sortPermutationBuffer.GetBufferView(), rungeKutta.tempPositionBuffer.GetBufferView(), data.tempBuffer0.GetBufferView());
			GpuSort<Float2>::ApplyPermutation(ComputeType::preRender, data.sortPermutationBuffer.GetBufferView(), rungeKutta.tempVelocityBuffer.GetBufferView(), data.tempBuffer1.GetBufferView());
			GpuSort<Float2>::ApplyPermutation(ComputeType::preRender, data.sortPermutationBuffer.GetBufferView(), rungeKutta.kp1Buffer.GetBufferView(), data.tempBuffer2.GetBufferView());
			GpuSort<Float2>::ApplyPermutation(ComputeType::preRender, data.sortPermutationBuffer.GetBufferView(), rungeKutta.kv1Buffer.GetBufferView(), data.tempBuffer3.GetBufferView());
			GpuSort<Float2>::ApplyPermutation(ComputeType::preRender, data.sortPermutationBuffer.GetBufferView(), data.positionBuffer.GetBufferView(), data.tempBuffer4.GetBufferView());
			GpuSort<Float2>::ApplyPermutation(ComputeType::preRender, data.sortPermutationBuffer.GetBufferView(), data.velocityBuffer.GetBufferView(), data.tempBuffer5.GetBufferView());
			Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType);

			std::swap(rungeKutta.tempPositionBuffer.GetBufferView(), data.tempBuffer0.GetBufferView());
			std::swap(rungeKutta.tempVelocityBuffer.GetBufferView(), data.tempBuffer1.GetBufferView());
			std::swap(rungeKutta.kp1Buffer.GetBufferView(), data.tempBuffer2.GetBufferView());
			std::swap(rungeKutta.kv1Buffer.GetBufferView(), data.tempBuffer3.GetBufferView());
			std::swap(data.positionBuffer.GetBufferView(), data.tempBuffer4.GetBufferView());
			std::swap(data.velocityBuffer.GetBufferView(), data.tempBuffer5.GetBufferView());
		}

		// Second Runge-Kutta step:
		ComputeDensities(computeShaders, data.densityBuffer.GetBufferView(), rungeKutta.tempPositionBuffer.GetBufferView(), data.startIndexBuffer.GetBufferView(), data.cellKeyBuffer.GetBufferView());
		Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType);
		ComputeNormalsAndCurvatures(computeShaders, data.normalBuffer.GetBufferView(), data.curvatureBuffer.GetBufferView(), data.densityBuffer.GetBufferView(), rungeKutta.tempPositionBuffer.GetBufferView(), data.startIndexBuffer.GetBufferView(), data.cellKeyBuffer.GetBufferView());
		Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType);
		ComputeForceDensities(computeShaders, data.forceDensityBuffer.GetBufferView(), data.densityBuffer.GetBufferView(), rungeKutta.tempPositionBuffer.GetBufferView(), rungeKutta.tempVelocityBuffer.GetBufferView(), data.normalBuffer.GetBufferView(), data.curvatureBuffer.GetBufferView(), data.startIndexBuffer.GetBufferView(), data.cellKeyBuffer.GetBufferView());
		Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType);
		ComputeRungeKutta2Step1(computeShaders, data.forceDensityBuffer.GetBufferView(), data.densityBuffer.GetBufferView(), data.positionBuffer.GetBufferView(), data.velocityBuffer.GetBufferView(), rungeKutta.kp1Buffer.GetBufferView(), rungeKutta.kv1Buffer.GetBufferView(), rungeKutta.tempPositionBuffer.GetBufferView(), rungeKutta.tempVelocityBuffer.GetBufferView());
		Compute::RecordBarrierWaitStorageWriteBeforeRead(computeShaders.computeType);

		// Resolve boundary collisions:
		ComputeBoundaryCollisions(computeShaders, data.positionBuffer.GetBufferView(), data.velocityBuffer.GetBufferView());
	}



	// Field computations:
	void SphFluid2dGpuSolver::ComputeCellKeys(ComputeShaders& computeShaders, const BufferView<int>& cellKeyBufferView, const BufferView<Float2>& positionBufferView)
	{
		Uint3 threadCount(cellKeyBufferView.GetCount(), 1, 1);
		computeShaders.cellKeysProperties.SetBuffer("cellKeyBuffer", cellKeyBufferView.GetBuffer());
		computeShaders.cellKeysProperties.SetBuffer("positionBuffer", positionBufferView.GetBuffer());
		Compute::RecordComputeShader(computeShaders.computeType, computeShaders.cellKeysComputeShader, computeShaders.cellKeysProperties, threadCount);
	}
	void SphFluid2dGpuSolver::ComputeStartIndices(ComputeShaders& computeShaders, const BufferView<int> startIndexBufferView, const BufferView<int>& cellKeyBufferView)
	{
		Uint3 threadCount(cellKeyBufferView.GetCount(), 1, 1);	// dispatch size is equal to number of particles. startIndexBuffer is bigger.
		computeShaders.startIndicesProperties.SetBuffer("startIndexBuffer", startIndexBufferView.GetBuffer());
		computeShaders.startIndicesProperties.SetBuffer("cellKeyBuffer", cellKeyBufferView.GetBuffer());
		Compute::RecordComputeShader(computeShaders.computeType, computeShaders.startIndicesComputeShader, computeShaders.startIndicesProperties, threadCount);
	}
	void SphFluid2dGpuSolver::ComputeDensities(ComputeShaders& computeShaders, const BufferView<float>& densityBufferView, const BufferView<Float2>& positionBufferView, const BufferView<int> startIndexBufferView, const BufferView<int>& cellKeyBufferView)
	{
		Uint3 threadCount(densityBufferView.GetCount(), 1, 1);
		computeShaders.densityProperties.SetBuffer("densityBuffer", densityBufferView.GetBuffer());
		computeShaders.densityProperties.SetBuffer("positionBuffer", positionBufferView.GetBuffer());
		computeShaders.densityProperties.SetBuffer("startIndexBuffer", startIndexBufferView.GetBuffer());
		computeShaders.densityProperties.SetBuffer("cellKeyBuffer", cellKeyBufferView.GetBuffer());
		Compute::RecordComputeShader(computeShaders.computeType, computeShaders.densityComputeShader, computeShaders.densityProperties, threadCount);
	}
	void SphFluid2dGpuSolver::ComputeNormalsAndCurvatures(ComputeShaders& computeShaders, const BufferView<Float2>& normalBufferView, const BufferView<Float2>& curvatureBufferView, const BufferView<float>& densityBufferView, const BufferView<Float2>& positionBufferView, const BufferView<int> startIndexBufferView, const BufferView<int>& cellKeyBufferView)
	{
		Uint3 threadCount(normalBufferView.GetCount(), 1, 1);
		computeShaders.normalAndCurvatureProperties.SetBuffer("normalBuffer", normalBufferView.GetBuffer());
		computeShaders.normalAndCurvatureProperties.SetBuffer("curvatureBuffer", curvatureBufferView.GetBuffer());
		computeShaders.normalAndCurvatureProperties.SetBuffer("densityBuffer", densityBufferView.GetBuffer());
		computeShaders.normalAndCurvatureProperties.SetBuffer("positionBuffer", positionBufferView.GetBuffer());
		computeShaders.normalAndCurvatureProperties.SetBuffer("startIndexBuffer", startIndexBufferView.GetBuffer());
		computeShaders.normalAndCurvatureProperties.SetBuffer("cellKeyBuffer", cellKeyBufferView.GetBuffer());
		Compute::RecordComputeShader(computeShaders.computeType, computeShaders.normalAndCurvatureComputeShader, computeShaders.normalAndCurvatureProperties, threadCount);
	}
	void SphFluid2dGpuSolver::ComputeForceDensities(ComputeShaders& computeShaders, const BufferView<Float2>& forceDensityBufferView, const BufferView<float>& densityBufferView, const BufferView<Float2>& positionBufferView, const BufferView<Float2>& velocityBufferView, const BufferView<Float2>& normalBufferView, const BufferView<Float2>& curvatureBufferView, const BufferView<int> startIndexBufferView, const BufferView<int>& cellKeyBufferView)
	{
		Uint3 threadCount(forceDensityBufferView.GetCount(), 1, 1);
		computeShaders.forceDensityProperties.SetBuffer("forceDensityBuffer", forceDensityBufferView.GetBuffer());
		computeShaders.forceDensityProperties.SetBuffer("densityBuffer", densityBufferView.GetBuffer());
		computeShaders.forceDensityProperties.SetBuffer("positionBuffer", positionBufferView.GetBuffer());
		computeShaders.forceDensityProperties.SetBuffer("velocityBuffer", velocityBufferView.GetBuffer());
		computeShaders.forceDensityProperties.SetBuffer("normalBuffer", normalBufferView.GetBuffer());
		computeShaders.forceDensityProperties.SetBuffer("curvatureBuffer", curvatureBufferView.GetBuffer());
		computeShaders.forceDensityProperties.SetBuffer("startIndexBuffer", startIndexBufferView.GetBuffer());
		computeShaders.forceDensityProperties.SetBuffer("cellKeyBuffer", cellKeyBufferView.GetBuffer());
		Compute::RecordComputeShader(computeShaders.computeType, computeShaders.forceDensityComputeShader, computeShaders.forceDensityProperties, threadCount);
	}
	void SphFluid2dGpuSolver::ComputeRungeKutta2Step1(ComputeShaders& computeShaders, const BufferView<Float2>& forceDensityBufferView, const BufferView<float>& densityBufferView, const BufferView<Float2>& positionBufferView, const BufferView<Float2>& velocityBufferView, const BufferView<Float2>& kp1BufferView, const BufferView<Float2>& kv1BufferView, const BufferView<Float2>& tempPositionBufferView, const BufferView<Float2>& tempVelocityBufferView)
	{
		Uint3 threadCount(positionBufferView.GetCount(), 1, 1);
		computeShaders.rungeKutta2Step1Properties.SetBuffer("forceDensityBuffer", forceDensityBufferView.GetBuffer());
		computeShaders.rungeKutta2Step1Properties.SetBuffer("densityBuffer", densityBufferView.GetBuffer());
		computeShaders.rungeKutta2Step1Properties.SetBuffer("positionBuffer", positionBufferView.GetBuffer());
		computeShaders.rungeKutta2Step1Properties.SetBuffer("velocityBuffer", velocityBufferView.GetBuffer());
		computeShaders.rungeKutta2Step1Properties.SetBuffer("kp1Buffer", kp1BufferView.GetBuffer());
		computeShaders.rungeKutta2Step1Properties.SetBuffer("kv1Buffer", kv1BufferView.GetBuffer());
		computeShaders.rungeKutta2Step1Properties.SetBuffer("tempPositionBuffer", tempPositionBufferView.GetBuffer());
		computeShaders.rungeKutta2Step1Properties.SetBuffer("tempVelocityBuffer", tempVelocityBufferView.GetBuffer());
		Compute::RecordComputeShader(computeShaders.computeType, computeShaders.rungeKutta2Step1ComputeShader, computeShaders.rungeKutta2Step1Properties, threadCount);
	}
	void SphFluid2dGpuSolver::ComputeRungeKutta2Step2(ComputeShaders& computeShaders, const BufferView<Float2>& forceDensityBufferView, const BufferView<float>& densityBufferView, const BufferView<Float2>& kp1BufferView, const BufferView<Float2>& kv1BufferView, const BufferView<Float2>& tempPositionBufferView, const BufferView<Float2>& tempVelocityBufferView, const BufferView<Float2>& positionBufferView, const BufferView<Float2>& velocityBufferView)
	{
		Uint3 threadCount(positionBufferView.GetCount(), 1, 1);
		computeShaders.rungeKutta2Step2Properties.SetBuffer("forceDensityBuffer", forceDensityBufferView.GetBuffer());
		computeShaders.rungeKutta2Step2Properties.SetBuffer("densityBuffer", densityBufferView.GetBuffer());
		computeShaders.rungeKutta2Step2Properties.SetBuffer("kp1Buffer", kp1BufferView.GetBuffer());
		computeShaders.rungeKutta2Step2Properties.SetBuffer("kv1Buffer", kv1BufferView.GetBuffer());
		computeShaders.rungeKutta2Step2Properties.SetBuffer("tempPositionBuffer", tempPositionBufferView.GetBuffer());
		computeShaders.rungeKutta2Step2Properties.SetBuffer("tempVelocityBuffer", tempVelocityBufferView.GetBuffer());
		computeShaders.rungeKutta2Step2Properties.SetBuffer("positionBuffer", positionBufferView.GetBuffer());
		computeShaders.rungeKutta2Step2Properties.SetBuffer("velocityBuffer", velocityBufferView.GetBuffer());
		Compute::RecordComputeShader(computeShaders.computeType, computeShaders.rungeKutta2Step2ComputeShader, computeShaders.rungeKutta2Step2Properties, threadCount);
	}
	void SphFluid2dGpuSolver::ComputeBoundaryCollisions(ComputeShaders& computeShaders, const BufferView<Float2>& positionBufferView, const BufferView<Float2>& velocityBufferView)
	{
		Uint3 threadCount(positionBufferView.GetCount(), 1, 1);
		computeShaders.boundaryCollisionsProperties.SetBuffer("positionBuffer", positionBufferView.GetBuffer());
		computeShaders.boundaryCollisionsProperties.SetBuffer("velocityBuffer", velocityBufferView.GetBuffer());
		Compute::RecordComputeShader(computeShaders.computeType, computeShaders.boundaryCollisionsComputeShader, computeShaders.boundaryCollisionsProperties, threadCount);
	}
}