#pragma once
#include "application.h"
#include "emberEngine.h"
#include "sphFluid2dCpuSolver.h"
#include "sphFluid2dGpuSolver.h"



class TEST_SphGluid2dGpu : public ::testing::Test
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
		Material::Create(emberCommon::MaterialType::forwardTransparent, "particleMaterial2d", emberCommon::RenderQueue::transparent, directoryPath / "particle2d.vert.spv", directoryPath / "particle2d.frag.spv");
		Material::Create(emberCommon::MaterialType::forwardOpaque, "particleMaterial3d", emberCommon::RenderQueue::transparent, directoryPath / "particle3d.vert.spv", directoryPath / "particle3d.frag.spv");
	}
	static void TearDownTestSuite()
	{
		// App shutdown:
		emberApplication::Application::Clear();
	}
};



TEST_F(TEST_SphGluid2dGpu, Density)
{
	// Basic parameters:
	int particleCount = 1000;
	int hashGridSize = math::NextPrimeAbove(2 * particleCount);

	// Cpu buffers:
	std::vector<Float2> positions(particleCount);
	std::vector<float> densities(particleCount);
	fluidDynamics::HashGrid2d hashGrid(particleCount);

	// Gpu buffers:
	std::vector<float> densitiesGpu(particleCount);	// for downloading results and comparing to cpu version.
	BufferTyped<Float2> positionBuffer = BufferTyped<Float2>(particleCount, "positionBuffer", BufferUsage::storage);
	BufferTyped<float> densityBuffer = BufferTyped<float>(particleCount, "densityBuffer", BufferUsage::storage);
	BufferTyped<int> cellKeyBuffer = BufferTyped<int>(particleCount, "cellKeyBuffer", BufferUsage::storage);
	BufferTyped<int> startIndexBuffer = BufferTyped<int>(hashGridSize, "startIndexBuffer", BufferUsage::storage);

	// Data initialization:
	float initialDistributionRadius = 6.0f;
	float phi = math::pi * (math::Sqrt(5.0f) - 1.0f);
	for (int i = 0; i < particleCount; i++)
	{
		float r = i / (particleCount - 1.0f) * initialDistributionRadius;
		float theta = phi * i;
		positions[i].x = math::Cos(theta) * r;
		positions[i].y = math::Sin(theta) * r;
		densities[i] = 0.0f;
	}
	positionBuffer.Upload(positions.data(), positions.size());
	densityBuffer.Upload(densities.data(), densities.size());

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

		fluidDynamics::SphFluid2dCpuSolver::ComputeDensities(cpuSettings, densities, positions);
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
	
		fluidDynamics::SphFluid2dGpuSolver::ComputeDensities(computeShaders, densityBuffer.GetBufferView(), positionBuffer.GetBufferView(), startIndexBuffer.GetBufferView(), cellKeyBuffer.GetBufferView());
		densityBuffer.Download(densitiesGpu.data(), densitiesGpu.size() * sizeof(float));
	}

	// Compare results:
	{
		bool allGood = true;
		for (int i = 0; i < particleCount; i++)
		{
			if (!math::IsEpsilonEqual(densities[i], densitiesGpu[i], 1e-3f))
			{
				allGood = false;
				EXPECT_FALSE(true) << "Density mismatch at particle " << i << ": cpu = " << densities[i] << ", gpu = " << densitiesGpu[i];
			}
		}
		EXPECT_TRUE(allGood);
	}
}



