// needs to be defined before including stb_image.h, but may not be in a header file!
#define STB_IMAGE_IMPLEMENTATION
#include "assetLoader.h"
#include "logger.h"
#include "stb_image.h"
#include <fstream>



namespace emberAssetLoader
{
	// Helper functions:
	void TrimLeadingWhitespace(std::string& str)
	{
		str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](unsigned char ch)
		{ return !std::isspace(ch); }));
	}



    // Image loaders:
	Image LoadImageFile(const std::filesystem::path& path, int desiredChannels, bool flipImage)
	{
		// Error handling:
		if (!std::filesystem::exists(path))
		{
			LOG_ERROR("Error: File does not exist: {}", path.string());
			return Image{};
		}
		if (!std::filesystem::is_regular_file(path))
		{
			LOG_ERROR("Error: Path is not a regular file: {}", path.string());
			return Image{};
		}

        Image image = {};

		// Load pixelData:
        int width = 0;
        int height = 0;
        int imageChannels = 0;
		stbi_set_flip_vertically_on_load(flipImage);
		stbi_uc* pPixels = stbi_load(path.string().c_str(), &width, &height, &imageChannels, desiredChannels);
		if (!pPixels)
			throw std::runtime_error("Failed to load image!");

        // Determine actual channel count:
        int usedChannels = (desiredChannels != 0) ? desiredChannels : imageChannels;

        // Copy pixels into vector:
        std::vector<std::byte> pixels(reinterpret_cast<std::byte*>(pPixels), reinterpret_cast<std::byte*>(pPixels + usedChannels * width * height));
        stbi_image_free(pPixels);

		return Image{ width, height, usedChannels, std::move(pixels) };
	}
	Image LoadImageCubeFiles(const std::filesystem::path& directoryPath, int desiredChannels, bool flipImage)
	{
		// Error handling:
		if (!std::filesystem::exists(directoryPath))
			throw std::runtime_error("Given directory path '" + directoryPath.string() + "' does not exist");
		if (!std::filesystem::is_directory(directoryPath))
			throw std::runtime_error("Given directory path '" + directoryPath.string() + "' is not a directory.");

		// Cube face names:
		static const std::array<std::string, 6> directions = { "px", "nx", "py", "ny", "pz", "nz" };

		// Collect file paths:
		std::vector<std::filesystem::path> filePaths(6);
		for (uint32_t i = 0; i < 6; i++)
		{
			bool found = false;
			for (const auto& entry : std::filesystem::directory_iterator(directoryPath))
			{
				if (entry.is_regular_file() && entry.path().stem() == directions[i])
				{
					filePaths[i] = entry.path();
					found = true;
					break;
				}
			}
			if (!found)
				throw std::runtime_error("Missing cubemap face: " + directions[i]);
		}

		// Load all 6 images and append pixel data continuously:
		std::vector<std::byte> data;
		int width = 0;
		int height = 0;
		for (size_t i = 0; i < filePaths.size(); i++)
		{
			emberAssetLoader::Image imageAsset = emberAssetLoader::LoadImageFile(filePaths[i], desiredChannels, flipImage);
			if (i == 0)
			{
				width = imageAsset.width;
				height = imageAsset.height;
			}
			else
			{
				if (imageAsset.width != width || imageAsset.height != height)
					throw std::runtime_error("Cubemap face size mismatch at: " + filePaths[i].string());
			}
			data.insert(data.end(), imageAsset.pixels.begin(), imageAsset.pixels.end());
		}

		return Image{ width, height, desiredChannels, std::move(data) };
	}

	// Mesh loaders:
    Mesh LoadMeshCsv(const std::filesystem::path& path)
    {
		// Error handling:
		if (!std::filesystem::exists(path))
		{
			LOG_ERROR("Error: File does not exist: {}", path.string());
			return Mesh{};
		}
		if (!std::filesystem::is_regular_file(path))
		{
			LOG_ERROR("Error: Path is not a regular file: {}", path.string());
			return Mesh{};
		}

		// Read mesh data:
		Mesh mesh = {};
		bool readingPositions = false;
		bool readingNormals = false;
		bool readingTriangles = false;
		bool readingColors = false;
		bool readingUvs = false;
		std::ifstream file(path);
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
					return Mesh{};
				}
				mesh.positions.push_back(position);
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
					return Mesh{};
				}
				mesh.normals.push_back(normal);
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
					return Mesh{};
				}
				mesh.triangles.push_back(triangle);
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
					return Mesh{};
				}
				mesh.colors.push_back(color);
			}

			// Read uvs:
			if (readingColors)
			{
				std::istringstream iss(line);
				Float4 uvs;
				char delimiter;

				if (!(iss >> uvs.x >> delimiter >> uvs.y >> delimiter >> uvs.z >> delimiter >> uvs.w))
				{
					LOG_ERROR("Invalid uvs values: {}", line);
					file.close();
					return Mesh{};
				}
				mesh.uvs.push_back(uvs);
			}
		}
		file.close();

		// Construct mesh and return it:
		return mesh;
    }
}