#include "meshGenerator.h"
#include "mesh.h"



namespace MeshGenerator
{
	Mesh* Triangle(Float3 a, Float3 b, Float3 c, const std::string& name)
	{
		Mesh* pMesh = new Mesh(name);

		std::vector<Float3> positions;
		positions.emplace_back(a);
		positions.emplace_back(b);
		positions.emplace_back(c);

		Float3 normal = Float3::Cross(b - a, c - a).Normalize();
		std::vector<Float3> normals;
		normals.emplace_back(normal);
		normals.emplace_back(normal);
		normals.emplace_back(normal);

		std::vector<Float4> uvs;
		uvs.emplace_back(0.0f, 0.0f, 0.0f, 0.0f);
		uvs.emplace_back(1.0f, 0.0f, 0.0f, 0.0f);
		uvs.emplace_back(0.0f, 1.0f, 0.0f, 0.0f);

		std::vector<Uint3> triangles;
		triangles.emplace_back(Uint3(0, 1, 2));

		pMesh->MovePositions(positions);
		pMesh->MoveNormals(normals);
		pMesh->MoveUVs(uvs);
		pMesh->MoveTriangles(triangles);
		pMesh->ComputeTangents();
		return pMesh;
	}

	Mesh* UnitQuad()
	{
		Mesh* pMesh = new Mesh("unitQuad");

		std::vector<Float3> positions;
		positions.emplace_back(-0.5f, -0.5f, 0.0f);
		positions.emplace_back(-0.5f, 0.5f, 0.0f);
		positions.emplace_back(0.5f, -0.5f, 0.0f);
		positions.emplace_back(0.5f, 0.5f, 0.0f);

		std::vector<Float3> normals;
		normals.emplace_back(0.0f, 0.0f, 1.0f);
		normals.emplace_back(0.0f, 0.0f, 1.0f);
		normals.emplace_back(0.0f, 0.0f, 1.0f);
		normals.emplace_back(0.0f, 0.0f, 1.0f);

		std::vector<Float4> uvs;
		uvs.emplace_back(0.0f, 0.0f, 0.0f, 0.0f);
		uvs.emplace_back(0.0f, 1.0f, 0.0f, 0.0f);
		uvs.emplace_back(1.0f, 0.0f, 0.0f, 0.0f);
		uvs.emplace_back(1.0f, 1.0f, 0.0f, 0.0f);

		std::vector<Uint3> triangles;
		triangles.emplace_back(Uint3(0, 2, 1));
		triangles.emplace_back(Uint3(1, 2, 3));

		pMesh->MovePositions(positions);
		pMesh->MoveNormals(normals);
		pMesh->MoveUVs(uvs);
		pMesh->MoveTriangles(triangles);
		pMesh->ComputeTangents();
		return pMesh;
	}
	Mesh* ClockwiseQuad(Float3 a, Float3 b, Float3 c, Float3 d, const std::string& name)
	{
		Mesh* pMesh = new Mesh(name);

		std::vector<Float3> positions;
		positions.emplace_back(a);
		positions.emplace_back(b);
		positions.emplace_back(c);
		positions.emplace_back(d);

		Float3 normal = Float3::Cross(c - a, b - a).Normalize();
		std::vector<Float3> normals;
		normals.emplace_back(normal);
		normals.emplace_back(normal);
		normals.emplace_back(normal);
		normals.emplace_back(normal);

		std::vector<Float4> uvs;
		uvs.emplace_back(0.0f, 0.0f, 0.0f, 0.0f);
		uvs.emplace_back(1.0f, 0.0f, 0.0f, 0.0f);
		uvs.emplace_back(1.0f, 1.0f, 0.0f, 0.0f);
		uvs.emplace_back(0.0f, 1.0f, 0.0f, 0.0f);

		std::vector<Uint3> triangles;
		triangles.emplace_back(Uint3(0, 3, 1));
		triangles.emplace_back(Uint3(1, 3, 2));

		pMesh->MovePositions(positions);
		pMesh->MoveNormals(normals);
		pMesh->MoveUVs(uvs);
		pMesh->MoveTriangles(triangles);
		pMesh->ComputeTangents();
		return pMesh;
	}
	Mesh* Grid(int resolutionX, int resolutionY, const std::string& name)
	{
		Mesh* pMesh = new Mesh(name);

		std::vector<Float3> positions;
		std::vector<Float3> normals;
		std::vector<Float4> uvs;
		std::vector<Uint3> triangles;

		for (int j = 0; j < resolutionY; j++)
		{
			float v = j / (resolutionY - 1.0f);
			float y = v - 0.5f;
			for (int i = 0; i < resolutionX; i++)
			{
				float u = i / (resolutionX - 1.0f);
				float x = u - 0.5f;
				positions.emplace_back(x, y, 0.0f);
				normals.emplace_back(0.0f, 0.0f, 1.0f);
				uvs.emplace_back(u, v, 0.0f, 0.0f);
			}
		}

		for (int i = 0; i < resolutionX - 1; i++)
		{
			for (int j = 0; j < resolutionY - 1; j++)
			{
				int t0 = i + j * resolutionX;
				int t1 = i + (j + 1) * resolutionX;
				int t2 = i + 1 + j * resolutionX;
				int t3 = i + 1 + (j + 1) * resolutionX;
				triangles.emplace_back(Uint3(t0, t2, t1));
				triangles.emplace_back(Uint3(t2, t3, t1));
			}
		}

		pMesh->MovePositions(positions);
		pMesh->MoveNormals(normals);
		pMesh->MoveUVs(uvs);
		pMesh->MoveTriangles(triangles);
		pMesh->ComputeTangents();
		return pMesh;
	}

