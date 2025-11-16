#include "SphFluid2dCpuSolver.h"
#include "hashGrid2d.h"
#include "smoothingKernals.h"



namespace fluidDynamics
{
	// Internal constants:
	std::array<Int2, 9> offsets =
	{
		Int2(-1, -1), Int2(-1, 0), Int2(-1, 1),
		Int2( 0, -1), Int2( 0, 0), Int2( 0, 1),
		Int2( 1, -1), Int2( 1, 0), Int2( 1, 1)
	};



	// Public structs:
	int SphFluid2dCpuSolver::Data::ParticleCount()
	{
		return static_cast<int>(positions.size());
	}
	void SphFluid2dCpuSolver::Data::Reallocate(int particleCount)
	{
		// Reallocate:
		positions.resize(particleCount);
		velocities.resize(particleCount);
		densities.resize(particleCount);
		normals.resize(particleCount);
		curvatures.resize(particleCount);
		forceDensities.resize(particleCount);

		// Reset all entries to zero:
		std::fill(positions.begin(), positions.end(), Float2::zero);
		std::fill(velocities.begin(), velocities.end(), Float2::zero);
		std::fill(densities.begin(), densities.end(), 0.0f);
		std::fill(normals.begin(), normals.end(), Float2::zero);
		std::fill(curvatures.begin(), curvatures.end(), 0.0f);
		std::fill(forceDensities.begin(), forceDensities.end(), Float2::zero);
	}
	void SphFluid2dCpuSolver::RungeKutta::Reallocate(int particleCount)
	{
		// Reallocate:
		kp1.resize(particleCount);
		kv1.resize(particleCount);
		kp2.resize(particleCount);
		kv2.resize(particleCount);
		tempPositions.resize(particleCount);
		tempVelocities.resize(particleCount);

		// Reset all entries to zero:
		std::fill(kp1.begin(), kp1.end(), Float2::zero);
		std::fill(kv1.begin(), kv1.end(), Float2::zero);
		std::fill(kp2.begin(), kp2.end(), Float2::zero);
		std::fill(kv2.begin(), kv2.end(), Float2::zero);
		std::fill(tempPositions.begin(), tempPositions.end(), Float2::zero);
		std::fill(tempVelocities.begin(), tempVelocities.end(), Float2::zero);
	}



