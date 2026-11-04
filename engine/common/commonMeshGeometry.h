#pragma once
#include "commonSubmesh.h"
#include "emberMath.h"
#include <vector>



namespace emberCommon
{
	/// <summary>
	/// Complete geometry of a mesh.
	/// Optional attributes (normals, tangents, colors, uvs) are either empty or contain one entry per position.
	/// Submeshes must be ordered, contiguous and cover all vertices and triangles. Each submesh's triangles may only reference its own vertices.
	/// Empty submeshes means a single submesh covering the entire mesh. Submesh bounds are computed by the mesh.
	/// </summary>
	struct MeshGeometry
	{
	public: // Members:
		std::vector<Float3> positions;
		std::vector<Float3> normals;
		std::vector<Float3> tangents;
		std::vector<Float4> colors;
		std::vector<Float4> uvs;
		std::vector<Uint3> triangles;
		std::vector<Submesh> submeshes;
	};
}