	Mesh* UnitCube()
	{
		Float3 p000 = 0.5f * Float3(-1.0f, -1.0f, -1.0f);
		Float3 p001 = 0.5f * Float3(-1.0f, -1.0f,  1.0f);
		Float3 p010 = 0.5f * Float3(-1.0f,  1.0f, -1.0f);
		Float3 p011 = 0.5f * Float3(-1.0f,  1.0f,  1.0f);
		Float3 p100 = 0.5f * Float3( 1.0f, -1.0f, -1.0f);
		Float3 p101 = 0.5f * Float3( 1.0f, -1.0f,  1.0f);
		Float3 p110 = 0.5f * Float3( 1.0f,  1.0f, -1.0f);
		Float3 p111 = 0.5f * Float3( 1.0f,  1.0f,  1.0f);

		std::vector<Mesh*> faces;
		faces.reserve(6);
		faces.push_back(ClockwiseQuad(p100, p101, p111, p110, "+x"));
		faces.push_back(ClockwiseQuad(p001, p000, p010, p011, "-x"));
		faces.push_back(ClockwiseQuad(p111, p011, p010, p110, "+y"));
		faces.push_back(ClockwiseQuad(p001, p101, p100, p000, "-y"));
		faces.push_back(ClockwiseQuad(p101, p001, p011, p111, "+z"));
		faces.push_back(ClockwiseQuad(p000, p100, p110, p010, "-z"));
		//faces.push_back(ClockwiseQuad(p100 + 0.1f * Float3::right, p101 + 0.1f * Float3::right, p111 + 0.1f * Float3::right, p110 + 0.1f * Float3::right, "+x"));
		//faces.push_back(ClockwiseQuad(p001 + 0.1f * Float3::left, p000 + 0.1f * Float3::left, p010 + 0.1f * Float3::left, p011 + 0.1f * Float3::left, "-x"));
		//faces.push_back(ClockwiseQuad(p111 + 0.1f * Float3::up, p011 + 0.1f * Float3::up, p010 + 0.1f * Float3::up, p110 + 0.1f * Float3::up, "+y"));
		//faces.push_back(ClockwiseQuad(p001 + 0.1f * Float3::down, p101 + 0.1f * Float3::down, p100 + 0.1f * Float3::down, p000 + 0.1f * Float3::down, "-y"));
		//faces.push_back(ClockwiseQuad(p101 + 0.1f * Float3::forward, p001 + 0.1f * Float3::forward, p011 + 0.1f * Float3::forward, p111 + 0.1f * Float3::forward, "+z"));
		//faces.push_back(ClockwiseQuad(p000 + 0.1f * Float3::backward, p100 + 0.1f * Float3::backward, p110 + 0.1f * Float3::backward, p010 + 0.1f * Float3::backward, "-z"));

		return Mesh::Merge(faces, "unitCube");
	}
	Mesh* HalfCube()
	{
		Float3 p000 = 0.5f * Float3(-1,-1, 0);
		Float3 p001 = 0.5f * Float3(-1,-1, 1);
		Float3 p010 = 0.5f * Float3(-1, 1, 0);
		Float3 p011 = 0.5f * Float3(-1, 1, 1);
		Float3 p100 = 0.5f * Float3( 1,-1, 0);
		Float3 p101 = 0.5f * Float3( 1,-1, 1);
		Float3 p110 = 0.5f * Float3( 1, 1, 0);
		Float3 p111 = 0.5f * Float3( 1, 1, 1);

		std::vector<Mesh*> faces;
		faces.reserve(5);
		faces.push_back(ClockwiseQuad(p110, p100, p101, p111, "+x"));
		faces.push_back(ClockwiseQuad(p011, p001, p000, p010, "-x"));
		faces.push_back(ClockwiseQuad(p111, p011, p010, p110, "+y"));
		faces.push_back(ClockwiseQuad(p100, p000, p001, p101, "-y"));
		faces.push_back(ClockwiseQuad(p101, p001, p011, p111, "+z"));

		return Mesh::Merge(faces, "halfCube");
	}