	// Public methods:
	// Ordinary Differential Equation Solvers:
	void SphFluid2dCpuSolver::TimeStepLeapFrog(float dt, int timeStep, Data& data, const Settings& settings, const Attractor& attractor)
	{
		// Update grid for fast nearest neighbor particle look up:
		if (settings.pHashGrid != nullptr)
		{
			settings.pHashGrid->UpdateGrid(data.positions, settings.effectRadius);
			settings.pHashGrid->ApplySort(data.positions);
			settings.pHashGrid->ApplySort(data.velocities);
		}

		// Compute force densities:
		ComputeDensities(settings, data.densities, data.positions);
		ComputeNormals(settings, data.normals, data.positions, data.densities);
		ComputeCurvatures(settings, data.curvatures, data.positions, data.densities, data.normals);
		ComputeForceDensities(settings, attractor, data.forceDensities, data.positions, data.densities, data.velocities);

		// Step:
		if (timeStep == 0)
		{
			for (int i = 0; i < data.ParticleCount(); i++)
			{
				Float2 acceleration = data.forceDensities[i] / data.densities[i];
				data.velocities[i] += 0.5f * acceleration * dt;
			}
		}
		for (int i = 0; i < data.ParticleCount(); i++)
		{
			Float2 acceleration = data.forceDensities[i] / data.densities[i];
			data.positions[i] += data.velocities[i] * dt;
			data.velocities[i] += acceleration * dt;
			float speed = data.velocities[i].Length();
			if (speed > settings.maxVelocity)
				data.velocities[i] *= (settings.maxVelocity / speed);
			BoundaryCollisions(data.positions[i], data.velocities[i], settings);
		}
	}
	void SphFluid2dCpuSolver::TimeStepVelocityVerlet(float dt, Data& data, const Settings& settings, const Attractor& attractor)
	{
		// Update grid for fast nearest neighbor particle look up:
		if (settings.pHashGrid != nullptr)
		{
			settings.pHashGrid->UpdateGrid(data.positions, settings.effectRadius);
			settings.pHashGrid->ApplySort(data.positions);
			settings.pHashGrid->ApplySort(data.velocities);
		}

		// Compute force densities:
		ComputeDensities(settings, data.densities, data.positions);
		ComputeNormals(settings, data.normals, data.positions, data.densities);
		ComputeCurvatures(settings, data.curvatures, data.positions, data.densities, data.normals);
		ComputeForceDensities(settings, attractor, data.forceDensities, data.positions, data.densities, data.velocities);

		// Step:
		for (int i = 0; i < data.ParticleCount(); i++)
		{
			Float2 acceleration = data.forceDensities[i] / data.densities[i];
			Float2 velocityHalfStep = data.velocities[i] + 0.5f * acceleration * dt;
			data.positions[i] += velocityHalfStep * dt;
			data.velocities[i] = velocityHalfStep + 0.5f * acceleration * dt;
			float speed = data.velocities[i].Length();
			if (speed > settings.maxVelocity)
				data.velocities[i] *= (settings.maxVelocity / speed);
			BoundaryCollisions(data.positions[i], data.velocities[i], settings);
		}
	}
	void SphFluid2dCpuSolver::TimeStepRungeKutta2(float dt, Data& data, const Settings& settings, const Attractor& attractor, RungeKutta& rungeKutta)
	{
		// Runge Kutta coefficients:
		constexpr float a1 = 1.0f / 3.0f;
		constexpr float a2 = 2.0f / 3.0f;
		constexpr float q = 3.0f / 4.0f;

		// Update hash grid for fast nearest neighbor particle look up:
		if (settings.pHashGrid != nullptr)
		{
			settings.pHashGrid->UpdateGrid(data.positions, settings.effectRadius);
			settings.pHashGrid->ApplySort(data.positions);
			settings.pHashGrid->ApplySort(data.velocities);
		}

		// Compute force densities:
		ComputeDensities(settings, data.densities, data.positions);
		ComputeNormals(settings, data.normals, data.positions, data.densities);
		ComputeCurvatures(settings, data.curvatures, data.positions, data.densities, data.normals);
		ComputeForceDensities(settings, attractor, data.forceDensities, data.positions, data.densities, data.velocities);

		// First Runte-Kutta step:
		for (int i = 0; i < data.ParticleCount(); i++)
		{
			Float2 acceleration = data.forceDensities[i] / data.densities[i];
			rungeKutta.kp1[i] = data.velocities[i];
			rungeKutta.kv1[i] = acceleration;
			rungeKutta.tempPositions[i] = data.positions[i] + q * rungeKutta.kp1[i] * dt;
			rungeKutta.tempVelocities[i] = data.velocities[i] + q * rungeKutta.kv1[i] * dt;
			float speed = rungeKutta.tempVelocities[i].Length();
			if (speed > settings.maxVelocity)
				rungeKutta.tempVelocities[i] *= (settings.maxVelocity / speed);
		}

		// Update hash grid for fast nearest neighbor particle look up:
		if (settings.pHashGrid != nullptr)
		{
			settings.pHashGrid->UpdateGrid(rungeKutta.tempPositions, settings.effectRadius);
			settings.pHashGrid->ApplySort(rungeKutta.tempPositions);
			settings.pHashGrid->ApplySort(rungeKutta.tempVelocities);
			settings.pHashGrid->ApplySort(data.positions);
			settings.pHashGrid->ApplySort(data.velocities);
			settings.pHashGrid->ApplySort(rungeKutta.kv1);
		}

		// Compute force densities:
		ComputeDensities(settings, data.densities, rungeKutta.tempPositions);
		ComputeNormals(settings, data.normals, rungeKutta.tempPositions, data.densities);
		ComputeCurvatures(settings, data.curvatures, rungeKutta.tempPositions, data.densities, data.normals);
		ComputeForceDensities(settings, attractor, data.forceDensities, rungeKutta.tempPositions, data.densities, rungeKutta.tempVelocities);

		// Second Runge-Kutta step:
		for (int i = 0; i < data.ParticleCount(); i++)
		{
			Float2 acceleration = data.forceDensities[i] / data.densities[i];
			rungeKutta.kp2[i] = rungeKutta.tempVelocities[i];
			rungeKutta.kv2[i] = acceleration;
			data.positions[i] += (a1 * rungeKutta.kp1[i] + a2 * rungeKutta.kp2[i]) * dt;
			data.velocities[i] += (a1 * rungeKutta.kv1[i] + a2 * rungeKutta.kv2[i]) * dt;
			float speed = data.velocities[i].Length();
			if (speed > settings.maxVelocity)
				data.velocities[i] *= (settings.maxVelocity / speed);
			BoundaryCollisions(data.positions[i], data.velocities[i], settings);
		}
	}



