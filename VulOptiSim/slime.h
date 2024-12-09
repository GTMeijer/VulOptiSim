#pragma once

class Slime
{
public:

    Slime(const std::string& model, const std::string& texture, const Transform& transform, const float speed);

    void update(const float delta_time, const Terrain& terrain);
    void draw(vulvox::Renderer* renderer) const;

    void set_route(const std::vector<glm::vec2>& new_route);

    glm::vec3 get_position() const;

private:

    std::string model;
    std::string texture;

    Transform transform;
    float speed;

    std::vector<glm::vec2> route;
};