TEST_F(TEST_SphGluid2dGpu, DensityHashGrid)
{
	// Basic parameters:
	int particleCount = 1000;
	int hashGridSize = math::NextPrimeAbove(2 * particleCount);

	// Cpu buffers:
	std::vector<Float2> positions(particleCount);
	std::vector<float> densities(particleCount);
	fluidDynamics::HashGrid2d hashGrid(particleCount);

	// Gpu buffers:
	std::vector<float> densitiesGpu(particleCount);		// for downloading results and comparing to cpu version.
	std::vector<Float2> positionsGpu(particleCount);	// for downloading results and comparing to cpu version.
	BufferTyped<Float2> positionBuffer = BufferTyped<Float2>(particleCount, "positionBuffer", BufferUsage::storage);
	BufferTyped<float> densityBuffer = BufferTyped<float>(particleCount, "densityBuffer", BufferUsage::storage);
	BufferTyped<int> cellKeyBuffer = BufferTyped<int>(particleCount, "cellKeyBuffer", BufferUsage::storage);
	BufferTyped<int> startIndexBuffer = BufferTyped<int>(hashGridSize, "startIndexBuffer", BufferUsage::storage);
	BufferTyped<uint32_t> sortPermutationBuffer = BufferTyped<uint32_t>(particleCount, "sortPermutationBuffer", BufferUsage::storage);
	BufferTyped<uint32_t> inverseSortPermutationBuffer = BufferTyped<uint32_t>(particleCount, "inverseSortPermutationBuffer", BufferUsage::storage);
	BufferTyped<Float2> tempBuffer0 = BufferTyped<Float2>(particleCount, "tempBuffer0", BufferUsage::storage);
	BufferTyped<float> tempBuffer1 = BufferTyped<float>(particleCount, "tempBuffer1", BufferUsage::storage);

	// Data initialization:
	float initialDistributionRadius = 6.0f;
	float phi = math::pi * (math::Sqrt(5.0f) - 1.0f);
	for (int i = 0; i < particleCount; i++)
	{
		float r = i / (particleCount - 1.0f) * initialDistributionRadius;
		float theta = phi * i;
		positions[i].x = math::Cos(theta) * r;
		positions[i].y = math::Sin(theta) * r;
		densities[i] = 0.0f;
	}
	positionBuffer.Upload(positions.data(), positions.size());
	densityBuffer.Upload(densities.data(), densities.size());

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

		// Compute densities:
		hashGrid.UpdateGrid(positions, cpuSettings.effectRadius);
		hashGrid.ApplySort(positions);
		fluidDynamics::SphFluid2dCpuSolver::ComputeDensities(cpuSettings, densities, positions);

		// Undo sorting (for comparison):
		hashGrid.UndoSort(positions);
		hashGrid.UndoSort(densities);
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
		GpuSort<int>::SortPermutation(ComputeType::immediate, cellKeyBuffer.GetBufferView(), sortPermutationBuffer.GetBufferView());
		fluidDynamics::SphFluid2dGpuSolver::ComputeStartIndices(computeShaders, startIndexBuffer.GetBufferView(), cellKeyBuffer.GetBufferView());
		GpuSort<Float2>::ApplyPermutation(ComputeType::immediate, sortPermutationBuffer.GetBufferView(), positionBuffer.GetBufferView(), tempBuffer0.GetBufferView());
		std::swap(positionBuffer, tempBuffer0);

		// Compute densities:
		fluidDynamics::SphFluid2dGpuSolver::ComputeDensities(computeShaders, densityBuffer.GetBufferView(), positionBuffer.GetBufferView(), startIndexBuffer.GetBufferView(), cellKeyBuffer.GetBufferView());

		// Undo sorting (for comparison):
		GpuSort<int>::InvertPermutation(ComputeType::immediate, sortPermutationBuffer.GetBufferView(), inverseSortPermutationBuffer.GetBufferView());
		GpuSort<Float2>::ApplyPermutation(ComputeType::immediate, inverseSortPermutationBuffer.GetBufferView(), positionBuffer.GetBufferView(), tempBuffer0.GetBufferView());
		GpuSort<float>::ApplyPermutation(ComputeType::immediate, inverseSortPermutationBuffer.GetBufferView(), densityBuffer.GetBufferView(), tempBuffer1.GetBufferView());
		std::swap(positionBuffer, tempBuffer0);
		std::swap(densityBuffer, tempBuffer1);

		// Download results:
		positionBuffer.Download(positionsGpu.data(), positionsGpu.size() * sizeof(Float2));
		densityBuffer.Download(densitiesGpu.data(), densitiesGpu.size() * sizeof(float));
	}

	// Compare results:
	{
		bool allGood = true;
		for (int i = 0; i < particleCount; i++)
		{
			if (!math::IsEpsilonEqual(densities[i], densitiesGpu[i], 1e-3f))
			{
				allGood = false;
				EXPECT_FALSE(true) << "Density mismatch at particle " << i << ": cpu = " << densities[i] << ", gpu = " << densitiesGpu[i] <<", ratio = " << densitiesGpu[i] / densities[i] << ", cpuPos = " << positions[i] << ", gpuPos = " << positionsGpu[i];
			}
		}
		EXPECT_TRUE(allGood);
	}
}