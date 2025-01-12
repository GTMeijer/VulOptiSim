#pragma once

class Magic_Staff
{
public:

    Magic_Staff(const glm::vec3& position, const Terrain* terrain);

    void update(const float delta_time, std::vector<Slime>& slimes, std::vector<Lightning>& active_lightning, std::vector<Projectile>& active_projectiles);
    void draw(vulvox::Renderer* renderer) const;

    float get_lightning_cooldown() const { return lightning_cooldown - current_lightning_cooldown; };
    float get_shoot_cooldown() const { return shoot_cooldown - current_shoot_cooldown; };

private:

    void spawn_lightning(std::vector<Lightning>& active_lightning) const;
    void spawn_projectile(std::vector<Projectile>& active_projectiles, std::vector<Slime>& slimes) const;

    Slime* find_closest_target(std::vector<Slime>& slimes) const;

    //enum class Staff_Animation
    //{
    //    idle,
    //    spinning,
    //    shooting
    //};

    //Staff_Animation current_animation;

    ////TODO: Animation times
    //const float spin_time = 10.f;
    //const float idle_time = 10.f;
    //const float shoot_time = 10.f;

    float animation_time = 0.f;

    const float lightning_cooldown = 10.f;
    const float shoot_cooldown = 1.f;

    float current_lightning_cooldown = 0.f;
    float current_shoot_cooldown = 0.f;

    const float lightning_uptime = 5.f;
    float current_lightning_uptime = 0.f;

    Transform transform;

    const Terrain* terrain;
};