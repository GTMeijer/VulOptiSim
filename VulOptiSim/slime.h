#pragma once

class Slime
{
public:
    Slime(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale, const float speed);

    void update(const float delta_time);

    void set_route(std::vector<glm::vec2> new_route);

private:

    Transform transform;
    float speed;

    std::vector<glm::vec2> route;
};

