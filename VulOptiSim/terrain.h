#pragma once
class Terrain
{
public:

    enum class Terrain_Types
    {
        Sea,
        Grass,
        Mountain,
        Stone
    };

    Terrain() = default;
    Terrain(const std::filesystem::path& path_to_height_map);

    void draw(vulvox::Renderer* renderer) const;

    float get_height(const glm::vec2& position2d) const;

    std::vector<glm::vec2> find_route(const glm::vec2& start_position, const glm::vec2& target_position) const;

    bool in_bounds(const glm::vec2& position2d) const;
    void clamp_to_bounds(glm::vec2& position2d) const;

    int map_width = 0;
    int map_length = 0;

    float tile_width = 6.f;
    float tile_length = 6.f;
    float tile_height = 6.f;

    float terrain_width = 0.f;
    float terrain_length = 0.f;

    std::vector<Terrain_Types> tile_types;
    std::vector<float> terrain_heights;
    std::vector<glm::mat4> terrain_transforms;
    std::vector<uint32_t> texture_indices;

private:

    std::vector<glm::vec2> reconstruct_path(const std::unordered_map<glm::ivec2, glm::ivec2>& parents, const glm::ivec2& start_position, const glm::ivec2& target_position) const;
    std::vector<glm::ivec2> get_neighbours(const glm::ivec2& node) const;
    bool is_accessible(const glm::ivec2& tile, const glm::ivec2& from) const;

    struct Tile_Data
    {
        uint32_t height;
        uint32_t tile_type;
        uint32_t alpha;
    };

    std::vector<Tile_Data> read_map_file(const std::filesystem::path& path_to_height_map, int& map_width, int& map_length) const;

    int get_tile_index(const int x, const int y) const;
};