	// Particle computations:
	float SphFluid2dCpuSolver::Density(int particleIndex, const Settings& settings, const std::vector<Float2>& positions)
	{
		float density = 0;
		for (int i = 0; i < positions.size(); i++)
		{
			Float2 offset = positions[particleIndex] - positions[i];
			float r = offset.Length();
			if (r < settings.effectRadius)
				density += settings.mass * smoothingKernals::Poly6(r, settings.effectRadius);
		}
		return density;
	}
	float SphFluid2dCpuSolver::Density(int particleIndex, const Settings& settings, const std::vector<Float2>& positions, HashGrid2d& hashGrid)
	{
		float density = 0;
		Int2 particleCell = hashGrid.Cell(positions[particleIndex], settings.effectRadius);
		for (const Int2& offset : offsets)
		{
			Int2 neighbourCell = particleCell + offset;
			const uint32_t cellKey = hashGrid.GetCellKey(neighbourCell);
			uint32_t otherIndex = hashGrid.GetStartIndex(neighbourCell);

			// Skip empty cells:
			if (otherIndex == uint32_t(-1) || otherIndex >= positions.size())
				continue;

			while (otherIndex < positions.size() && hashGrid.GetCellKey(otherIndex) == cellKey)
			{
				Float2 offset = positions[particleIndex] - positions[otherIndex];
				float r = offset.Length();
				if (r < settings.effectRadius)
					density += settings.mass * smoothingKernals::Poly6(r, settings.effectRadius);
				otherIndex++;
			}
		}
		return density;
	}
	Float2 SphFluid2dCpuSolver::Normal(int particleIndex, const Settings& settings, const std::vector<Float2>& positions, const std::vector<float>& densities)
	{
		Float2 normal = Float2::zero;
		for (int i = 0; i < positions.size(); i++)
		{
			if (i == particleIndex)
				continue;

			Float2 offset = positions[particleIndex] - positions[i];
			float r = offset.Length();
			if (r < settings.effectRadius && r > 1e-8f)
			{
				Float2 dir = offset / r;
				normal += settings.mass / densities[i] * smoothingKernals::DSpiky(r, dir, settings.effectRadius);
			}
		}
		return normal;
	}
	Float2 SphFluid2dCpuSolver::Normal(int particleIndex, const Settings& settings, const std::vector<Float2>& positions, const std::vector<float>& densities, HashGrid2d& hashGrid)
	{
		Float2 normal = Float2::zero;
		Int2 particleCell = hashGrid.Cell(positions[particleIndex], settings.effectRadius);
		for (const Int2& offset : offsets)
		{
			Int2 neighbourCell = particleCell + offset;
			const uint32_t cellKey = hashGrid.GetCellKey(neighbourCell);
			uint32_t otherIndex = hashGrid.GetStartIndex(neighbourCell);

			// Skip empty cells:
			if (otherIndex == uint32_t(-1) || otherIndex >= positions.size())
				continue;

			while (otherIndex < positions.size() && hashGrid.GetCellKey(otherIndex) == cellKey)
			{
				if (otherIndex == particleIndex)
				{
					otherIndex++;
					continue;
				}

				Float2 offset = positions[particleIndex] - positions[otherIndex];
				float r = offset.Length();
				if (r < settings.effectRadius && r > 1e-8f)
				{
					Float2 dir = offset / r;
					normal += settings.mass / densities[otherIndex] * smoothingKernals::DSpiky(r, dir, settings.effectRadius);
				}
				otherIndex++;
			}
		}
		return normal;
	}
	float SphFluid2dCpuSolver::Curvature(int particleIndex, const Settings& settings, const std::vector<Float2>& positions, const std::vector<float>& densities, const std::vector<Float2>& normals)
	{
		float curvature = 0.0f;
		for (int i = 0; i < positions.size(); i++)
		{
			if (i == particleIndex)
				continue;

			Float2 offset = positions[particleIndex] - positions[i];
			float r = offset.Length();
			if (r < settings.effectRadius && r > 1e-8f)
				curvature -= settings.mass / densities[i] * smoothingKernals::DDSpiky(r, settings.effectRadius) / normals[i].Length();
		}
		return curvature;
	}
	float SphFluid2dCpuSolver::Curvature(int particleIndex, const Settings& settings, const std::vector<Float2>& positions, const std::vector<float>& densities, const std::vector<Float2>& normals, HashGrid2d& hashGrid)
	{
		float curvature = 0.0f;
		Int2 particleCell = hashGrid.Cell(positions[particleIndex], settings.effectRadius);
		for (const Int2& offset : offsets)
		{
			Int2 neighbourCell = particleCell + offset;
			const uint32_t cellKey = hashGrid.GetCellKey(neighbourCell);
			uint32_t otherIndex = hashGrid.GetStartIndex(neighbourCell);

			// Skip empty cells:
			if (otherIndex == uint32_t(-1) || otherIndex >= positions.size())
				continue;

			while (otherIndex < positions.size() && hashGrid.GetCellKey(otherIndex) == cellKey)
			{
				if (otherIndex == particleIndex)
				{
					otherIndex++;
					continue;
				}

				Float2 offset = positions[particleIndex] - positions[otherIndex];
				float r = offset.Length();
				if (r < settings.effectRadius && r > 1e-8f)
					curvature -= settings.mass / densities[otherIndex] * smoothingKernals::DDSpiky(r, settings.effectRadius) / normals[otherIndex].Length();
				otherIndex++;
			}
		}
		return curvature;
	}
	float Pressure(float density, float targetDensity, float pressureMultiplier)
	{
		float densityError = density - targetDensity;
		return densityError * pressureMultiplier;
		//return pressureMultiplier * (math::Pow(density / targetDensity, 7.0f) - 1.0f);
	}
	Float2 SphFluid2dCpuSolver::InternalForceDensity(int particleIndex, const Settings& settings, const std::vector<Float2>& positions, const std::vector<float>& densities, const std::vector<Float2>& velocities)
	{
		Float2 forceDensity = Float2::zero;
		float particlePressure = Pressure(densities[particleIndex], settings.targetDensity, settings.pressureMultiplier);
		for (int i = 0; i < positions.size(); i++)
		{
			if (i == particleIndex)
				continue;

			Float2 offset = positions[particleIndex] - positions[i];
			float r = offset.Length();
			if (r < settings.effectRadius)
			{
				// Pressure force density:
				Float2 dir = (r < 1e-8f) ? math::Random::UniformDirection2() : offset / r;
				float otherParticlePressure = Pressure(densities[i], settings.targetDensity, settings.pressureMultiplier);
				float sharedPressure = 0.5f * (particlePressure + otherParticlePressure);
				forceDensity += -settings.mass * sharedPressure * smoothingKernals::DSpiky(r, dir, settings.effectRadius) / densities[i];

				// Viscosity force density:
				Float2 velocityDiff = velocities[i] - velocities[particleIndex];
				forceDensity += (settings.mass * smoothingKernals::DDViscos(r, settings.effectRadius) / densities[i]) * velocityDiff;
			}
		}
		return forceDensity;
	}
	Float2 SphFluid2dCpuSolver::InternalForceDensity(int particleIndex, const Settings& settings, const std::vector<Float2>& positions, const std::vector<float>& densities, const std::vector<Float2>& velocities, HashGrid2d& hashGrid)
	{
		Float2 forceDensity = Float2::zero;
		float particlePressure = Pressure(densities[particleIndex], settings.targetDensity, settings.pressureMultiplier);

		// Internal interactions (particle-particle):
		Int2 particleCell = hashGrid.Cell(positions[particleIndex], settings.effectRadius);
		for (const Int2& offset : offsets)
		{
			Int2 neighbourCell = particleCell + offset;
			const uint32_t cellKey = hashGrid.GetCellKey(neighbourCell);
			uint32_t otherIndex = hashGrid.GetStartIndex(neighbourCell);

			// Skip empty cells:
			if (otherIndex == uint32_t(-1) || otherIndex >= positions.size())
				continue;

			while (otherIndex < positions.size() && hashGrid.GetCellKey(otherIndex) == cellKey)
			{
				if (otherIndex == particleIndex)
				{
					otherIndex++;
					continue;
				}

				Float2 otherPos = positions[otherIndex];
				Float2 otherVel = velocities[otherIndex];
				Float2 offset = positions[particleIndex] - otherPos;
				float r = offset.Length();
				if (r < settings.effectRadius)
				{
					// Pressure force density:
					Float2 dir = (r < 1e-8f) ? math::Random::UniformDirection2() : offset / r;
					float otherParticlePressure = Pressure(densities[otherIndex], settings.targetDensity, settings.pressureMultiplier);
					float sharedPressure = 0.5f * (particlePressure + otherParticlePressure);
					forceDensity += -settings.mass * sharedPressure * smoothingKernals::DSpiky(r, dir, settings.effectRadius) / densities[otherIndex];

					// Viscosity force density:
					Float2 velocityDiff = otherVel - velocities[particleIndex];
					forceDensity += (settings.mass * smoothingKernals::DDViscos(r, settings.effectRadius) / densities[otherIndex]) * velocityDiff;
				}
				otherIndex++;
			}
		}
		return forceDensity;
	}
	Float2 SphFluid2dCpuSolver::ExternalForceDensity(int particleIndex, const Settings& settings, const Attractor& attractor, const std::vector<Float2>& positions, const std::vector<float>& densities)
	{
		// Gravity force density:
		Float2 forceDensity = densities[particleIndex] * Float2(0.0f, -settings.gravity);

		// External force density:
		Float2 offset = attractor.point - positions[particleIndex];
		float r = offset.Length();
		if (r < attractor.radius && r > 1e-8f)
		{
			r /= attractor.radius;
			forceDensity += attractor.state * attractor.strength * densities[particleIndex] * offset * r * r;
		}

		return forceDensity;
	}
	void SphFluid2dCpuSolver::BoundaryCollisions(Float2& position, Float2& velocity, const Settings& settings)
	{
		float epsilon = 1e-4f;
		Float3 min = settings.fluidBounds.GetMin();
		Float3 max = settings.fluidBounds.GetMax();

		if (position.x < min.x)
		{
			position.x = min.x + epsilon;
			velocity.x *= -settings.collisionDampening;
		}
		if (position.x > max.x)
		{
			position.x = max.x - epsilon;
			velocity.x *= -settings.collisionDampening;
		}
		if (position.y < min.y)
		{
			position.y = min.y + epsilon;
			velocity.y *= -settings.collisionDampening;
		}
		if (position.y > max.y)
		{
			position.y = max.y - epsilon;
			velocity.y *= -settings.collisionDampening;
		}
	}



