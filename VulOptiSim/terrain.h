#pragma once
class Terrain
{
public:

    Terrain() = default;
    Terrain(const std::filesystem::path& path_to_height_map);

    void draw(vulvox::Renderer* renderer) const;


    int width = 0;
    int length = 0;

    //std::vector<std::vector<glm::mat4>> terrain_transforms;
    std::vector<glm::mat4> terrain_transforms;
    std::vector<uint32_t> texture_indices;
};

