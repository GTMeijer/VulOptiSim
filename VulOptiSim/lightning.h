#pragma once

class Lightning
{
public:

    Lightning();
    Lightning(glm::vec3 position);

    void update(const float delta_time, const Camera& camera);
    void register_draw(Sprite_Manager<Lightning>& sprite_manager) const;

    void check_hit(std::vector<Slime>& slimes) const;

    glm::mat4 get_model_matrix() const;
    glm::uint32_t get_texture_index() const;

    bool is_active() const { return active; };

    const static int radius = 12;

private:

    void rotate_to_camera(const Camera& camera);

    Sprite_Animation animation_timer;

    int damage_per_second = 100;

    float uptime = 0.f;
    float lifetime = 5.f;
    bool active = true;

    glm::vec2 plane_size{ 24.f, 24.f };

    Transform transform;
};

