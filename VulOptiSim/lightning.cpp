#include "pch.h"
#include "lightning.h"

Lightning::Lightning() = default;

Lightning::Lightning(glm::vec3 position) : animation_timer("lightning", 0, 10, 0.25f), transform(position)
{
    transform.scale = glm::vec3(plane_size.x, plane_size.y, 1.f);
    transform.position.y += plane_size.y / 4.f;
}

void Lightning::update(const float delta_time, const Camera& camera)
{
    if (active)
    {
        uptime += delta_time;

        if (uptime >= lifetime)
        {
            active = false;
            return;
        }

        rotate_to_camera(camera);
        animation_timer.update(delta_time);
    }
}

void Lightning::register_draw(Sprite_Manager<Lightning>& sprite_manager) const
{
    if (active)
    {
        sprite_manager.register_draw(*this);
    }
}

void Lightning::check_hit(std::vector<Slime>& slimes) const
{
    if (active)
    {
        glm::vec2 position = transform.get_position2d();
        for (auto& slime : slimes)
        {
            glm::vec2 distance_vec = slime.get_position2d() - position;
            float distance_sqrd = glm::dot(distance_vec, distance_vec);

            if (distance_sqrd < (float)(radius * radius))
            {
                slime.take_damage(damage_per_second);
            }
        }
    }
}

glm::mat4 Lightning::get_model_matrix() const
{
    return transform.get_matrix();
}

glm::uint32_t Lightning::get_texture_index() const
{
    return animation_timer.get_current_frame();
}

void Lightning::rotate_to_camera(const Camera& camera)
{
    //Rotate so the animation is always facing the camera
    glm::vec3 facing_direction = camera.get_position() - transform.position;
    facing_direction.y = 0.f; //Only rotate the x and z
    facing_direction = glm::normalize(facing_direction);

    float angle = atan2f(facing_direction.x, facing_direction.z);

    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 1.0f, 0.0f));

    transform.rotation = rotation;
}
