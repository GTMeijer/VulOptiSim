#pragma once

class Slime
{
public:

    Slime(const std::string& model, const std::string& texture, const Transform& transform, const float speed);

    void update(const float delta_time, const Terrain& terrain);
    void draw(vulvox::Renderer* renderer) const;

    void set_route(const std::vector<glm::vec2>& new_route);

    void push(glm::vec2 direction, float magnitude);

    glm::vec3 get_position() const;
    glm::vec2 get_position2d() const;

    float get_collision_radius() const;

    int health = 1000;

private:

    std::string model;
    std::string texture;

    Transform transform;
    float speed;

    float collision_radius = 1.5f;
    glm::vec2 force = glm::vec2{ 0.f,0.f };

    std::vector<glm::vec2> route;

};

