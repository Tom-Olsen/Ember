#include "meshAssetLoader.h"
#include "assetLoaderUtility.h"
#include "logger.h"
#include <array>
#include <cstdint>
#include <fstream>
#include <limits>
#include <sstream>
#include <string>



namespace emberAssetLoader
{
	namespace MeshAssetLoader
	{
		emberCommon::MeshGeometry LoadCsv(const std::filesystem::path& path)
		{
			// Error handling:
			if (!std::filesystem::exists(path))
			{
				LOG_ERROR("Error: File does not exist: {}", path.string());
				return emberCommon::MeshGeometry{};
			}
			if (!std::filesystem::is_regular_file(path))
			{
				LOG_ERROR("Error: Path is not a regular file: {}", path.string());
				return emberCommon::MeshGeometry{};
			}

			// Prepare data:
			emberCommon::MeshGeometry meshGeometry = {};
			bool readingPositions = false;
			bool readingNormals = false;
			bool readingTriangles = false;
			bool readingColors = false;
			bool readingUvs = false;
			bool readingSubmeshes = false;

			// Open file:
			std::ifstream file(path);
			if (!file.is_open())
			{
				LOG_ERROR("Error: Could not open mesh file: {}", path.string());
				return emberCommon::MeshGeometry{};
			}

			// Read mesh data:
			std::string line;
			while (std::getline(file, line))
			{
				Utility::TrimLeadingWhitespace(line);
				if (line == "" || line[0] == '#')
					continue;
				size_t lineEnd = line.find_last_not_of(" \t\r\n\f\v");
				line.erase(lineEnd + 1);

				// Check if data type block has changed:
				if (line == "positions:")
				{
					readingPositions = true;
					readingNormals = false;
					readingTriangles = false;
					readingColors = false;
					readingUvs = false;
					readingSubmeshes = false;
					continue;
				}
				else if (line == "normals:")
				{
					readingPositions = false;
					readingNormals = true;
					readingTriangles = false;
					readingColors = false;
					readingUvs = false;
					readingSubmeshes = false;
					continue;
				}
				else if (line == "triangles:")
				{
					readingPositions = false;
					readingNormals = false;
					readingTriangles = true;
					readingColors = false;
					readingUvs = false;
					readingSubmeshes = false;
					continue;
				}
				else if (line == "colors:")
				{
					readingPositions = false;
					readingNormals = false;
					readingTriangles = false;
					readingColors = true;
					readingUvs = false;
					readingSubmeshes = false;
					continue;
				}
				else if (line == "uvs:")
				{
					readingPositions = false;
					readingNormals = false;
					readingTriangles = false;
					readingColors = false;
					readingUvs = true;
					readingSubmeshes = false;
					continue;
				}
				else if (line == "submeshes:")
				{
					readingPositions = false;
					readingNormals = false;
					readingTriangles = false;
					readingColors = false;
					readingUvs = false;
					readingSubmeshes = true;
					continue;
				}

				// Read positions:
				if (readingPositions)
				{
					std::istringstream iss(line);
					Float3 position;
					char delimiter;

					bool valid = static_cast<bool>(iss >> position.x >> delimiter) && delimiter == ','
					          && static_cast<bool>(iss >> position.y >> delimiter) && delimiter == ','
					          && static_cast<bool>(iss >> position.z);
					iss >> std::ws;
					if (!valid || !iss.eof())
					{
						LOG_ERROR("Invalid position values: {}", line);
						file.close();
						return emberCommon::MeshGeometry{};
					}
					meshGeometry.positions.push_back(position);
				}

				// Read normals:
				if (readingNormals)
				{
					std::istringstream iss(line);
					Float3 normal;
					char delimiter;

					bool valid = static_cast<bool>(iss >> normal.x >> delimiter) && delimiter == ','
					          && static_cast<bool>(iss >> normal.y >> delimiter) && delimiter == ','
					          && static_cast<bool>(iss >> normal.z);
					iss >> std::ws;
					if (!valid || !iss.eof())
					{
						LOG_ERROR("Invalid normal values: {}", line);
						file.close();
						return emberCommon::MeshGeometry{};
					}
					meshGeometry.normals.push_back(normal);
				}

				// Read triangles:
				if (readingTriangles)
				{
					std::istringstream iss(line);
					Uint3 triangle;
					char delimiter;

					bool valid = static_cast<bool>(iss >> triangle.x >> delimiter) && delimiter == ','
					          && static_cast<bool>(iss >> triangle.y >> delimiter) && delimiter == ','
					          && static_cast<bool>(iss >> triangle.z);
					iss >> std::ws;
					if (!valid || !iss.eof())
					{
						LOG_ERROR("Invalid triangle values: {}", line);
						file.close();
						return emberCommon::MeshGeometry{};
					}
					meshGeometry.triangles.push_back(triangle);
				}

				// Read colors:
				if (readingColors)
				{
					std::istringstream iss(line);
					Float4 color;
					char delimiter;

					bool valid = static_cast<bool>(iss >> color.x >> delimiter) && delimiter == ','
					          && static_cast<bool>(iss >> color.y >> delimiter) && delimiter == ','
					          && static_cast<bool>(iss >> color.z >> delimiter) && delimiter == ','
					          && static_cast<bool>(iss >> color.w);
					iss >> std::ws;
					if (!valid || !iss.eof())
					{
						LOG_ERROR("Invalid color values: {}", line);
						file.close();
						return emberCommon::MeshGeometry{};
					}
					meshGeometry.colors.push_back(color);
				}

				// Read uvs:
				if (readingUvs)
				{
					std::istringstream iss(line);
					Float4 uvs;
					char delimiter;

					bool valid = static_cast<bool>(iss >> uvs.x >> delimiter) && delimiter == ','
					          && static_cast<bool>(iss >> uvs.y >> delimiter) && delimiter == ','
					          && static_cast<bool>(iss >> uvs.z >> delimiter) && delimiter == ','
					          && static_cast<bool>(iss >> uvs.w);
					iss >> std::ws;
					if (!valid || !iss.eof())
					{
						LOG_ERROR("Invalid uvs values: {}", line);
						file.close();
						return emberCommon::MeshGeometry{};
					}
					meshGeometry.uvs.push_back(uvs);
				}

				// Read submeshes:
				if (readingSubmeshes)
				{
					std::istringstream iss(line);
					std::array<int64_t, 4> values = {};
					char delimiter;

					bool valid = static_cast<bool>(iss >> values[0] >> delimiter) && delimiter == ','
					          && static_cast<bool>(iss >> values[1] >> delimiter) && delimiter == ','
					          && static_cast<bool>(iss >> values[2] >> delimiter) && delimiter == ','
					          && static_cast<bool>(iss >> values[3]);
					for (int64_t value : values)
						valid = valid && value >= 0 && value <= std::numeric_limits<uint32_t>::max();
					iss >> std::ws;
					if (!valid || !iss.eof())
					{
						LOG_ERROR("Invalid submesh values in '{}': {}", path.string(), line);
						file.close();
						return emberCommon::MeshGeometry{};
					}

					emberCommon::Submesh submesh;
					submesh.firstVertex = static_cast<uint32_t>(values[0]);
					submesh.vertexCount = static_cast<uint32_t>(values[1]);
					submesh.firstTriangle = static_cast<uint32_t>(values[2]);
					submesh.triangleCount = static_cast<uint32_t>(values[3]);
					meshGeometry.submeshes.push_back(submesh);
				}
			}
			if (file.bad())
			{
				LOG_ERROR("Error: Could not read mesh file: {}", path.string());
				return emberCommon::MeshGeometry{};
			}
			file.close();

			// Return the mesh geometry:
			return meshGeometry;
		}
	}
}