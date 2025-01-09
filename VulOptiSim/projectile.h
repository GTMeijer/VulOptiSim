#pragma once

class Projectile
{
public:
    Projectile();
    Projectile(glm::vec3 spawn_position, Slime* target);


    void update(const float delta_time, const Camera& camera);
    void register_draw(Sprite_Manager<Projectile>& sprite_manager) const;


    glm::mat4 get_model_matrix() const;
    glm::uint32_t get_texture_index() const;

    bool is_active() const { return active; }

private:

    void rotate_to_camera(const Camera& camera);

    bool active = true;

    Slime* target = nullptr;

    float speed = 100.f;
    glm::vec3 direction;
    float uptime = 0.f;
    float lifetime = 100.f;
    Transform transform;

    Sprite_Animation animation_timer;

};