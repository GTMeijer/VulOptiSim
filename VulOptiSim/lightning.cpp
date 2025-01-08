#include "pch.h"
#include "lightning.h"

Lightning::Lightning() = default;

Lightning::Lightning(glm::vec3 position) : transform(position)
{
    transform.scale = glm::vec3(plane_size.x, plane_size.y, 1.f);
    transform.position.y += plane_size.y / 4.f;
}

void Lightning::update(const float delta_time, const Camera& camera)
{
    //Rotate so the animation is always facing the camera
    glm::vec3 facing_direction = camera.get_position() - transform.position;
    facing_direction.y = 0.f; //Only rotate the x and z
    facing_direction = glm::normalize(facing_direction);

    float angle = atan2f(facing_direction.x, facing_direction.z);

    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 1.0f, 0.0f));

    transform.rotation = rotation;

    current_frame_time += delta_time;
    update_animation(delta_time);
}

void Lightning::draw(vulvox::Renderer* renderer) const
{
    renderer->draw_planes("lightning", { transform.get_matrix() }, { current_frame }, { glm::vec4(0,0,1,1) });
}

void Lightning::update_animation(const float delta_time)
{
    current_frame_time += delta_time; //accumulate delta time

    //check if it's time to switch to the next frame
    if (current_frame_time >= frame_time)
    {
        current_frame_time -= frame_time; //subtract the frame time (handles overflows)
        current_frame = (current_frame + 1) % total_frames; //move to the next frame, loop if needed
    }
}

void Lightning::check_hit(std::vector<Slime>& slimes) const
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
