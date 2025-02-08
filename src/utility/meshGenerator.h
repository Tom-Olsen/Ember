#ifndef __INCLUDE_GUARD_meshGenerator_h__
#define __INCLUDE_GUARD_meshGenerator_h__
#include "mathf.h"
#include <string>



namespace emberEngine
{
	// Forward declarations:
	class Mesh;



	namespace MeshGenerator
	{
		Mesh* Triangle(Float3 a, Float3 b, Float3 c, const std::string& name);

		Mesh* UnitQuad();
		Mesh* ClockwiseQuad(Float3 a, Float3 b, Float3 c, Float3 d, const std::string& name);
		Mesh* Grid(int resolutionX, int resolutionY, const std::string& name);

		Mesh* UnitCube();
		Mesh* HalfCube();

		Mesh* CubeSphere(float radius, int subdivisions, const std::string& name);

		Mesh* Disk(float radius, int cornerCount, const std::string& name);

		Mesh* ArcFlatUv(float radius0, float radius1, float degrees, int cornerCount, const std::string& name);
		Mesh* ArcCurvedUv(float radius0, float radius1, float degrees, int cornerCount, const std::string& name);

		Mesh* ConeMantleSmooth(float radius, float height, int cornerCount, const std::string& name);
		Mesh* ConeMantleEdgy(float radius, float height, int cornerCount, const std::string& name);
		Mesh* ConeSmooth(float radius, float height, int cornerCount, const std::string& name);
		Mesh* ConeEdgy(float radius, float height, int cornerCount, const std::string& name);


		Mesh* ZylinderMantleSmooth(float radius, float height, int cornerCount, const std::string& name);
		Mesh* ZylinderMantleEdgy(float radius, float height, int cornerCount, const std::string& name);
		Mesh* ZylinderSmooth(float radius, float height, int cornerCount, const std::string& name);
		Mesh* ZylinderEdgy(float radius, float height, int cornerCount, const std::string& name);

		Mesh* ArrowSmooth(Float3 direction, float bodyHeight, float bodyRadius, float headHeight, float headRadius, int cornerCount, const std::string& name);
		Mesh* ArrowEdgy(Float3 direction, float bodyHeight, float bodyRadius, float headHeight, float headRadius, int cornerCount, const std::string& name);

		Mesh* ThreeLeg();
		Mesh* FourLeg();

		Mesh* Camera();
		Mesh* Frame(float radius, const Float3& lengths);
	};
}



#endif // __INCLUDE_GUARD_meshGenerator_h__