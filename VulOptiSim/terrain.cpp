#include "pch.h"
#include "terrain.h"

Terrain::Terrain(const std::filesystem::path& path_to_height_map)
{
    int image_width;
    int image_height;
    int channels;

    unsigned char* data = stbi_load(path_to_height_map.string().c_str(), &image_width, &image_height, &channels, 1);

    width = image_width;
    height = image_height;

    terrain_transforms.reserve((height / 8) * (width / 8));
    texture_indices.reserve((height / 8) * (width / 8));

    for (size_t z = 0; z < (height); z += 8)
    {
        //std::vector<glm::mat4>& current_row = terrain_transforms.emplace_back();
        for (size_t x = 0; x < (width); x += 8)
        {

            //glm::mat4& voxel_transform = current_row.emplace_back(1.0f);
            glm::mat4& voxel_transform = terrain_transforms.emplace_back(1.0f);
            float y = data[(z * image_width) + x];
            voxel_transform = glm::translate(voxel_transform, glm::vec3(x / 8, y - (y / 2), z / 8));
            voxel_transform = glm::scale(voxel_transform, glm::vec3(1.f, y, 1.f));
            //std::cout << glm::to_string(voxel_transform) << std::endl;

            if (y < 140.f)
            {
                texture_indices.push_back(0);
            }
            else if (y < 160.f)
            {
                texture_indices.push_back(1);
            }
            else
            {
                texture_indices.push_back(2);

            }

        }
    }




    stbi_image_free(data);
}
