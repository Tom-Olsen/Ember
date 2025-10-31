#pragma once
#include "emberCoreExport.h"
#include "emberMath.h"
#include "mesh.h"
#include <string>



namespace emberEngine
{
	namespace MeshGenerator
	{
		Mesh EMBER_CORE_API Triangle(Float3 a, Float3 b, Float3 c, const std::string& name);

		Mesh EMBER_CORE_API UnitQuad();
		Mesh EMBER_CORE_API UnitQuadTwoSided();
		Mesh EMBER_CORE_API FullScreenRenderQuad();
		Mesh EMBER_CORE_API ClockwiseQuad(Float3 a, Float3 b, Float3 c, Float3 d, const std::string& name);
		Mesh EMBER_CORE_API Grid(int resolutionX, int resolutionY, const std::string& name);

		Mesh EMBER_CORE_API UnitCube();
		Mesh EMBER_CORE_API HalfCube();

		Mesh EMBER_CORE_API CubeSphere(float radius, int subdivisions, const std::string& name);

		Mesh EMBER_CORE_API Disk(float radius, int cornerCount, const std::string& name);

		Mesh EMBER_CORE_API ArcFlatUv(float radius0, float radius1, float degrees, int cornerCount, const std::string& name);
		Mesh EMBER_CORE_API ArcCurvedUv(float radius0, float radius1, float degrees, int cornerCount, const std::string& name);

		Mesh EMBER_CORE_API ConeMantleSmooth(float radius, float height, int cornerCount, const std::string& name);
		Mesh EMBER_CORE_API ConeMantleFlat(float radius, float height, int cornerCount, const std::string& name);
		Mesh EMBER_CORE_API ConeSmooth(float radius, float height, int cornerCount, const std::string& name);
		Mesh EMBER_CORE_API ConeFlat(float radius, float height, int cornerCount, const std::string& name);

		Mesh EMBER_CORE_API ZylinderMantleSmooth(float radius, float height, int cornerCount, const std::string& name);
		Mesh EMBER_CORE_API ZylinderMantleFlat(float radius, float height, int cornerCount, const std::string& name);
		Mesh EMBER_CORE_API ZylinderSmooth(float radius, float height, int cornerCount, const std::string& name);
		Mesh EMBER_CORE_API ZylinderFlat(float radius, float height, int cornerCount, const std::string& name);

		Mesh EMBER_CORE_API ArrowSmooth(Float3 direction, float bodyHeight, float bodyRadius, float headHeight, float headRadius, int cornerCount, const std::string& name);
		Mesh EMBER_CORE_API ArrowFlat(Float3 direction, float bodyHeight, float bodyRadius, float headHeight, float headRadius, int cornerCount, const std::string& name);

		Mesh EMBER_CORE_API ThreeLeg();
		Mesh EMBER_CORE_API FourLeg();

		Mesh EMBER_CORE_API Camera();
		Mesh EMBER_CORE_API Frame(float radius, const Float3& lengths);
	};
}