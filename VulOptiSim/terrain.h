#pragma once
class Terrain
{
public:

    Terrain() = default;
    Terrain(const std::filesystem::path& path_to_height_map);

    void draw(vulvox::Renderer* renderer) const;

    float get_height(const glm::vec2& position2d) const;

    std::vector<glm::vec2> find_route(const glm::vec2& start_position, const glm::vec2& target_position) const;

    int width = 0;
    int length = 0;

    float tile_width = 5.f;
    float tile_length = 5.f;

    std::vector<float> terrain_heights;
    std::vector<glm::mat4> terrain_transforms;
    std::vector<uint32_t> texture_indices;

private:

    std::vector<glm::vec2> reconstruct_path(const std::unordered_map<glm::ivec2, glm::ivec2>& parents, const glm::ivec2& start_position, const glm::ivec2& target_position) const;
    std::vector<glm::ivec2> get_neighbours(const glm::ivec2& node) const;
};
