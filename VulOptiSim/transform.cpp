#include "pch.h"
#include "transform.h"

Transform::Transform(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale)
    : position(position), rotation(rotation), scale(scale)
{
}

Transform::Transform(const glm::vec3& position, const glm::quat& rotation)
    : position(position), rotation(rotation), scale(1.f)

{
}

Transform::Transform(const glm::vec3& position)
    : position(position), rotation(), scale(1.f)
{
}

Transform::Transform()
    : position(0.0f), rotation(), scale(1.f)
{
}

glm::vec2 Transform::get_position2d() const
{
    //y is up, so return x and z
    return glm::vec2(position.x, position.z);
}

void Transform::set_position2d(const float x, const float z)
{
    position.x = x;
    position.z = z;
}

void Transform::set_position2d(const glm::vec2& position2d)
{
    position.x = position2d.x;
    position.z = position2d.y;
}

void Transform::set_height(const float height)
{
    position.y = height;
}

void Transform::set_direction2d(const glm::vec2& direction2d)
{
    // Define the original vector and the target direction
    glm::vec3 start_vector = glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f)); // Arbitrary vector
    glm::vec3 target_vector = glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f)); // Desired direction

    // Calculate the rotation quaternion
    //glm::quat rotation_quat = glm::rotation(start_vector, target_vector);
}

glm::mat4 Transform::get_matrix() const
{
    glm::mat4 translate = glm::translate(glm::mat4(1.0f), position + offset);
    glm::mat4 rotate = glm::mat4_cast(rotation);
    glm::mat4 scale_matrix = glm::scale(glm::mat4(1.0f), scale);

    return translate * rotate * scale_matrix;
}