	Mesh* CubeSphere(float radius, int subdivisions, const std::string& name)
	{
		radius = std::max(1e-8f, radius);
		Mesh* pMesh = UnitCube();
		for (int i = 0; i < subdivisions; i++)
			pMesh->Subdivide();

		pMesh->SetName(name);
		pMesh->Spherify(1.0f, radius);
		return pMesh;
	}

	Mesh* Disk(float radius, int cornerCount, const std::string& name)
	{
		cornerCount = std::max(3, cornerCount);
		std::vector<Float3> positions;	positions.reserve(cornerCount + 1);
		std::vector<Float3> normals;	normals.reserve(cornerCount + 1);
		std::vector<Float4> uvs;		uvs.reserve(cornerCount + 1);
		std::vector<Uint3> triangles;	triangles.reserve(cornerCount);

		// Per vertex data:
		float dAlpha = 2.0f * mathf::PI  / cornerCount;
		for (int i = 0; i < cornerCount; i++)
		{
			float alpha = i * dAlpha;
			float x = mathf::Cos(alpha);
			float y = mathf::Sin(alpha);
			positions.push_back(Float3(radius * x, radius * y, 0.0f));
			normals.push_back(Float3(0.0f, 0.0f, 1.0f));
			uvs.push_back(Float4(0.5f * (x + 1.0f), 0.5f * (y + 1.0f), 0.0f, 0.0f));
		}
		positions.push_back(Float3(0.0f));
		normals.push_back(Float3(0.0f, 0.0f, 1.0f));
		uvs.push_back(Float4(0.5f, 0.5f, 0.0f, 0.0f));

		// Triangles:
		for (int i = 0; i < cornerCount - 1; i++)
			triangles.push_back(Uint3(i, i + 1, cornerCount));
		triangles.push_back(Uint3(0, cornerCount, cornerCount - 1));

		Mesh* pMesh = new Mesh(name);
		pMesh->MovePositions(positions);
		pMesh->MoveNormals(normals);
		pMesh->MoveUVs(uvs);
		pMesh->MoveTriangles(triangles);
		pMesh->ComputeTangents();
		return pMesh;
	}
	