	// Field computations:
	void SphFluid2dCpuSolver::ComputeDensities(const Settings& settings, std::vector<float>& densities, const std::vector<Float2>& positions)
	{
		if (settings.pHashGrid != nullptr)
		{
			for (int i = 0; i < positions.size(); i++)
				densities[i] = Density(i, settings, positions, *settings.pHashGrid);
		}
		else
		{
			for (int i = 0; i < positions.size(); i++)
				densities[i] = Density(i, settings, positions);
		}
	}
	void SphFluid2dCpuSolver::ComputeNormals(const Settings& settings, std::vector<Float2>& normals, const std::vector<Float2>& positions, const std::vector<float>& densities)
	{
		if (settings.pHashGrid != nullptr)
		{
			for (int i = 0; i < positions.size(); i++)
				normals[i] = Normal(i, settings, positions, densities, *settings.pHashGrid);
		}
		else
		{
			for (int i = 0; i < positions.size(); i++)
				normals[i] = Normal(i, settings, positions, densities);
		}
	}
	void SphFluid2dCpuSolver::ComputeCurvatures(const Settings& settings, std::vector<float>& curvatures, const std::vector<Float2>& positions, const std::vector<float>& densities, const std::vector<Float2>& normals)
	{
		if (settings.pHashGrid != nullptr)
		{
			for (int i = 0; i < positions.size(); i++)
				curvatures[i] = Curvature(i, settings, positions, densities, normals, *settings.pHashGrid);
		}
		else
		{
			for (int i = 0; i < positions.size(); i++)
				curvatures[i] = Curvature(i, settings, positions, densities, normals);
		}
	}
	void SphFluid2dCpuSolver::ComputeForceDensities(const Settings& settings, const Attractor& attractor, std::vector<Float2>& forceDensities, const std::vector<Float2>& positions, const std::vector<float>& densities, const std::vector<Float2>& velocities)
	{
		if (settings.pHashGrid != nullptr)
		{
			for (int i = 0; i < positions.size(); i++)
			{
				forceDensities[i] = InternalForceDensity(i, settings, positions, densities, velocities, *settings.pHashGrid);
				forceDensities[i] += ExternalForceDensity(i, settings, attractor, positions, densities);
			}
		}
		else
		{
			for (int i = 0; i < positions.size(); i++)
			{
				forceDensities[i] = InternalForceDensity(i, settings, positions, densities, velocities);
				forceDensities[i] += ExternalForceDensity(i, settings, attractor, positions, densities);
			}
		}
	}
}