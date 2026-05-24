#include "application.h"
#include "emberEngine.h"
#include "sphFluid2dCpuSolver.h"
#include "sphFluid2dGpuSolver.h"
#include <gtest/gtest.h>



// This setUp/tearDown runs once for all test belonging to this suite/fixture:
class TEST_SphFluid2dGpu : public ::testing::Test
{
public:
	static void SetUpTestSuite()
	{
		// Initialization:
		emberApplication::Application::CreateInfo appCreateInfo = {};
		appCreateInfo.vSyncEnabled = true;
		appCreateInfo.framesInFlight = 2;
		appCreateInfo.msaaSampleCount = emberCommon::MsaaSampleCount::sampleCount04;
		appCreateInfo.windowWidth = 1600;// 2560; //1920;
		appCreateInfo.windowHeight = 900;// 1440; //1080;
		appCreateInfo.renderWidth = 1600;// 2560; //1280;
		appCreateInfo.renderHeight = 900;// 1440; //720;
		emberApplication::Application::Init(appCreateInfo);

		// Add project specific shaders:
		std::filesystem::path directoryPath = (std::filesystem::path(PROJECT_SHADERS_DIR) / "bin").make_preferred();
		Material::Create(emberCommon::RenderMode::transparent, "particleMaterial2d", emberCommon::RenderQueue::transparent, directoryPath / "particle2d.vert.spv", directoryPath / "particle2d.frag.spv");
		Material::Create(emberCommon::RenderMode::opaque, "particleMaterial3d", emberCommon::RenderQueue::transparent, directoryPath / "particle3d.vert.spv", directoryPath / "particle3d.frag.spv");
	}
	static void TearDownTestSuite()
	{
		// App shutdown:
		emberApplication::Application::Clear();
	}
};



