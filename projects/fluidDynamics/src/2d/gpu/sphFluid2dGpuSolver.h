#pragma once
#include "emberEngine.h"
using namespace emberEngine;



namespace fluidDynamics
{
	class SphFluid2dGpuSolver
	{
	public: // Structs:
		struct ComputeShaders
		{
			ComputeType computeType;
			uint32_t sessionID;
			ComputeShader resetDataComputeShader;
			ComputeShader resetRungeKuttaComputeShader;
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
			void SetNearPressureRatio(float nearPressureRatio);
			void SetGravity(float gravity);
			void SetMaxVelocity(float maxVelocity);
			void SetFluidBounds(const Bounds& bounds);
			void SetAttractorRadius(float attractorRadius);
			void SetAttractorStrength(float attractorStrength);
			void SetAttractorState(int attractorState);
			void SetAttractorPoint(const Float2& attractorPoint);
		};
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
			float nearPressureRatio;
			float gravity;
			float maxVelocity;
			Bounds fluidBounds;
		};
		struct ScratchData
		{
			BufferTyped<uint32_t> cellKeyBuffer;
			BufferTyped<uint32_t> startIndexBuffer;
			BufferTyped<uint32_t> sortPermutationBuffer;
			BufferTyped<Float2> forceDensityBuffer;
			BufferTyped<float> nearDensityBuffer;
			BufferTyped<Float2> tempBuffer0;
			BufferTyped<Float2> tempBuffer1;
			BufferTyped<Float2> tempBuffer2;
			BufferTyped<Float2> tempBuffer3;
			BufferTyped<Float2> tempBuffer4;
			BufferTyped<Float2> tempBuffer5;
			BufferTyped<Float2> tempBuffer6;
			BufferTyped<Float2> tempBuffer7;
			BufferTyped<Float2> kp1Buffer;
			BufferTyped<Float2> kv1Buffer;
			BufferTyped<Float2> tempPositionBuffer;
			BufferTyped<Float2> tempVelocityBuffer;

			int ParticleCount();
			void Reallocate(int particleCount);
		};
		struct TripleData
		{
			TripleBuffer<Float2> positionBuffer;
			TripleBuffer<Float2> velocityBuffer;
			TripleBuffer<float> densityBuffer;
			TripleBuffer<Float2> normalBuffer;
			TripleBuffer<float> curvatureBuffer;

			int ParticleCount();
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
		//static void TimeStepLeapFrog(float dt, int timeStep, const Settings& settings, Data& data, const Attractor& attractor);
		//static void TimeStepVelocityVerlet(float dt, const Settings& settings, Data& data, const Attractor& attractor);
		static void TimeStepRungeKutta2(float dt, const Settings& settings, const Attractor& attractor, ComputeShaders& computeShaders, ScratchData& scratchData, TripleData& tripleData, uint32_t sourceDataIndex, uint32_t destinationDataIndex);

		// Data management:
		static void ResetData(ComputeShaders& computeShaders, ScratchData& scratchData, TripleData& tripleData, uint32_t dataIndex, float initialDistributionRadius);

		// Field computations:
		static void ComputeCellKeys(ComputeShaders& computeShaders, const BufferView<uint32_t>& cellKeyBufferView, const BufferView<Float2>& positionBufferView);
		static void ComputeStartIndices(ComputeShaders& computeShaders, const BufferView<uint32_t>& startIndexBufferView, const BufferView<uint32_t>& cellKeyBufferView);
		static void ComputeDensities(ComputeShaders& computeShaders, const BufferView<float>& densityBufferView, const BufferView<float>& nearDensityBufferView, const BufferView<Float2>& positionBufferView, const BufferView<uint32_t>& startIndexBufferView, const BufferView<uint32_t>& cellKeyBufferView);
		static void ComputeNormalsAndCurvatures(ComputeShaders& computeShaders, const BufferView<Float2>& normalBufferView, const BufferView<float>& curvatureBufferView, const BufferView<float>& densityBufferView, const BufferView<Float2>& positionBufferView, const BufferView<uint32_t>& startIndexBufferView, const BufferView<uint32_t>& cellKeyBufferView);
		static void ComputeForceDensities(ComputeShaders& computeShaders, const BufferView<Float2>& forceDensityBufferView, const BufferView<float>& densityBufferView, const BufferView<float>& nearDensityBufferView, const BufferView<Float2>& positionBufferView, const BufferView<Float2>& velocityBufferView, const BufferView<Float2>& normalBufferView, const BufferView<float>& curvatureBufferView, const BufferView<uint32_t>& startIndexBufferView, const BufferView<uint32_t>& cellKeyBufferView);
		static void ComputeRungeKutta2Step1(ComputeShaders& computeShaders, float dt, const BufferView<Float2>& forceDensityBufferView, const BufferView<float>& densityBufferView, const BufferView<Float2>& positionBufferView, const BufferView<Float2>& velocityBufferView, const BufferView<Float2>& kp1BufferView, const BufferView<Float2>& kv1BufferView, const BufferView<Float2>& tempPositionBufferView, const BufferView<Float2>& tempVelocityBufferView);
		static void ComputeRungeKutta2Step2(ComputeShaders& computeShaders, float dt, const BufferView<Float2>& forceDensityBufferView, const BufferView<float>& densityBufferView, const BufferView<Float2>& kp1BufferView, const BufferView<Float2>& kv1BufferView, const BufferView<Float2>& tempVelocityBufferView, const BufferView<Float2>& sourcePositionBufferView, const BufferView<Float2>& sourceVelocityBufferView, const BufferView<Float2>& positionBufferView, const BufferView<Float2>& velocityBufferView);
		static void ComputeBoundaryCollisions(ComputeShaders& computeShaders, const BufferView<Float2>& positionBufferView, const BufferView<Float2>& velocityBufferView);
	};
}