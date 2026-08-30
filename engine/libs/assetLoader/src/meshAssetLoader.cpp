#include "meshAssetLoader.h"
#include "assetLoaderUtility.h"
#include "logger.h"
#include <fstream>
#include <sstream>
#include <string>



namespace emberAssetLoader
{
	namespace MeshAssetLoader
	{
		MeshAsset LoadCsv(const std::filesystem::path& path)
		{
			// Error handling:
			if (!std::filesystem::exists(path))
			{
				LOG_ERROR("Error: File does not exist: {}", path.string());
				return MeshAsset{};
			}
			if (!std::filesystem::is_regular_file(path))
			{
				LOG_ERROR("Error: Path is not a regular file: {}", path.string());
				return MeshAsset{};
			}

			// Read mesh data:
			MeshAsset meshAsset = {};
			bool readingPositions = false;
			bool readingNormals = false;
			bool readingTriangles = false;
			bool readingColors = false;
			bool readingUvs = false;
			std::ifstream file(path);
			std::string line;
			while (std::getline(file, line))
			{
				Utility::TrimLeadingWhitespace(line);
				if (line == "" || line[0] == '#')
					continue;

				// Check if data type block has changed:
				if (line.find("positions") != std::string::npos)
				{
					readingPositions = true;
					readingNormals = false;
					readingTriangles = false;
					readingColors = false;
					readingUvs = false;
					continue;
				}
				else if (line.find("normals") != std::string::npos)
				{
					readingPositions = false;
					readingNormals = true;
					readingTriangles = false;
					readingColors = false;
					readingUvs = false;
					continue;
				}
				else if (line.find("triangles") != std::string::npos)
				{
					readingPositions = false;
					readingNormals = false;
					readingTriangles = true;
					readingColors = false;
					readingUvs = false;
					continue;
				}
				else if (line.find("colors") != std::string::npos)
				{
					readingPositions = false;
					readingNormals = false;
					readingTriangles = false;
					readingColors = true;
					readingUvs = false;
					continue;
				}
				else if (line.find("uvs") != std::string::npos)
				{
					readingPositions = false;
					readingNormals = false;
					readingTriangles = false;
					readingColors = false;
					readingUvs = true;
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
						LOG_ERROR("Invalid position values: {}", line);
						file.close();
						return MeshAsset{};
					}
					meshAsset.positions.push_back(position);
				}

				// Read normals:
				if (readingNormals)
				{
					std::istringstream iss(line);
					Float3 normal;
					char delimiter;

					if (!(iss >> normal.x >> delimiter >> normal.y >> delimiter >> normal.z))
					{
						LOG_ERROR("Invalid normal values: {}", line);
						file.close();
						return MeshAsset{};
					}
					meshAsset.normals.push_back(normal);
				}

				// Read triangles:
				if (readingTriangles)
				{
					std::istringstream iss(line);
					Uint3 triangle;
					char delimiter;

					if (!(iss >> triangle.x >> delimiter >> triangle.y >> delimiter >> triangle.z))
					{
						LOG_ERROR("Invalid triangle values: {}", line);
						file.close();
						return MeshAsset{};
					}
					meshAsset.triangles.push_back(triangle);
				}

				// Read colors:
				if (readingColors)
				{
					std::istringstream iss(line);
					Float4 color;
					char delimiter;

					if (!(iss >> color.x >> delimiter >> color.y >> delimiter >> color.z >> delimiter >> color.w))
					{
						LOG_ERROR("Invalid color values: {}", line);
						file.close();
						return MeshAsset{};
					}
					meshAsset.colors.push_back(color);
				}

				// Read uvs:
				if (readingUvs)
				{
					std::istringstream iss(line);
					Float4 uvs;
					char delimiter;

					if (!(iss >> uvs.x >> delimiter >> uvs.y >> delimiter >> uvs.z >> delimiter >> uvs.w))
					{
						LOG_ERROR("Invalid uvs values: {}", line);
						file.close();
						return MeshAsset{};
					}
					meshAsset.uvs.push_back(uvs);
				}
			}
			file.close();

			// Construct mesh and return it:
			return meshAsset;
		}
	}
}