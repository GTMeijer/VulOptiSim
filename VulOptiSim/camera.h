#pragma once
class Camera
{
public:

    Camera() = default;
    Camera(glm::vec3 position, glm::vec3 up, glm::vec3 direction, float movement_speed, float rotation_speed);

    glm::mat4 get_view_matrix()  const;

    glm::vec3 get_position() const;
    void set_position(const glm::vec3& new_position);
    void update_position(const glm::vec3& position_offset);

    glm::vec3 get_up() const;
    void set_up(const glm::vec3& new_up);
    void update_up(const glm::mat4& transformation_matrix);

    glm::vec3 get_direction() const;
    void set_direction(const glm::vec3& new_direction);
    void update_direction(const glm::mat4& transformation_matrix);
    void update_direction(const glm::vec2& offset);

    void move_forward(float delta_time);
    void move_backward(float delta_time);
    void move_left(float delta_time);
    void move_right(float delta_time);
    void move_up(float delta_time);
    void move_down(float delta_time);
    void rotate_left(float delta_time);
    void rotate_right(float delta_time);


private:

    glm::vec3 position{ 0.0f, 0.0f, 0.0f };
    glm::vec3 up{ 0.0f, 1.0f, 0.0f };
    glm::vec3 direction{ 0.0f, 0.0f, -1.0f };

    float rotation_speed{ 100.f };
    float movement_speed{ 100.f };
};