	Mesh* ArcFlatUv(float radius0, float radius1, float degrees, int cornerCount, const std::string& name)
	{
		// Validate input values:
		degrees = mathf::Clamp(degrees, 0.0f, 360.0f);
		cornerCount = std::max(2, cornerCount);
		radius0 = std::max(1e-8f, radius0);
		if (radius0 > radius1)
			std::swap(radius0, radius1);

		std::vector<Float3> positions;	positions.reserve(2 * cornerCount);
		std::vector<Float3> normals;	normals.reserve(2 * cornerCount);
		std::vector<Float4> uvs;		uvs.reserve(2 * cornerCount);
		std::vector<Uint3> triangles;	triangles.reserve(2 * (cornerCount - 1));

		// Per vertex data:
		float dAlpha = mathf::DEG2RAD * degrees / (cornerCount - 1.0f);
		for (int i = 0; i < cornerCount; i++)
		{
			float x = mathf::Cos(i * dAlpha);
			float y = mathf::Sin(i * dAlpha);
			positions.push_back(radius0 * Float3(x, y, 0.0f));
			positions.push_back(radius1 * Float3(x, y, 0.0f));
			normals.push_back(Float3(0.0f, 0.0f, 1.0f));
			normals.push_back(Float3(0.0f, 0.0f, 1.0f));
			float ratio = radius0 / radius1;
			uvs.push_back(Float4(0.5f * (ratio * x + 1.0f), 0.5f * (ratio * y + 1.0f), 0.0f, 0.0f));
			uvs.push_back(Float4(0.5f * (x + 1.0f), 0.5f * (y + 1.0f), 0.0f, 0.0f));
		}

		// Triangles:
		for (int i = 0; i < 2 * (cornerCount - 1); i += 2)
		{
			triangles.push_back(Uint3(i, i + 1, i + 2));
			triangles.push_back(Uint3(i + 1, i + 3, i + 2));
		}

		Mesh* pMesh = new Mesh(name);
		pMesh->MovePositions(positions);
		pMesh->MoveNormals(normals);
		pMesh->MoveUVs(uvs);
		pMesh->MoveTriangles(triangles);
		pMesh->ComputeTangents();
		return pMesh;
	}
	Mesh* ArcCurvedUv(float radius0, float radius1, float degrees, int cornerCount, const std::string& name)
	{
		// Validate input values:
		degrees = mathf::Clamp(degrees, 0.0f, 360.0f);
		cornerCount = std::max(2, cornerCount);
		radius0 = std::max(1e-8f, radius0);
		if (radius0 > radius1)
			std::swap(radius0, radius1);

		std::vector<Float3> positions;	positions.reserve(2 * cornerCount);
		std::vector<Float3> normals;	normals.reserve(2 * cornerCount);
		std::vector<Float4> uvs;		uvs.reserve(2 * cornerCount);
		std::vector<Uint3> triangles;	triangles.reserve(2 * (cornerCount - 1));

		// Per vertex data:
		float duv = 1.0f / (cornerCount - 1.0f);
		float dAlpha = mathf::DEG2RAD * degrees / (cornerCount - 1.0f);
		for (int i = 0; i < cornerCount; i++)
		{
			float x = mathf::Cos(i * dAlpha);
			float y = mathf::Sin(i * dAlpha);
			positions.push_back(radius0 * Float3(x, y, 0.0f));
			positions.push_back(radius1 * Float3(x, y, 0.0f));
			normals.push_back(Float3(0.0f, 0.0f, 1.0f));
			normals.push_back(Float3(0.0f, 0.0f, 1.0f));
			float ratio = radius0 / radius1;
			uvs.push_back(Float4(0.0f, i * duv, 0.0f, 0.0f));
			uvs.push_back(Float4(1.0f, i * duv, 0.0f, 0.0f));
		}

		// Triangles:
		for (int i = 0; i < 2 * (cornerCount - 1); i += 2)
		{
			triangles.push_back(Uint3(i, i + 1, i + 2));
			triangles.push_back(Uint3(i + 1, i + 3, i + 2));
		}

		Mesh* pMesh = new Mesh(name);
		pMesh->MovePositions(positions);
		pMesh->MoveNormals(normals);
		pMesh->MoveUVs(uvs);
		pMesh->MoveTriangles(triangles);
		pMesh->ComputeTangents();
		return pMesh;
	}

