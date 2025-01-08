#pragma once

#include "magic_staff.h"


class Scene
{
public:
    explicit Scene(vulvox::Renderer& renderer);

    void update(const float delta_time);
    void draw();

    std::vector<int> sort(const std::vector<int>& to_sort) const;
    std::vector<glm::vec2> convex_hull(const std::vector<glm::vec2>& points) const;

    void load_models_and_textures() const;
    void spawn_slimes();
    void spawn_staves();

private:

    //Toggle for following the character at the front
    bool follow_mode = false;

    void handle_input(const float delta_time);
    void show_controls();

    void show_health_values() const;

    glm::dvec2 prev_mouse_pos;

    std::vector<Slime> slimes;
    std::vector<Magic_Staff> staves;

    int num_layers = 1;

    vulvox::Renderer* renderer;
    Camera camera;

    Terrain terrain;

    Shield shield;
};