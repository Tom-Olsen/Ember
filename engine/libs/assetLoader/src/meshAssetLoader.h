#pragma once
#include "commonMeshGeometry.h"
#include <filesystem>



namespace emberAssetLoader
{
	namespace MeshAssetLoader
	{
		// normals/colors/uvs/submeshes are optional.
		// submeshes format is: firstVertex,vertexCount,firstTriangle,triangleCount
		// Example CSV:
		// positions:
		// 0,0,0
		// 1,0,0
		// 0,1,0
		//
		// triangles:
		// 0,1,2
		//
		// normals:
		// 0,0,1
		// 0,0,1
		// 0,0,1
		//
		// colors:
		// 1,0,0,1
		// 0,1,0,1
		// 0,0,1,1
		//
		// uvs:
		// 0,0,0,0
		// 1,0,0,0
		// 0,1,0,0
		//
		// submeshes:
		// 0,3,0,1
		
		// Loads data. Submesh range verification is handeled by the mesh class.
		emberCommon::MeshGeometry LoadCsv(const std::filesystem::path& path);
	}
}