	Mesh* ConeSmooth(float radius, float height, int cornerCount, const std::string& name)
	{
		radius = std::max(1e-8f, radius);
		height = std::max(1e-8f, height);
		cornerCount = std::max(3, cornerCount);

		std::vector<Float3> positions;	positions.reserve(cornerCount + 2);
		std::vector<Float3> normals;	normals.reserve(cornerCount + 2);
		std::vector<Float4> uvs;		uvs.reserve(cornerCount + 2);
		std::vector<Uint3> triangles;	triangles.reserve(3 * cornerCount);

		float dAlpha = 2.0f * mathf::PI / cornerCount;
		for (int i = 0; i < cornerCount + 1; i++)
		{
			float alpha = i * dAlpha;
			float x = mathf::Cos(alpha);
			float y = mathf::Sin(alpha);
			positions.push_back(Float3(radius * x, radius * y, 0.0f));

			float normalX = 1.0f / static_cast<float>(sqrt(pow(height / radius, 2) + 1.0f));
			float normalY = normalX * height / radius;
			normals.push_back(Float3(normalY * x, normalY * y, normalX));

			uvs.push_back(Float4(0.5f * (x + 1.0f), 0.5f * (y + 1.0f), 0.0f, 0.0f));
		}
		positions.push_back(Float3(0.0f, 0.0f, height));
		normals.push_back(Float3(0.0f, 0.0f, 1.0f));
		uvs.push_back(Float4(0.5f, 0.5f, 0.0f, 0.0f));

		for (int i = 0; i < cornerCount; i++)
			triangles.push_back(Uint3(i, i + 1, cornerCount + 1));

		Mesh* pMesh = new Mesh(name);
		pMesh->MovePositions(positions);
		pMesh->MoveNormals(normals);
		pMesh->MoveUVs(uvs);
		pMesh->MoveTriangles(triangles);
		pMesh->ComputeTangents();
		return pMesh;
	}
	Mesh* ConeEdgy(float radius, float height, int cornerCount, const std::string& name)
	{
		std::vector<Mesh*> faces;
		faces.reserve(cornerCount);
		std::vector<Float4> uvs(3);

		float dAlpha = 2.0f * mathf::PI / cornerCount;
		for (int i = 0; i < cornerCount; i++)
		{
			float alpha0 = i * dAlpha;
			float alpha1 = (i + 1) % cornerCount * dAlpha;
			float x0 = mathf::Cos(alpha0);
			float x1 = mathf::Cos(alpha1);
			float y0 = mathf::Sin(alpha0);
			float y1 = mathf::Sin(alpha1);

			Float3 a = radius * Float3(x0, y0, 0.0f);
			Float3 b = radius * Float3(x1, y1, 0.0f);
			Float3 c = Float3(0.0f, 0.0f, height);
			Mesh* face = Triangle(a, b, c, "");
			uvs[0] = Float4(0.5f * (x0 + 1.0f), 0.5f * (y0 + 1.0f), 0.0f, 0.0f);
			uvs[1] = Float4(0.5f * (x1 + 1.0f), 0.5f * (y1 + 1.0f), 0.0f, 0.0f);
			uvs[2] = Float4(0.5f, 0.5f, 0.0f, 0.0f);
			face->SetUVs(uvs);

			faces.push_back(face);
		}

		return Mesh::Merge(faces, name);
	}

