#pragma once

class Scene
{
public:
    explicit Scene(vulvox::Renderer& renderer);

    void update(float delta_time);
    void draw();

    std::vector<int> sort(const std::vector<int>& to_sort) const;


private:

    int num_layers = 1;

    vulvox::Renderer* renderer;
    Camera camera;

    glm::mat4 konata_matrix;

    std::vector<glm::mat4> konata_matrices;
    std::vector<uint32_t> texture_indices;

    Terrain terrain;
};