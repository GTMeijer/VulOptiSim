#include "pch.h"
#include "slime.h"

Slime::Slime(const std::string& model, const std::string& texture, const Transform& transform, const float speed)
    : model(model), texture(texture), transform(transform), speed(speed)
{
}

void Slime::update(const float delta_time, const Terrain& terrain)
{
    float distance = delta_time * speed;
    glm::vec2 position = transform.get_position2d();

    //Displace position based on accumulated forces
    position += force;
    force = glm::vec2{ 0.f,0.f };

    //Traverse route
    if (!route.empty())
    {
        while (!route.empty() && distance > 0.f)
        {
            glm::vec2 target = route.back();
            glm::vec2 target_direction = target - position;
            float distance_to_target = glm::length(target_direction);

            //Move towards the current target node
            //Prevent overshooting by checking the remaining distance
            if (distance_to_target - distance > 0.0001f)
            {
                position += distance * glm::normalize(target_direction);
                distance = 0.f;
            }
            else
            {
                position = target;
                distance -= distance_to_target;
            }

            //If next node is (nearly) reached, set target to next node
            if (glm::length2(target - position) < terrain.tile_width)
            {
                route.pop_back();
            }
        }
    }

    //Make sure we stay with the terrain bounds
    if (terrain.in_bounds(position))
    {
        transform.set_position2d(position);
    }
    else
    {
        terrain.clamp_to_bounds(position);
        transform.set_position2d(position);
    }

    transform.set_height(terrain.get_height(position));
}

void Slime::set_route(const std::vector<glm::vec2>& new_route)
{
    route = new_route;
}

void Slime::push(glm::vec2 direction, float magnitude)
{
    force += direction * magnitude;
}

void Slime::take_damage(int damage)
{
    health -= damage;

    if (health < 0)
    {
        health = 0;
    }
}

glm::vec3 Slime::get_position() const
{
    return transform.position;
}

glm::vec2 Slime::get_position2d() const
{
    return transform.get_position2d();
}

float Slime::get_collision_radius() const
{
    return collision_radius;
}

void Slime::draw(vulvox::Renderer* renderer) const
{
    renderer->draw_model(model, texture, transform.get_matrix());
}