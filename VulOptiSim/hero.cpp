#include "pch.h"
#include "hero.h"

Hero::Hero(const std::string& model, const std::string& texture, const Transform& transform, const float speed)
    : model(model), texture(texture), transform(transform), speed(speed)
{
}

void Hero::update(const float delta_time, const Terrain& terrain)
{
    if (!active)
    {
        return;
    }

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

            //Face target
            face_target(target_direction);

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

void Hero::set_route(const std::vector<glm::vec2>& new_route)
{
    route = new_route;
}

void Hero::push(glm::vec2 direction, float magnitude)
{
    if (!active)
    {
        return;
    }

    force += direction * magnitude;
}

void Hero::take_damage(int damage)
{
    if (!active)
    {
        return;
    }

    health -= damage;

    if (health <= 0)
    {
        health = 0;

        active = false;
        //TODO: RIP animation
    }
}

void Hero::drain_mana(int cost)
{
    mana -= cost;

    if (mana < 0)
    {
        mana = 0;
    }
}

bool Hero::collision(const glm::vec3& position, float radius) const
{
    return circle_collision(transform.position, collision_radius, position, radius);
}

bool Hero::collision(const glm::vec2& min, const glm::vec2 max) const
{
    glm::vec2 position = transform.get_position2d();

    return aabb_circle_collision(min, max, position, collision_radius);
}

glm::vec3 Hero::get_position() const
{
    return transform.position;
}

glm::vec2 Hero::get_position2d() const
{
    return transform.get_position2d();
}

float Hero::get_collision_radius() const
{
    return collision_radius;
}

void Hero::face_target(const glm::vec2& target)
{
    glm::vec2 facing_direction = glm::normalize(target);

    float angle = atan2f(facing_direction.x, facing_direction.y);

    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 1.0f, 0.0f));

    transform.rotation = rotation;
}

void Hero::draw(vulvox::Renderer* renderer) const
{
    renderer->draw_model(model, texture, transform.get_matrix());
}