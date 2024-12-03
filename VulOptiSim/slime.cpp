#include "pch.h"
#include "transform.h"
#include "slime.h"

Slime::Slime(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale, const float speed)
    : transform(position, rotation, scale), speed(speed)
{
}

void Slime::update(const float delta_time)
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
            if (distance - distance_to_target < 0.0001f)
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
    }
}

void Slime::set_route(std::vector<glm::vec2> new_route)
{
    route = new_route;
}
