#include "pch.h"
#include "slime.h"

Slime::Slime(const std::string& model, const std::string& texture, const Transform& transform, const float speed)
    : model(model), texture(texture), transform(transform), speed(speed)
{
}

void Slime::update(const float delta_time, const Terrain& terrain)
{
    if (!route.empty())
    {
        float distance = delta_time * speed;
        glm::vec2 position = transform.get_position2d();

        while (!route.empty() && distance > 0.f)
        {
            glm::vec2 target = route.back();
            glm::vec2 target_direction = target - position;
            float distance_to_target = glm::length(target_direction);

            //Move towards the next node in the route
            if (distance_to_target - distance > 0.0001f)
            {
                position += distance * glm::normalize(target_direction);
                break;
            }

            //If next node is reached, set position to the node and delete it
            position = target;
            route.pop_back();

            //Remove distance traveled and continue iteration
            distance -= distance_to_target;
        }

        transform.set_position2d(position);
        transform.set_height(terrain.get_height(position));
    }
}

void Slime::set_route(const std::vector<glm::vec2>& new_route)
{
    route = new_route;
}

glm::vec3 Slime::get_position() const
{
    return transform.position;
}

void Slime::draw(vulvox::Renderer* renderer) const
{
    renderer->draw_model(model, texture, transform.get_matrix());
}