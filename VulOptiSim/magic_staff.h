#pragma once
class Magic_Staff
{
public:

    Magic_Staff(const glm::vec3& position, const Terrain* terrain);

    void update(const float delta_time, const Camera& camera, std::vector<Slime>& slimes);
    void draw(vulvox::Renderer* renderer) const;

    float current_lightning_cooldown = 0.f;

private:

    void spawn_lightning();

    enum class Staff_Animation
    {
        idle,
        spinning,
        shooting
    };

    Staff_Animation current_animation;

    //Animation times
    const float spin_time = 10.f;
    const float idle_time = 10.f;
    const float shoot_time = 10.f;

    float animation_time = 0.f;

    const float lightning_cooldown = 10.f;
    const float shoot_cooldown = 1.f;

    float current_shoot_cooldown = 0.f;

    const float lightning_uptime = 5.f;
    float current_lightning_uptime = 0.f;

    std::vector<Lightning> active_lightning;

    Transform transform;

    const Terrain* terrain;
};