TEST_F(TEST_SphFluid2dGpu, WithoutHashGrid)
{
	// Basic parameters:
	int particleCount = 200;
	int hashGridSize = math::NextPrimeAbove(2 * particleCount);
	float dt = 0.01;
	float a1 = 1.0f / 3.0f;
	float a2 = 2.0f / 3.0f;
	float q = 3.0f / 4.0f;

	// Cpu arrays:
	std::vector<Float2> positions(particleCount);
	std::vector<Float2> velocities(particleCount);
	std::vector<float> densities(particleCount);
	std::vector<Float2> normals(particleCount);
	std::vector<float> curvatures(particleCount);
	std::vector<Float2> forceDensities(particleCount);
	std::vector<Float2> kp1(particleCount);
	std::vector<Float2> kv1(particleCount);
	std::vector<Float2> tempPositions(particleCount);
	std::vector<Float2> tempVelocities(particleCount);
	fluidDynamics::HashGrid2d hashGrid(particleCount);

	// Gpu buffers:
	BufferTyped<Float2> positionBuffer = BufferTyped<Float2>(particleCount, "positionBuffer", BufferUsage::storage);
	BufferTyped<Float2> velocityBuffer = BufferTyped<Float2>(particleCount, "velocityBuffer", BufferUsage::storage);
	BufferTyped<float> densityBuffer = BufferTyped<float>(particleCount, "densityBuffer", BufferUsage::storage);
	BufferTyped<Float2> normalBuffer = BufferTyped<Float2>(particleCount, "normalBuffer", BufferUsage::storage);
	BufferTyped<float> curvatureBuffer = BufferTyped<float>(particleCount, "curvatureBuffer", BufferUsage::storage);
	BufferTyped<Float2> forceDensityBuffer = BufferTyped<Float2>(particleCount, "forceDensityBuffer", BufferUsage::storage);
	BufferTyped<Float2> kp1Buffer = BufferTyped<Float2>(particleCount, "kp1Buffer", BufferUsage::storage);
	BufferTyped<Float2> kv1Buffer = BufferTyped<Float2>(particleCount, "kv1Buffer", BufferUsage::storage);
	BufferTyped<Float2> tempPositionBuffer = BufferTyped<Float2>(particleCount, "tempPositionBuffer", BufferUsage::storage);
	BufferTyped<Float2> tempVelocityBuffer = BufferTyped<Float2>(particleCount, "tempVelocityBuffer", BufferUsage::storage);
	BufferTyped<uint32_t> cellKeyBuffer = BufferTyped<uint32_t>(particleCount, "cellKeyBuffer", BufferUsage::storage);
	BufferTyped<uint32_t> startIndexBuffer = BufferTyped<uint32_t>(hashGridSize, "startIndexBuffer", BufferUsage::storage);
	// Gpu host vectors for download:
	std::vector<Float2> positionsGpu(particleCount);
	std::vector<Float2> velocitiesGpu(particleCount);
	std::vector<float> densitiesGpu(particleCount);
	std::vector<Float2> normalsGpu(particleCount);
	std::vector<float> curvaturesGpu(particleCount);
	std::vector<Float2> forceDensitiesGpu(particleCount);
	std::vector<Float2> kp1Gpu(particleCount);
	std::vector<Float2> kv1Gpu(particleCount);
	std::vector<Float2> tempPositionsGpu(particleCount);
	std::vector<Float2> tempVelocitiesGpu(particleCount);

	// Data initialization:
	float initialDistributionRadius = 6.0f;
	float phi = math::pi * (math::Sqrt(5.0f) - 1.0f);
	for (int i = 0; i < particleCount; i++)
	{
		float r = i / (particleCount - 1.0f) * initialDistributionRadius;
		float theta = phi * i;
		positions[i].x = math::Cos(theta) * r;
		positions[i].y = math::Sin(theta) * r;
		velocities[i] = Float2::zero;
		densities[i] = 0.0f;
        normals[i] = Float2::zero;
        curvatures[i] = 0.0f;
	}
	positionBuffer.Upload(positions);
	velocityBuffer.Upload(velocities);
	densityBuffer.Upload(densities);
	normalBuffer.Upload(normals);
	curvatureBuffer.Upload(curvatures);

	// Cpu version:
	{
		fluidDynamics::SphFluid2dCpuSolver::Settings cpuSettings;
		cpuSettings.pHashGrid = nullptr;
		cpuSettings.effectRadius = 0.5f;
		cpuSettings.mass = 1.0f;
		cpuSettings.viscosity = 0.5f;
		cpuSettings.surfaceTension = 0.07f;
		cpuSettings.collisionDampening = 0.95f;
		cpuSettings.targetDensity = 15.0f;
		cpuSettings.pressureMultiplier = 20.0f;
		cpuSettings.gravity = 0.5f;
		cpuSettings.maxVelocity = 5.0f;
		cpuSettings.fluidBounds = Bounds(Float3::zero, Float3(16.0f, 9.0f, 0.01f));

		fluidDynamics::SphFluid2dCpuSolver::Attractor cpuAttractor;
		cpuAttractor.radius = 0;
		cpuAttractor.state = 0;
		cpuAttractor.strength = 0;

        // Single simulation step:
		fluidDynamics::SphFluid2dCpuSolver::ComputeDensities(cpuSettings, densities, positions);
		fluidDynamics::SphFluid2dCpuSolver::ComputeNormals(cpuSettings, normals, positions, densities);
		fluidDynamics::SphFluid2dCpuSolver::ComputeCurvatures(cpuSettings, curvatures, positions, densities, normals);
		fluidDynamics::SphFluid2dCpuSolver::ComputeForceDensities(cpuSettings, cpuAttractor, forceDensities, positions, densities, velocities, normals, curvatures);
		// First Runge-Kutta step:
		for (int i = 0; i < particleCount; i++)
		{
			Float2 acceleration = forceDensities[i] / densities[i];
			kp1[i] = velocities[i];
			kv1[i] = acceleration;
			tempPositions[i] = positions[i] + q * kp1[i] * dt;
			tempVelocities[i] = velocities[i] + q * kv1[i] * dt;
			float speed = tempVelocities[i].Length();
			if (speed > cpuSettings.maxVelocity)
				tempVelocities[i] *= (cpuSettings.maxVelocity / speed);
		}
	}

	// Gpu version:
	{
		fluidDynamics::SphFluid2dGpuSolver::Settings gpuSettings;
		gpuSettings.useHashGridOptimization = false;
		gpuSettings.effectRadius = 0.5f;
		gpuSettings.mass = 1.0f;
		gpuSettings.viscosity = 0.5f;
		gpuSettings.surfaceTension = 0.07f;
		gpuSettings.collisionDampening = 0.95f;
		gpuSettings.targetDensity = 15.0f;
		gpuSettings.pressureMultiplier = 20.0f;
		gpuSettings.gravity = 0.5f;
		gpuSettings.maxVelocity = 5.0f;
		gpuSettings.fluidBounds = Bounds(Float3::zero, Float3(16.0f, 9.0f, 0.01f));

		fluidDynamics::SphFluid2dGpuSolver::ComputeShaders computeShaders;
		computeShaders.computeType = ComputeType::immediate;
		computeShaders.SetUseHashGridOptimization(gpuSettings.useHashGridOptimization);
		computeShaders.SetEffectRadius(gpuSettings.effectRadius);
		computeShaders.SetHashGridSize(hashGridSize);
		computeShaders.SetMass(gpuSettings.mass);
		computeShaders.SetViscosity(gpuSettings.viscosity);
		computeShaders.SetSurfaceTension(gpuSettings.surfaceTension);
		computeShaders.SetCollisionDampening(gpuSettings.collisionDampening);
		computeShaders.SetTargetDensity(gpuSettings.targetDensity);
		computeShaders.SetPressureMultiplier(gpuSettings.pressureMultiplier);
		computeShaders.SetGravity(gpuSettings.gravity);
		computeShaders.SetMaxVelocity(gpuSettings.maxVelocity);
		computeShaders.SetFluidBounds(gpuSettings.fluidBounds);

        // Single simulation step:
		fluidDynamics::SphFluid2dGpuSolver::ComputeDensities(computeShaders, densityBuffer.GetBufferView(), positionBuffer.GetBufferView(), startIndexBuffer.GetBufferView(), cellKeyBuffer.GetBufferView());
		fluidDynamics::SphFluid2dGpuSolver::ComputeNormalsAndCurvatures(computeShaders, normalBuffer.GetBufferView(), curvatureBuffer.GetBufferView(), densityBuffer.GetBufferView(), positionBuffer.GetBufferView(), startIndexBuffer.GetBufferView(), cellKeyBuffer.GetBufferView());
		fluidDynamics::SphFluid2dGpuSolver::ComputeForceDensities(computeShaders, forceDensityBuffer.GetBufferView(), densityBuffer.GetBufferView(), positionBuffer.GetBufferView(), velocityBuffer.GetBufferView(), normalBuffer.GetBufferView(), curvatureBuffer.GetBufferView(), startIndexBuffer.GetBufferView(), cellKeyBuffer.GetBufferView());
		fluidDynamics::SphFluid2dGpuSolver::ComputeRungeKutta2Step1(computeShaders, forceDensityBuffer.GetBufferView(), densityBuffer.GetBufferView(), positionBuffer.GetBufferView(), velocityBuffer.GetBufferView(), kp1Buffer.GetBufferView(), kv1Buffer.GetBufferView(), tempPositionBuffer.GetBufferView(), tempVelocityBuffer.GetBufferView());

		positionBuffer.Download(positionsGpu);
		velocityBuffer.Download(velocitiesGpu);
		normalBuffer.Download(normalsGpu);
		curvatureBuffer.Download(curvaturesGpu);
		densityBuffer.Download(densitiesGpu);
		forceDensityBuffer.Download(forceDensitiesGpu);
		kp1Buffer.Download(kp1Gpu);
		kv1Buffer.Download(kv1Gpu);
		tempPositionBuffer.Download(tempPositionsGpu);
		tempVelocityBuffer.Download(tempVelocitiesGpu);
	}

	// Compare results:
	{
		bool allGood = true;
		for (int i = 0; i < particleCount; i++)
		{
			if (!positions[i].IsEpsilonEqual(positionsGpu[i]))
			{
				allGood = false;
				EXPECT_FALSE(true) << "position mismatch at particle " << i << ": cpu = " << positions[i] << ", gpu = " << positionsGpu[i];
			}
			if (!velocities[i].IsEpsilonEqual(velocitiesGpu[i]))
			{
				allGood = false;
				EXPECT_FALSE(true) << "velocity mismatch at particle " << i << ": cpu = " << velocities[i] << ", gpu = " << velocitiesGpu[i];
			}
			if (!normals[i].IsEpsilonEqual(normalsGpu[i]))
			{
				allGood = false;
				EXPECT_FALSE(true) << "normal mismatch at particle " << i << ": cpu = " << normals[i] << ", gpu = " << normalsGpu[i];
			}
			if (!math::IsEpsilonEqual(curvatures[i], curvaturesGpu[i]))
			{
				allGood = false;
				EXPECT_FALSE(true) << "curvature mismatch at particle " << i << ": cpu = " << curvatures[i] << ", gpu = " << curvaturesGpu[i];
			}
			if (!math::IsEpsilonEqual(densities[i], densitiesGpu[i]))
			{
				allGood = false;
				EXPECT_FALSE(true) << "density mismatch at particle " << i << ": cpu = " << densities[i] << ", gpu = " << densitiesGpu[i];
			}
			if (!forceDensities[i].IsEpsilonEqual(forceDensitiesGpu[i]))
			{
				allGood = false;
				EXPECT_FALSE(true) << "forceDensity mismatch at particle " << i << ": cpu = " << forceDensities[i] << ", gpu = " << forceDensitiesGpu[i];
			}
		}
		EXPECT_TRUE(allGood);
	}
}



