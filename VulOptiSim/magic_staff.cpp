#include "pch.h"
#include "magic_staff.h"

Magic_Staff::Magic_Staff(const glm::vec3& position, const Terrain* terrain) : transform(position), terrain(terrain)
{
    transform.scale = glm::vec3(0.5f);
}

void Magic_Staff::update(
    const float delta_time,
    std::vector<Hero>& heroes,
    std::vector<Lightning>& active_lightning,
    std::vector<Projectile>& active_projectiles
)
{
    current_lightning_cooldown += delta_time;

    if (current_lightning_cooldown >= lightning_cooldown)
    {
        current_lightning_cooldown -= lightning_cooldown;
        spawn_lightning(active_lightning);
    }

    current_shoot_cooldown += delta_time;

    if (current_shoot_cooldown >= shoot_cooldown)
    {
        current_shoot_cooldown -= shoot_cooldown;
        spawn_projectile(active_projectiles, heroes);
    }
}

void Magic_Staff::draw(vulvox::Renderer* renderer) const
{
    renderer->draw_model("staff", "staff", transform.get_matrix());
}

void Magic_Staff::spawn_lightning(std::vector<Lightning>& active_lightning) const
{
    glm::vec2 staff_position = transform.get_position2d();

    //Spawn two lightning storms in a line under the staff
    for (int i = 0; i < 2; i++)
    {
        glm::vec2 lightning_position_2d = staff_position;
        lightning_position_2d.y += (float)i * (terrain->tile_length * 20.f);

        float height = terrain->get_height(lightning_position_2d);

        glm::vec3 lightning_position{ lightning_position_2d.x, height, lightning_position_2d.y };

        active_lightning.emplace_back(lightning_position);
    }
}

void Magic_Staff::spawn_projectile(std::vector<Projectile>& active_projectiles, std::vector<Hero>& heroes) const
{
    Hero* closest_target = find_closest_target(heroes);

    if (closest_target)
    {
        active_projectiles.emplace_back(transform.position, closest_target);
    }
}

Hero* Magic_Staff::find_closest_target(std::vector<Hero>& heroes) const
{
    Hero* closest_hero = nullptr;

    float closest_distance_squared = std::numeric_limits<float>::max();

    for (auto& hero : heroes)
    {
        if (!hero.is_active())
        {
            continue;
        }

        float distance_squared = glm::length2(hero.get_position2d() - transform.get_position2d()); // Use squared distance to avoid sqrt

        if (distance_squared < closest_distance_squared)
        {
            closest_distance_squared = distance_squared;
            closest_hero = &hero;
        }
    }

    return closest_hero;
}