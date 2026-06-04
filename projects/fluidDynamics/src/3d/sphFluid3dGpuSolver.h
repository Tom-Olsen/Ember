#pragma once
#include "emberEngine.h"
using namespace emberEngine;



namespace fluidDynamics
{
	class SphFluid3dGpuSolver
	{
	public: // Structs:
		struct Settings
		{
			bool useHashGridOptimization;
			float effectRadius;
			float mass;
			float viscosity;
			float surfaceTension;
			float collisionDampening;
			float targetDensity;
			float pressureMultiplier;
			float gravity;
			float maxVelocity;
			Bounds fluidBounds;
		};
		struct Data
		{
			BufferTyped<uint32_t> cellKeyBuffer;
			BufferTyped<uint32_t> startIndexBuffer;
			BufferTyped<uint32_t> sortPermutationBuffer;
			BufferTyped<Float3> positionBuffer;
			BufferTyped<Float3> velocityBuffer;
			BufferTyped<float> densityBuffer;
			BufferTyped<Float3> normalBuffer;
			BufferTyped<float> curvatureBuffer;
			BufferTyped<Float3> forceDensityBuffer;
			BufferTyped<Float3> tempBuffer0;
			BufferTyped<Float3> tempBuffer1;
			BufferTyped<Float3> tempBuffer2;
			BufferTyped<Float3> tempBuffer3;
			BufferTyped<Float3> tempBuffer4;
			BufferTyped<Float3> tempBuffer5;
			ComputeShader resetComputeShader;

			Data();
			int ParticleCount();
			void Reallocate(int particleCount, float initialDistributionRadius, ComputeType computeType, uint32_t sessionID = -1);
		};
		struct RungeKutta	// only needed for RungeKutta2 solver.
		{
			BufferTyped<Float3> kp1Buffer;
			BufferTyped<Float3> kv1Buffer;
			BufferTyped<Float3> tempPositionBuffer;
			BufferTyped<Float3> tempVelocityBuffer;
			ComputeShader resetComputeShader;

			RungeKutta();
			int ParticleCount();
			void Reallocate(int particleCount, ComputeType computeType, uint32_t sessionID = -1);
		};
		struct Attractor
		{
			Float3 point;
			float radius;
			float strength;
			int state;	// -1 = repulse, 0 = off, 1 = attract.
		};
		struct ComputeShaders
		{
			ComputeType computeType;
			uint32_t sessionID;
			ComputeShader cellKeysComputeShader;
			ComputeShader startIndicesResetComputeShader;
			ComputeShader startIndicesComputeShader;
			ComputeShader densityComputeShader;
			ComputeShader normalAndCurvatureComputeShader;
			ComputeShader forceDensityComputeShader;
			ComputeShader rungeKutta2Step1ComputeShader;
			ComputeShader rungeKutta2Step2ComputeShader;
			ComputeShader boundaryCollisionsComputeShader;

			ComputeShaders();
			void SetUseHashGridOptimization(bool useHashGridOptimization);
			void SetEffectRadius(float effectRadius);
			void SetHashGridSize(int hashGridSize);
			void SetMass(float mass);
			void SetViscosity(float viscosity);
			void SetSurfaceTension(float surfaceTension);
			void SetCollisionDampening(float collisionDampening);
			void SetTargetDensity(float targetDensity);
			void SetPressureMultiplier(float pressureMultiplier);
			void SetGravity(float gravity);
			void SetMaxVelocity(float maxVelocity);
			void SetFluidBounds(const Bounds& bounds);
			void SetAttractorRadius(float attractorRadius);
			void SetAttractorStrength(float attractorStrength);
			void SetAttractorState(int attractorState);
			void SetAttractorPoint(const Float3& attractorPoint);
		};

	public: // Methods:
		// Ordinary Differential Equation Solvers:
		//static void TimeStepLeapFrog(float dt, int timeStep, const Settings& settings, Data& data, const Attractor& attractor);
		//static void TimeStepVelocityVerlet(float dt, const Settings& settings, Data& data, const Attractor& attractor);
		static void TimeStepRungeKutta2(float dt, const Settings& settings, Data& data, ComputeShaders& computeShaders, RungeKutta& rungeKutta);

		// Field computations:
		static void ComputeCellKeys(ComputeShaders& computeShaders, const BufferView<uint32_t>& cellKeyBufferView, const BufferView<Float3>& positionBufferView);
		static void ComputeStartIndices(ComputeShaders& computeShaders, const BufferView<uint32_t>& startIndexBufferView, const BufferView<uint32_t>& cellKeyBufferView);
		static void ComputeDensities(ComputeShaders& computeShaders, const BufferView<float>& densityBufferView, const BufferView<Float3>& positionBufferView, const BufferView<uint32_t>& startIndexBufferView, const BufferView<uint32_t>& cellKeyBufferView);
		static void ComputeNormalsAndCurvatures(ComputeShaders& computeShaders, const BufferView<Float3>& normalBufferView, const BufferView<float>& curvatureBufferView, const BufferView<float>& densityBufferView, const BufferView<Float3>& positionBufferView, const BufferView<uint32_t>& startIndexBufferView, const BufferView<uint32_t>& cellKeyBufferView);
		static void ComputeForceDensities(ComputeShaders& computeShaders, const BufferView<Float3>& forceDensityBufferView, const BufferView<float>& densityBufferView, const BufferView<Float3>& positionBufferView, const BufferView<Float3>& velocityBufferView, const BufferView<Float3>& normalBufferView, const BufferView<float>& curvatureBufferView, const BufferView<uint32_t>& startIndexBufferView, const BufferView<uint32_t>& cellKeyBufferView);
		static void ComputeRungeKutta2Step1(ComputeShaders& computeShaders, float dt, const BufferView<Float3>& forceDensityBufferView, const BufferView<float>& densityBufferView, const BufferView<Float3>& positionBufferView, const BufferView<Float3>& velocityBufferView, const BufferView<Float3>& kp1BufferView, const BufferView<Float3>& kv1BufferView, const BufferView<Float3>& tempPositionBufferView, const BufferView<Float3>& tempVelocityBufferView);
		static void ComputeRungeKutta2Step2(ComputeShaders& computeShaders, float dt, const BufferView<Float3>& forceDensityBufferView, const BufferView<float>& densityBufferView, const BufferView<Float3>& kp1BufferView, const BufferView<Float3>& kv1BufferView, const BufferView<Float3>& tempVelocityBufferView, const BufferView<Float3>& positionBufferView, const BufferView<Float3>& velocityBufferView);
		static void ComputeBoundaryCollisions(ComputeShaders& computeShaders, const BufferView<Float3>& positionBufferView, const BufferView<Float3>& velocityBufferView);
	};
}