	Mesh* ZylinderMantleSmooth(float radius, float height, int cornerCount, const std::string& name)
	{
		radius = std::max(1e-8f, radius);
		height = std::max(1e-8f, height);
		cornerCount = std::max(3, cornerCount);

		std::vector<Float3> positions;	positions.reserve(2 * (cornerCount + 1));
		std::vector<Float3> normals;	normals.reserve(2 * (cornerCount + 1));
		std::vector<Float4> uvs;		uvs.reserve(2 * (cornerCount + 1));
		std::vector<Uint3> triangles;	triangles.reserve(2 * cornerCount);

		float dAlpha = 2.0f * mathf::PI / cornerCount;
		for (int i = 0; i < cornerCount + 1; i++)
		{
			float alpha = i * dAlpha;
			float x = mathf::Cos(alpha);
			float y = mathf::Sin(alpha);
			positions.push_back(Float3(radius * x, radius * y, -0.5f * height));
			positions.push_back(Float3(radius * x, radius * y,  0.5f * height));

			normals.push_back(Float3(x, y, 0.0f));
			normals.push_back(Float3(x, y, 0.0f));

			float u = i / (float)cornerCount;
			uvs.push_back(Float4(u, 0.0f, 0.0f, 0.0f));
			uvs.push_back(Float4(u, 1.0f, 0.0f, 0.0f));
		}

		for (int i = 0; i < 2 * cornerCount; i += 2)
		{
			triangles.push_back(Uint3(i, i + 2, i + 1));
			triangles.push_back(Uint3(i + 1, i + 2, i + 3));
		}

		Mesh* pMesh = new Mesh(name);
		pMesh->MovePositions(positions);
		pMesh->MoveNormals(normals);
		pMesh->MoveUVs(uvs);
		pMesh->MoveTriangles(triangles);
		pMesh->ComputeTangents();
		return pMesh;
	}
	Mesh* ZylinderMantleEdgy(float radius, float height, int cornerCount, const std::string& name)
	{
		radius = std::max(1e-8f, radius);
		height = std::max(1e-8f, height);
		cornerCount = std::max(3, cornerCount);
		
		std::vector<Mesh*> faces;
		faces.reserve(2 * cornerCount);
		std::vector<Float4> uvs(4);
		
		float dAlpha = 2.0f * mathf::PI / cornerCount;
		float dist = radius * mathf::Cos(0.5f * dAlpha);
		float width = 2.0f * radius * mathf::Sin(0.5f * dAlpha);
		for (int i = 0; i < cornerCount; i++)
		{
			float alpha = (i + 0.5f) * dAlpha;

			Mesh* face = UnitQuad();
			face->Scale(Float3(width, height, 1.0f));
			Float3x3 rotation = Float3x3::RotateZ(mathf::PI_2 + alpha) * Float3x3::RotateX(mathf::PI_2);
			face->Rotate(rotation);
			face->Translate(Float3(dist * mathf::Cos(alpha), dist * mathf::Sin(alpha), 0.0f));

			uvs[0] = Float4((i + 0.0f) / cornerCount, 0.0f, 0.0f, 0.0f);
			uvs[1] = Float4((i + 0.0f) / cornerCount, 1.0f, 0.0f, 0.0f);
			uvs[2] = Float4((i + 1.0f) / cornerCount, 0.0f, 0.0f, 0.0f);
			uvs[3] = Float4((i + 1.0f) / cornerCount, 1.0f, 0.0f, 0.0f);
			face->SetUVs(uvs);

			faces.push_back(face);
		}
		
		return Mesh::Merge(faces, name);
	}
	Mesh* ZylinderSmooth(float radius, float height, int cornerCount, const std::string& name)
	{
		std::vector<Mesh*> meshes;
		meshes.reserve(3);
		meshes.push_back(ZylinderMantleSmooth(radius, height, cornerCount, "zylinderSmooth0"));
		meshes.push_back(Disk(radius, cornerCount, "zylinderSmooth1")->Translate(0.5f * height * Float3(0.0f, 0.0f, 1.0f)));
		meshes.push_back(meshes[1]->GetCopy("zylinderSmooth2")->Rotate(Float3x3::RotateX(mathf::PI)));
		return Mesh::Merge(meshes, name);
	}
	Mesh* ZylinderEdgy(float radius, float height, int cornerCount, const std::string& name)
	{
		std::vector<Mesh*> meshes;
		meshes.reserve(3);
		meshes.push_back(ZylinderMantleEdgy(radius, height, cornerCount, "zylinderEdgy0"));
		meshes.push_back(Disk(radius, cornerCount, "zylinderEdgy1")->Translate(0.5f * height * Float3(0.0f, 0.0f, 1.0f)));
		meshes.push_back(meshes[1]->GetCopy("zylinderEdgy2")->Rotate(Float3x3::RotateX(mathf::PI)));
		return Mesh::Merge(meshes, name);
	}

