#include "pch.h"
#include "projectile.h"

Projectile::Projectile()
{
}

Projectile::Projectile(glm::vec3 spawn_position, Hero* target) : target(target), transform(spawn_position), animation_timer("fireball", 0, 33, 0.1f)
{
    transform.scale = glm::vec3(10.f);
    direction = glm::normalize(target->get_position() - spawn_position);
}

void Projectile::update(const float delta_time, const Camera& camera, const Shield& shield, std::vector<Hero>& heroes)
{
    if (active)
    {
        uptime += delta_time;

        if (uptime >= lifetime)
        {
            active = false;
            return;
        }

        if (target)
        {
            direction = glm::normalize(target->get_position() - transform.position);
        }

        transform.position += direction * speed * delta_time;

        rotate_to_camera(camera);

        animation_timer.update(delta_time);

        //Disable if the projectile collides with the shield
        if (shield.intersects(transform.get_position2d(), radius))
        {
            shield.absorb(heroes, transform.get_position2d());
            active = false;
        }

        check_collisions(heroes);
    }
}

void Projectile::check_collisions(std::vector<Hero>& heroes)
{
    for (const auto& hero : heroes)
    {
        if (hero.collision(transform.position, radius))
        {
            explode(heroes);

            break; //Projectile exploded, exit
        }
    }
}

void Projectile::explode(std::vector<Hero>& heroes)
{
    for (auto& hero : heroes)
    {
        if (hero.collision(transform.position, explosion_radius))
        {
            hero.take_damage(damage);
        }
    }

    active = false;
    //TODO: Explode
}

void Projectile::register_draw(Sprite_Manager<Projectile>& sprite_manager) const
{
    if (active)
    {
        sprite_manager.register_draw(*this);
    }
}

glm::mat4 Projectile::get_model_matrix() const
{
    return transform.get_matrix();
}

glm::uint32_t Projectile::get_texture_index() const
{
    return animation_timer.get_current_frame();
}


void Projectile::rotate_to_camera(const Camera& camera)
{
    ////Rotate so the animation is always facing the camera
    glm::vec3 projectile_direction = glm::normalize(direction);
    glm::vec3 rot_axis = glm::normalize(glm::cross(glm::vec3(0, 1, 0), projectile_direction));
    float angle = acosf(glm::dot(glm::vec3(0, 1, 0), projectile_direction));

    glm::mat4 rotate_to_target = glm::rotate(glm::mat4(1.0f), angle, rot_axis);

    glm::vec3 normal_vec = rotate_to_target * glm::vec4(0.f, 0.f, 1.f, 0.f);
    glm::vec3 camera_direction = glm::normalize(camera.get_position() - transform.position);

    float camera_angle = acosf(glm::dot(normal_vec, camera_direction));

    glm::mat4 rotate_to_camera = glm::rotate(glm::mat4(1.0f), camera_angle, projectile_direction);


    transform.rotation = rotate_to_camera * rotate_to_target;



    //glm::vec3 facing_direction = glm::normalize(camera.get_position() - transform.position);



    //glm::vec3 facing_direction = glm::normalize(camera.get_position() - transform.position);

    //glm::vec3 rotation_axis = glm::normalize(direction);

    //float angle = acosf(glm::dot(glm::vec3(0, 1, 0), facing_direction));

    //glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), angle, rotation_axis);

    //transform.rotation = rotation;

}