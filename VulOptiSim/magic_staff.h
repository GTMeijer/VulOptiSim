#pragma once

class Magic_Staff
{
public:

    Magic_Staff(const glm::vec3& position, const Terrain* terrain);

    void update(const float delta_time, std::vector<Hero>& heroes, std::vector<Lightning>& active_lightning, std::vector<Projectile>& active_projectiles);
    void draw(vulvox::Renderer* renderer) const;

    float get_lightning_cooldown() const { return lightning_cooldown - current_lightning_cooldown; };
    float get_shoot_cooldown() const { return shoot_cooldown - current_shoot_cooldown; };

private:

    void spawn_lightning(std::vector<Lightning>& active_lightning) const;
    void spawn_projectile(std::vector<Projectile>& active_projectiles, std::vector<Hero>& heroes) const;

    Hero* find_closest_target(std::vector<Hero>& heroes) const;

    float animation_time = 0.f;

    const float lightning_cooldown = 7.5f;
    const float shoot_cooldown = 1.f;

    float current_lightning_cooldown = 0.f;
    float current_shoot_cooldown = 0.f;

    const float lightning_uptime = 5.f;
    float current_lightning_uptime = 0.f;

    Transform transform;

    const Terrain* terrain;
};