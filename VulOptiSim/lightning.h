#pragma once

class Lightning
{
public:

    Lightning();
    Lightning(glm::vec3 position);

    void update(const float delta_time, const Camera& camera, std::vector<Hero> heroes);
    void register_draw(Sprite_Manager<Lightning>& sprite_manager) const;


    glm::mat4 get_model_matrix() const;
    glm::uint32_t get_texture_index() const;

    bool is_active() const { return active; };

    const static int radius = 18;

private:

    void rotate_to_camera(const Camera& camera);

    void check_hits(std::vector<Hero>& heroes) const;

    Sprite_Animation animation_timer;

    int damage_per_frame = 1;

    float uptime = 0.f;
    float lifetime = 5.f;
    bool active = true;

    glm::vec2 plane_size{ 36.f, 36.f };

    glm::vec2 collision_box_min;
    glm::vec2 collision_box_max;

    Transform transform;
};

