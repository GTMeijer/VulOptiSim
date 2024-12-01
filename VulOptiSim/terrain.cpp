#include "pch.h"
#include "terrain.h"

Terrain::Terrain(const std::filesystem::path& path_to_height_map)
{
    int image_width;
    int image_height;
    int channels;

    unsigned char* data = stbi_load(path_to_height_map.string().c_str(), &image_width, &image_height, &channels, 1);

    width = image_width;
    length = image_height;

    terrain_transforms.reserve((length / 8) * (width / 8));
    texture_indices.reserve((length / 8) * (width / 8));

    int lowest = std::numeric_limits<int>::max();
    for (size_t i = 0; i < width * length; i++)
    {
        if (data[i] < lowest)
        {
            lowest = data[i];
        }
    }

    for (int z = 0; z < length; z += 8)
    {
        for (int x = 0; x < width; x += 8)
        {
            
            int height = data[(z * image_width) + x] - lowest + 1;

            //for (int i;  < length; ++)
            //{

            //}
            glm::mat4& voxel_transform = terrain_transforms.emplace_back(1.0f);
            voxel_transform = glm::translate(voxel_transform, glm::vec3(x / 8, height, z / 8));
            voxel_transform = glm::scale(voxel_transform, glm::vec3(1.f, height, 1.f));


            if (height < 15.f)
            {
                texture_indices.push_back(0);
            }
            else if (height < 40.f)
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

void Terrain::draw(vulvox::Renderer* renderer) const
{
    renderer->draw_instanced_with_texture_array("cube", "texture_array_test", terrain_transforms, texture_indices);

}
