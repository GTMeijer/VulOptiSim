#pragma once
class Terrain
{
public:

    Terrain() = default;
    Terrain(const std::filesystem::path& path_to_height_map);

    void draw(vulvox::Renderer* renderer) const;

    std::vector<glm::uvec2> find_route(const glm::uvec2& start_position, const glm::uvec2& target_position) const;

    int width = 0;
    int length = 0;

    //std::vector<std::vector<glm::mat4>> terrain_transforms;
    std::vector<glm::mat4> terrain_transforms;
    std::vector<uint32_t> texture_indices;

private:

    std::vector<glm::uvec2> reconstruct_path(const std::unordered_map<glm::uvec2, glm::uvec2>& parents, const glm::uvec2& start_position, const glm::uvec2& target_position) const;
    std::vector<glm::uvec2> get_neighbours(const glm::uvec2& node) const;
};
