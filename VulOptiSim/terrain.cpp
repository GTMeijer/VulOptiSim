#include "pch.h"
#include "terrain.h"

Terrain::Terrain(const std::filesystem::path& path_to_height_map)
{
    int image_width;
    int image_height;
    int channels;

    unsigned char* data = stbi_load(path_to_height_map.string().c_str(), &image_width, &image_height, &channels, 0);

    width = image_width;
    height = image_height;

    for (size_t z = 0; z < (height); z++)
    {
        //std::vector<glm::mat4>& current_row = terrain_transforms.emplace_back();
        for (size_t x = 0; x < (width); x++)
        {

            //glm::mat4& voxel_transform = current_row.emplace_back(1.0f);
            glm::mat4& voxel_transform = terrain_transforms.emplace_back(1.0f);
            float y = data[(z * image_width) + x];
            voxel_transform = glm::translate(voxel_transform, glm::vec3(x * 0.1f, y, z * 0.1f));
            voxel_transform = glm::scale(voxel_transform, glm::vec3(0.1f, 0.1f, 0.1f));
            //std::cout << glm::to_string(voxel_transform) << std::endl;
        }
    }




    stbi_image_free(data);
}
