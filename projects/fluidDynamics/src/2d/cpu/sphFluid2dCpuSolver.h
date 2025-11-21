#pragma once
#include "emberEngine.h"
#include "hashGrid2d.h"
using namespace emberEngine;

// TODO:
// Curvature calculation might be wrong and needs checking.

namespace fluidDynamics
{
	class SphFluid2dCpuSolver
	{
	public: // Structs:
		struct Settings
		{
			HashGrid2d* pHashGrid = nullptr;	// optional, but recommended.
			float effectRadius = 0.5f;
			float mass = 1.0f;
			float viscosity = 0.5f;
			float surfaceTension = 0.07f;
			float collisionDampening = 0.95f;
			float targetDensity = 15.0f;
			float pressureMultiplier = 20.0f;
			float gravity = 0.5f;
			float maxVelocity = 5.0f;
			Bounds fluidBounds = Bounds(Float3::zero, Float3(16.0f, 9.0f, 0.01f));
		};
		struct Data
		{
			std::vector<Float2> positions;
			std::vector<Float2> velocities;
			std::vector<float> densities;
			std::vector<Float2> normals;
			std::vector<float> curvatures;
			std::vector<Float2> forceDensities;

			int ParticleCount();
			void Reallocate(int particleCount);
		};
		struct RungeKutta	// only needed for RungeKutta2 solver.
		{
			std::vector<Float2> kp1;
			std::vector<Float2> kv1;
			std::vector<Float2> tempPositions;
			std::vector<Float2> tempVelocities;

			void Reallocate(int particleCount);
		};
		struct Attractor
		{
			Float2 point;
			float radius;
			float strength;
			int state;	// -1 = repulse, 0 = off, 1 = attract.
		};

	public: // Methods:
		// Ordinary Differential Equation Solvers:
		static void TimeStepLeapFrog(float dt, int timeStep, const Settings& settings, Data& data, const Attractor& attractor);
		static void TimeStepVelocityVerlet(float dt, const Settings& settings, Data& data, const Attractor& attractor);
		static void TimeStepRungeKutta2(float dt, const Settings& settings, Data& data, const Attractor& attractor, RungeKutta& rungeKutta);

		// Particle computations:
		static float Density(int particleIndex, const Settings& settings, const std::vector<Float2>& positions);
		static float Density(int particleIndex, const Settings& settings, const std::vector<Float2>& positions, HashGrid2d& hashGrid);
		static Float2 Normal(int particleIndex, const Settings& settings, const std::vector<Float2>& positions, const std::vector<float>& densities);
		static Float2 Normal(int particleIndex, const Settings& settings, const std::vector<Float2>& positions, const std::vector<float>& densities, HashGrid2d& hashGrid);
		static float Curvature(int particleIndex, const Settings& settings, const std::vector<Float2>& positions, const std::vector<float>& densities, const std::vector<Float2>& normals);
		static float Curvature(int particleIndex, const Settings& settings, const std::vector<Float2>& positions, const std::vector<float>& densities, const std::vector<Float2>& normals, HashGrid2d& hashGrid);
		static Float2 InternalForceDensity(int particleIndex, const Settings& settings, const std::vector<Float2>& positions, const std::vector<float>& densities, const std::vector<Float2>& velocities);
		static Float2 InternalForceDensity(int particleIndex, const Settings& settings, const std::vector<Float2>& positions, const std::vector<float>& densities, const std::vector<Float2>& velocities, HashGrid2d& hashGrid);
		static Float2 ExternalForceDensity(int particleIndex, const Settings& settings, const Attractor& attractor, const std::vector<Float2>& positions, const std::vector<float>& densities);
		static void BoundaryCollisions(Float2& position, Float2& velocity, const Settings& settings);

		// Field computations:
		static void ComputeDensities(const Settings& settings, std::vector<float>& densities, const std::vector<Float2>& positions);
		static void ComputeNormals(const Settings& settings, std::vector<Float2>& normals, const std::vector<Float2>& positions, const std::vector<float>& densities);
		static void ComputeCurvatures(const Settings& settings, std::vector<float>& curvatures, const std::vector<Float2>& positions, const std::vector<float>& densities, const std::vector<Float2>& normals);
		static void ComputeForceDensities(const Settings& settings, const Attractor& attractor, std::vector<Float2>& forceDensities, const std::vector<Float2>& positions, const std::vector<float>& densities, const std::vector<Float2>& velocities);
	};
}