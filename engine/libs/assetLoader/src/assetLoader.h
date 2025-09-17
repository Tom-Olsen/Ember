#include "emberMath.h"
#include <cstddef>
#include <filesystem>
#include <vector>



namespace emberAssetLoader
{
    // Container structs:
    struct Image
    {
        int width;
        int height;
        int channels;
        std::vector<std::byte> pixels;
    };
    struct Mesh
    {
		std::vector<Float3> positions;
		std::vector<Float3> normals;
		std::vector<Float3> tangents;
		std::vector<Float4> colors;
		std::vector<Float4> uvs;
		std::vector<Uint3> triangles;
    };

    // Loaders:
    // desiredChannels = 0                    : return image as-is (channels = file’s actual number).
    // desiredChannels = 1, imageChannels == 1: copy
    // desiredChannels = 1, imageChannels == 2: copy R, drop A
    // desiredChannels = 1, imageChannels == 3: compute gray = 0.299R + 0.587G + 0.114B
    // desiredChannels = 1, imageChannels == 4: compute gray = 0.299R + 0.587G + 0.114B, drop A
    // desiredChannels = 2, imageChannels == 1: use R, set A=255
    // desiredChannels = 2, imageChannels == 2: copy
    // desiredChannels = 2, imageChannels == 3: copy RG
    // desiredChannels = 2, imageChannels == 4: copy RG
    // desiredChannels = 3, imageChannels == 1: copy R into all: RRR
    // desiredChannels = 3, imageChannels == 2: use R as gray in RGB, drop B
    // desiredChannels = 3, imageChannels == 3: copy
    // desiredChannels = 3, imageChannels == 4: copy RGB, drop A
    // desiredChannels = 4, imageChannels == 1: use R as gray in RGB, set A=255
    // desiredChannels = 4, imageChannels == 2: use R as gray in RGB, set A=B
    // desiredChannels = 4, imageChannels == 3: copy RGB, set A=255
    // desiredChannels = 4, imageChannels == 4: copy
    Image LoadImageFile(const std::filesystem::path& path, int desiredChannels = 0);
}