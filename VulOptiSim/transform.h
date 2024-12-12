#pragma once
struct Transform
{
    Transform(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale);
    Transform(const glm::vec3& position, const glm::quat& rotation);
    Transform(const glm::vec3& position);
    Transform();

    glm::vec2 get_position2d() const;
    void set_position2d(const float x, const float z);
    void set_position2d(const glm::vec2& position2d);
    void set_height(const float height);
    void set_direction2d(const glm::vec2& direction2d);

    glm::mat4 get_matrix() const;

    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;

    //Offset for model origin
    glm::vec3 offset{ 0.f };
};