	Mesh* ArrowSmooth(Float3 direction, float bodyHeight, float bodyRadius, float headHeight, float headRadius, int cornerCount, const std::string& name)
	{
		if (direction == Float3(0.0f))
			direction = Float3::up;
		else
			direction = direction.Normalize();

		bodyHeight = std::max(1e-8f, bodyHeight);
		bodyRadius = std::max(1e-8f, bodyRadius);
		headHeight = std::max(1e-8f, headHeight);
		headRadius = std::max(headRadius, bodyRadius);
		cornerCount = std::max(3, cornerCount);

		std::vector<Mesh*> meshes;
		meshes.reserve(4);

		meshes.push_back(Disk(bodyRadius, cornerCount, "arrowSmooth0")->Rotate(Float3x3::RotateX(mathf::PI)));
		meshes.push_back(ZylinderMantleSmooth(bodyRadius, bodyHeight, cornerCount, "arrowSmooth1")->Translate(0.5f * bodyHeight * Float3::up));
		meshes.push_back(ArcFlatUv(bodyRadius, headRadius, 360.0f, cornerCount + 1, "arrowSmooth2")->Translate(-bodyHeight * Float3::up)->Rotate(Float3x3::RotateX(mathf::PI)));
		meshes.push_back(ConeSmooth(headRadius, headHeight, cornerCount, "arrowSmooth3")->Translate(bodyHeight * Float3::up));

		Float4x4 rotation = Float4x4::RotateFromTo(Float3::up, direction);
		return Mesh::Merge(meshes, name)->Rotate(rotation);
	}
	Mesh* ArrowEdgy(Float3 direction, float bodyHeight, float bodyRadius, float headHeight, float headRadius, int cornerCount, const std::string& name)
	{
		if (direction == Float3(0.0f))
			direction = Float3::up;
		else
			direction = direction.Normalize();

		bodyHeight = std::max(1e-8f, bodyHeight);
		bodyRadius = std::max(1e-8f, bodyRadius);
		headHeight = std::max(1e-8f, headHeight);
		headRadius = std::max(headRadius, bodyRadius);
		cornerCount = std::max(3, cornerCount);

		std::vector<Mesh*> meshes;
		meshes.reserve(4);

		meshes.push_back(Disk(bodyRadius, cornerCount, "arrowEdgy0")->Rotate(Float3x3::RotateX(mathf::PI)));
		meshes.push_back(ZylinderMantleEdgy(bodyRadius, bodyHeight, cornerCount, "arrowEdgy1")->Translate(0.5f * bodyHeight * Float3::forward));
		meshes.push_back(ArcFlatUv(bodyRadius, headRadius, 360.0f, cornerCount + 1, "arrowEdgy2")->Translate(-bodyHeight * Float3::forward)->Rotate(Float3x3::RotateX(mathf::PI)));
		meshes.push_back(ConeEdgy(headRadius, headHeight, cornerCount, "arrowEdgy3")->Translate(bodyHeight * Float3::forward));

		Float4x4 rotation = Float4x4::RotateFromTo(Float3::forward, direction);
		return Mesh::Merge(meshes, name)->Rotate(rotation);
	}

	Mesh* ThreeLeg()
	{
		std::vector<Mesh*> meshes;
		meshes.reserve(3);

		meshes.push_back(ArrowEdgy(Float3(1.0f, 0.0f, 0.0f), 0.8f, 0.1f, 0.2f, 0.2f, 8, "threeLeg0"));
		meshes.push_back(ArrowEdgy(Float3(0.0f, 1.0f, 0.0f), 0.8f, 0.1f, 0.2f, 0.2f, 8, "threeLeg1"));
		meshes.push_back(ArrowEdgy(Float3(0.0f, 0.0f, 1.0f), 0.8f, 0.1f, 0.2f, 0.2f, 8, "threeLeg2"));
		meshes[0]->SetUniformColor(Float4(1.0f, 0.0f, 0.0f, 1.0f));
		meshes[1]->SetUniformColor(Float4(0.0f, 1.0f, 0.0f, 1.0f));
		meshes[2]->SetUniformColor(Float4(0.0f, 0.0f, 1.0f, 1.0f));

		return Mesh::Merge(meshes, "threeLeg");
	}

	Mesh* FourLeg()
	{
		std::vector<Mesh*> meshes;
		meshes.reserve(4);

		meshes.push_back(ArrowEdgy(Float3(1.0f, 0.0f,  0.0f), 0.8f, 0.1f, 0.2f, 0.2f, 8, "fourLeg0"));
		meshes.push_back(ArrowEdgy(Float3(0.0f, 1.0f,  0.0f), 0.8f, 0.1f, 0.2f, 0.2f, 8, "fourLeg1"));
		meshes.push_back(ArrowEdgy(Float3(0.0f, 0.0f,  1.0f), 0.8f, 0.1f, 0.2f, 0.2f, 8, "fourLeg2"));
		meshes.push_back(ArrowEdgy(Float3(0.0f, 0.0f, -1.0f), 0.8f, 0.1f, 0.2f, 0.2f, 8, "fourLeg3"));
		meshes[0]->SetUniformColor(Float4(1.0f, 0.0f, 0.0f, 1.0f));
		meshes[1]->SetUniformColor(Float4(0.0f, 1.0f, 0.0f, 1.0f));
		meshes[2]->SetUniformColor(Float4(0.0f, 0.0f, 1.0f, 1.0f));
		meshes[3]->SetUniformColor(Float4(1.0f, 1.0f, 1.0f, 1.0f));

		return Mesh::Merge(meshes, "fourLeg");
	}
}