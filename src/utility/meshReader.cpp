#include "meshReader.h"
#include "mesh.h"
#include "logger.h"
#include <fstream>
#include <sstream>



namespace emberEngine
{
	// Static members:
	bool MeshReader::s_isInitialized = false;
	std::filesystem::path MeshReader::s_rootPath;



	// Static public methods:
	void TrimLeadingWhitespace(std::string& str)
	{
		str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](unsigned char ch)
		{ return !std::isspace(ch); }));
	}
	Mesh* MeshReader::ReadCsvFile(const std::filesystem::path& relativePath)
	{
		if (s_isInitialized == false)
			Init();

		std::string fileName = relativePath.stem().string();
		std::filesystem::path fullPath = s_rootPath / relativePath;
		fullPath = fullPath.make_preferred(); // normalize all "/" and "\" to preferred symbol.

		// Error handling:
		if (!std::filesystem::exists(fullPath))
		{
			LOG_ERROR("Error: File does not exist: {}", fullPath.string());
			return nullptr;
		}
		if (!std::filesystem::is_regular_file(fullPath))
		{
			LOG_ERROR("Error: Path is not a regular file: {}", fullPath.string());
			return nullptr;
		}

		// Read mesh data:
		std::vector<Float3> positions;
		std::vector<Float3> normals;
		std::vector<Uint3> triangles;
		bool readingPositions = false;
		bool readingNormals = false;
		bool readingTriangles = false;
		std::ifstream file(fullPath);
		std::string line;
		while (std::getline(file, line))
		{
			TrimLeadingWhitespace(line);
			if (line == "" || line[0] == '#')
				continue;

			// Check if data type block has changed:
			if (line.find("positions") != std::string::npos)
			{
				readingPositions = true;
				readingNormals = false;
				readingTriangles = false;
				continue;
			}
			else if (line.find("normals") != std::string::npos)
			{
				readingPositions = false;
				readingNormals = true;
				readingTriangles = false;
				continue;
			}
			else if (line.find("triangles") != std::string::npos)
			{
				readingPositions = false;
				readingNormals = false;
				readingTriangles = true;
				continue;
			}

			// Read positions:
			if (readingPositions)
			{
				std::istringstream iss(line);
				Float3 position;
				char delimiter;

				if (!(iss >> position.x >> delimiter >> position.y >> delimiter >> position.z))
				{
					LOG_ERROR("Invalid position triplet: {}", line);
					return nullptr;
				}
				positions.push_back(position);
			}

			// Read normals:
			if (readingNormals)
			{
				std::istringstream iss(line);
				Float3 normal;
				char delimiter;

				if (!(iss >> normal.x >> delimiter >> normal.y >> delimiter >> normal.z))
				{
					LOG_ERROR("Invalid normal triplet: {}", line);
					return nullptr;
				}
				normals.push_back(normal);
			}

			// Read triangles:
			if (readingTriangles)
			{
				std::istringstream iss(line);
				Uint3 triangle;
				char delimiter;

				if (!(iss >> triangle.x >> delimiter >> triangle.y >> delimiter >> triangle.z))
				{
					LOG_ERROR("Invalid triangle triplet: {}", line);
					return nullptr;
				}
				triangles.push_back(triangle);
			}
		}
		file.close();

		// Construct mesh and return it:
		Mesh* pMesh = new Mesh();
		pMesh->MovePositions(positions);
		pMesh->MoveNormals(normals);
		pMesh->MoveTriangles(triangles);
		pMesh->SetName(fileName);
		return pMesh;
	}



	// Static private methods
	void MeshReader::Init()
	{
		s_isInitialized = true;
		s_rootPath = std::filesystem::current_path().parent_path();
	}
}