#pragma once

class Scene
{
public:
    explicit Scene(vulvox::Renderer& renderer);

    void update(const float delta_time);
    void draw();

    std::vector<int> sort(const std::vector<int>& to_sort) const;
    std::vector<glm::vec2> convex_hull(const std::vector<glm::vec2>& points) const;

private:

    std::vector<Slime> slimes;

    int num_layers = 1;

    vulvox::Renderer* renderer;
    Camera camera;

    glm::mat4 konata_matrix;

    std::vector<glm::mat4> konata_matrices;
    std::vector<uint32_t> texture_indices;

    Terrain terrain;

    Shield shield;
};