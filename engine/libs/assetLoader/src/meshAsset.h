#pragma once
#include "emberMath.h"
#include <vector>



namespace emberAssetLoader
{
	struct MeshAsset
	{
		std::vector<Float3> positions;
		std::vector<Float3> normals;
		std::vector<Float3> tangents;
		std::vector<Float4> colors;
		std::vector<Float4> uvs;
		std::vector<Uint3> triangles;
	};
}