TEST_F(TEST_SphFluid2dGpu, WithHashGrid)
{
	// Basic parameters:
	int particleCount = 200;
	int hashGridSize = math::NextPrimeAbove(2 * particleCount);
	float dt = 0.01;
	float a1 = 1.0f / 3.0f;
	float a2 = 2.0f / 3.0f;
	float q = 3.0f / 4.0f;

	// Cpu arrays:
	std::vector<Float2> positions(particleCount);
	std::vector<Float2> velocities(particleCount);
	std::vector<float> densities(particleCount);
	std::vector<Float2> normals(particleCount);
	std::vector<float> curvatures(particleCount);
	std::vector<Float2> forceDensities(particleCount);
	std::vector<Float2> kp1(particleCount);
	std::vector<Float2> kv1(particleCount);
	std::vector<Float2> tempPositions(particleCount);
	std::vector<Float2> tempVelocities(particleCount);
	fluidDynamics::HashGrid2d hashGrid(particleCount);

	// Gpu buffers:
	BufferTyped<Float2> positionBuffer = BufferTyped<Float2>(particleCount, "positionBuffer", BufferUsage::storage);
	BufferTyped<Float2> velocityBuffer = BufferTyped<Float2>(particleCount, "velocityBuffer", BufferUsage::storage);
	BufferTyped<float> densityBuffer = BufferTyped<float>(particleCount, "densityBuffer", BufferUsage::storage);
	BufferTyped<Float2> normalBuffer = BufferTyped<Float2>(particleCount, "normalBuffer", BufferUsage::storage);
	BufferTyped<float> curvatureBuffer = BufferTyped<float>(particleCount, "curvatureBuffer", BufferUsage::storage);
	BufferTyped<Float2> forceDensityBuffer = BufferTyped<Float2>(particleCount, "forceDensityBuffer", BufferUsage::storage);
	BufferTyped<Float2> kp1Buffer = BufferTyped<Float2>(particleCount, "kp1Buffer", BufferUsage::storage);
	BufferTyped<Float2> kv1Buffer = BufferTyped<Float2>(particleCount, "kv1Buffer", BufferUsage::storage);
	BufferTyped<Float2> tempPositionBuffer = BufferTyped<Float2>(particleCount, "tempPositionBuffer", BufferUsage::storage);
	BufferTyped<Float2> tempVelocityBuffer = BufferTyped<Float2>(particleCount, "tempVelocityBuffer", BufferUsage::storage);
	BufferTyped<uint32_t> cellKeyBuffer = BufferTyped<uint32_t>(particleCount, "cellKeyBuffer", BufferUsage::storage);
	BufferTyped<uint32_t> startIndexBuffer = BufferTyped<uint32_t>(hashGridSize, "startIndexBuffer", BufferUsage::storage);
	BufferTyped<uint32_t> sortPermutationBuffer = BufferTyped<uint32_t>(particleCount, "sortPermutationBuffer", BufferUsage::storage);
	BufferTyped<uint32_t> inverseSortPermutationBuffer = BufferTyped<uint32_t>(particleCount, "inverseSortPermutationBuffer", BufferUsage::storage);
	BufferTyped<Float2> tempBuffer0 = BufferTyped<Float2>(particleCount, "tempBuffer0", BufferUsage::storage);
	BufferTyped<float> tempBuffer1 = BufferTyped<float>(particleCount, "tempBuffer1", BufferUsage::storage);
	// Gpu host vectors for download:
	std::vector<Float2> positionsGpu(particleCount);
	std::vector<Float2> velocitiesGpu(particleCount);
	std::vector<float> densitiesGpu(particleCount);
	std::vector<Float2> normalsGpu(particleCount);
	std::vector<float> curvaturesGpu(particleCount);
	std::vector<Float2> forceDensitiesGpu(particleCount);
	std::vector<Float2> kp1Gpu(particleCount);
	std::vector<Float2> kv1Gpu(particleCount);
	std::vector<Float2> tempPositionsGpu(particleCount);
	std::vector<Float2> tempVelocitiesGpu(particleCount);

	// Data initialization:
	float initialDistributionRadius = 6.0f;
	float phi = math::pi * (math::Sqrt(5.0f) - 1.0f);
	for (int i = 0; i < particleCount; i++)
	{
		float r = i / (particleCount - 1.0f) * initialDistributionRadius;
		float theta = phi * i;
		positions[i].x = math::Cos(theta) * r;
		positions[i].y = math::Sin(theta) * r;
		velocities[i] = Float2::zero;
		densities[i] = 0.0f;
        normals[i] = Float2::zero;
        curvatures[i] = 0.0f;
	}
	positionBuffer.Upload(positions);
	velocityBuffer.Upload(velocities);
	densityBuffer.Upload(densities);
	normalBuffer.Upload(normals);
	curvatureBuffer.Upload(curvatures);

	// Cpu version:
	{
		fluidDynamics::SphFluid2dCpuSolver::Settings cpuSettings;
		cpuSettings.pHashGrid = &hashGrid;
		cpuSettings.effectRadius = 0.5f;
		cpuSettings.mass = 1.0f;
		cpuSettings.viscosity = 0.5f;
		cpuSettings.surfaceTension = 0.07f;
		cpuSettings.collisionDampening = 0.95f;
		cpuSettings.targetDensity = 15.0f;
		cpuSettings.pressureMultiplier = 20.0f;
		cpuSettings.gravity = 0.5f;
		cpuSettings.maxVelocity = 5.0f;
		cpuSettings.fluidBounds = Bounds(Float3::zero, Float3(16.0f, 9.0f, 0.01f));

		fluidDynamics::SphFluid2dCpuSolver::Attractor cpuAttractor;
		cpuAttractor.radius = 0;
		cpuAttractor.state = 0;
		cpuAttractor.strength = 0;

        // Single simulation step:
		hashGrid.UpdateGrid(positions, cpuSettings.effectRadius);
		hashGrid.ApplySort(positions);
		hashGrid.ApplySort(velocities);
		fluidDynamics::SphFluid2dCpuSolver::ComputeDensities(cpuSettings, densities, positions);
		fluidDynamics::SphFluid2dCpuSolver::ComputeNormals(cpuSettings, normals, positions, densities);
		fluidDynamics::SphFluid2dCpuSolver::ComputeCurvatures(cpuSettings, curvatures, positions, densities, normals);
		fluidDynamics::SphFluid2dCpuSolver::ComputeForceDensities(cpuSettings, cpuAttractor, forceDensities, positions, densities, velocities, normals, curvatures);
		// First Runge-Kutta step:
		for (int i = 0; i < particleCount; i++)
		{
			Float2 acceleration = forceDensities[i] / densities[i];
			kp1[i] = velocities[i];
			kv1[i] = acceleration;
			tempPositions[i] = positions[i] + q * kp1[i] * dt;
			tempVelocities[i] = velocities[i] + q * kv1[i] * dt;
			float speed = tempVelocities[i].Length();
			if (speed > cpuSettings.maxVelocity)
				tempVelocities[i] *= (cpuSettings.maxVelocity / speed);
		}
        
		// Undo sorting (for comparison):
		hashGrid.UndoSort(positions);
		hashGrid.UndoSort(velocities);
		hashGrid.UndoSort(normals);
		hashGrid.UndoSort(curvatures);
		hashGrid.UndoSort(densities);
		hashGrid.UndoSort(forceDensities);
	}

	// Gpu version:
	{
		fluidDynamics::SphFluid2dGpuSolver::Settings gpuSettings;
		gpuSettings.useHashGridOptimization = true;
		gpuSettings.effectRadius = 0.5f;
		gpuSettings.mass = 1.0f;
		gpuSettings.viscosity = 0.5f;
		gpuSettings.surfaceTension = 0.07f;
		gpuSettings.collisionDampening = 0.95f;
		gpuSettings.targetDensity = 15.0f;
		gpuSettings.pressureMultiplier = 20.0f;
		gpuSettings.gravity = 0.5f;
		gpuSettings.maxVelocity = 5.0f;
		gpuSettings.fluidBounds = Bounds(Float3::zero, Float3(16.0f, 9.0f, 0.01f));

		fluidDynamics::SphFluid2dGpuSolver::ComputeShaders computeShaders;
		computeShaders.computeType = ComputeType::immediate;
		computeShaders.SetUseHashGridOptimization(gpuSettings.useHashGridOptimization);
		computeShaders.SetEffectRadius(gpuSettings.effectRadius);
		computeShaders.SetHashGridSize(hashGridSize);
		computeShaders.SetMass(gpuSettings.mass);
		computeShaders.SetViscosity(gpuSettings.viscosity);
		computeShaders.SetSurfaceTension(gpuSettings.surfaceTension);
		computeShaders.SetCollisionDampening(gpuSettings.collisionDampening);
		computeShaders.SetTargetDensity(gpuSettings.targetDensity);
		computeShaders.SetPressureMultiplier(gpuSettings.pressureMultiplier);
		computeShaders.SetGravity(gpuSettings.gravity);
		computeShaders.SetMaxVelocity(gpuSettings.maxVelocity);
		computeShaders.SetFluidBounds(gpuSettings.fluidBounds);

		// Hash grid setup:
		fluidDynamics::SphFluid2dGpuSolver::ComputeCellKeys(computeShaders, cellKeyBuffer.GetBufferView(), positionBuffer.GetBufferView());
		GpuSort<uint32_t>::SortPermutation(ComputeType::immediate, cellKeyBuffer.GetBufferView(), sortPermutationBuffer.GetBufferView());
		fluidDynamics::SphFluid2dGpuSolver::ComputeStartIndices(computeShaders, startIndexBuffer.GetBufferView(), cellKeyBuffer.GetBufferView());
		GpuSort<Float2>::ApplyPermutation(ComputeType::immediate, sortPermutationBuffer.GetBufferView(), positionBuffer.GetBufferView(), tempBuffer0.GetBufferView());
		std::swap(positionBuffer, tempBuffer0);
		GpuSort<Float2>::ApplyPermutation(ComputeType::immediate, sortPermutationBuffer.GetBufferView(), velocityBuffer.GetBufferView(), tempBuffer0.GetBufferView());
		std::swap(velocityBuffer, tempBuffer0);

        // Single simulation step:
		fluidDynamics::SphFluid2dGpuSolver::ComputeDensities(computeShaders, densityBuffer.GetBufferView(), positionBuffer.GetBufferView(), startIndexBuffer.GetBufferView(), cellKeyBuffer.GetBufferView());
		fluidDynamics::SphFluid2dGpuSolver::ComputeNormalsAndCurvatures(computeShaders, normalBuffer.GetBufferView(), curvatureBuffer.GetBufferView(), densityBuffer.GetBufferView(), positionBuffer.GetBufferView(), startIndexBuffer.GetBufferView(), cellKeyBuffer.GetBufferView());
		fluidDynamics::SphFluid2dGpuSolver::ComputeForceDensities(computeShaders, forceDensityBuffer.GetBufferView(), densityBuffer.GetBufferView(), positionBuffer.GetBufferView(), velocityBuffer.GetBufferView(), normalBuffer.GetBufferView(), curvatureBuffer.GetBufferView(), startIndexBuffer.GetBufferView(), cellKeyBuffer.GetBufferView());
		fluidDynamics::SphFluid2dGpuSolver::ComputeRungeKutta2Step1(computeShaders, forceDensityBuffer.GetBufferView(), densityBuffer.GetBufferView(), positionBuffer.GetBufferView(), velocityBuffer.GetBufferView(), kp1Buffer.GetBufferView(), kv1Buffer.GetBufferView(), tempPositionBuffer.GetBufferView(), tempVelocityBuffer.GetBufferView());

		// Undo sorting (for comparison):	
		GpuSort<uint32_t>::InvertPermutation(ComputeType::immediate, sortPermutationBuffer.GetBufferView(), inverseSortPermutationBuffer.GetBufferView());
		GpuSort<Float2>::ApplyPermutation(ComputeType::immediate, inverseSortPermutationBuffer.GetBufferView(), positionBuffer.GetBufferView(), tempBuffer0.GetBufferView());
		std::swap(positionBuffer, tempBuffer0);
        GpuSort<Float2>::ApplyPermutation(ComputeType::immediate, inverseSortPermutationBuffer.GetBufferView(), velocityBuffer.GetBufferView(), tempBuffer0.GetBufferView());
		std::swap(velocityBuffer, tempBuffer0);
        GpuSort<Float2>::ApplyPermutation(ComputeType::immediate, inverseSortPermutationBuffer.GetBufferView(), normalBuffer.GetBufferView(), tempBuffer0.GetBufferView());
		std::swap(normalBuffer, tempBuffer0);
        GpuSort<float>::ApplyPermutation(ComputeType::immediate, inverseSortPermutationBuffer.GetBufferView(), curvatureBuffer.GetBufferView(), tempBuffer1.GetBufferView());
		std::swap(curvatureBuffer, tempBuffer1);
        GpuSort<float>::ApplyPermutation(ComputeType::immediate, inverseSortPermutationBuffer.GetBufferView(), densityBuffer.GetBufferView(), tempBuffer1.GetBufferView());
		std::swap(densityBuffer, tempBuffer1);
        GpuSort<Float2>::ApplyPermutation(ComputeType::immediate, inverseSortPermutationBuffer.GetBufferView(), forceDensityBuffer.GetBufferView(), tempBuffer0.GetBufferView());
		std::swap(forceDensityBuffer, tempBuffer0);

		positionBuffer.Download(positionsGpu);
		velocityBuffer.Download(velocitiesGpu);
		normalBuffer.Download(normalsGpu);
		curvatureBuffer.Download(curvaturesGpu);
		densityBuffer.Download(densitiesGpu);
		forceDensityBuffer.Download(forceDensitiesGpu);
		kp1Buffer.Download(kp1Gpu);
		kv1Buffer.Download(kv1Gpu);
		tempPositionBuffer.Download(tempPositionsGpu);
		tempVelocityBuffer.Download(tempVelocitiesGpu);
	}

	// Compare results:
	{
		bool allGood = true;
		for (int i = 0; i < particleCount; i++)
		{
			if (!positions[i].IsEpsilonEqual(positionsGpu[i]))
			{
				allGood = false;
				EXPECT_FALSE(true) << "position mismatch at particle " << i << ": cpu = " << positions[i] << ", gpu = " << positionsGpu[i];
			}
			if (!velocities[i].IsEpsilonEqual(velocitiesGpu[i]))
			{
				allGood = false;
				EXPECT_FALSE(true) << "velocity mismatch at particle " << i << ": cpu = " << velocities[i] << ", gpu = " << velocitiesGpu[i];
			}
			if (!normals[i].IsEpsilonEqual(normalsGpu[i]))
			{
				allGood = false;
				EXPECT_FALSE(true) << "normal mismatch at particle " << i << ": cpu = " << normals[i] << ", gpu = " << normalsGpu[i];
			}
			if (!math::IsEpsilonEqual(curvatures[i], curvaturesGpu[i]))
			{
				allGood = false;
				EXPECT_FALSE(true) << "curvature mismatch at particle " << i << ": cpu = " << curvatures[i] << ", gpu = " << curvaturesGpu[i];
			}
			if (!math::IsEpsilonEqual(densities[i], densitiesGpu[i]))
			{
				allGood = false;
				EXPECT_FALSE(true) << "density mismatch at particle " << i << ": cpu = " << densities[i] << ", gpu = " << densitiesGpu[i];
			}
			if (!forceDensities[i].IsEpsilonEqual(forceDensitiesGpu[i]))
			{
				allGood = false;
				EXPECT_FALSE(true) << "forceDensity mismatch at particle " << i << ": cpu = " << forceDensities[i] << ", gpu = " << forceDensitiesGpu[i];
			}
		}
		EXPECT_TRUE(allGood);
	}
}