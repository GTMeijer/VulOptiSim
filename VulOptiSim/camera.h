#pragma once
class Camera
{
public:

    Camera() = default;
    Camera(glm::vec3 position, glm::vec3 up, glm::vec3 direction, float movement_speed, float rotation_speed);

    glm::mat4 get_view_matrix()  const;

    void set_position(glm::vec3 new_position);
    void update_position(glm::vec3 position_offset);

    void set_up(glm::vec3 new_up);
    void update_up(const glm::mat4& transformation_matrix);

    void move_forward(float delta_time);
    void move_backward(float delta_time);
    void move_left(float delta_time);
    void move_right(float delta_time);
    void move_up(float delta_time);
    void move_down(float delta_time);
    void rotate_left(float delta_time);
    void rotate_right(float delta_time);

    void set_direction(glm::vec3 new_direction);
    void update_direction(const glm::mat4& transformation_matrix);

    void update_direction(const glm::vec2& offset);

private:

    glm::vec3 position;
    glm::vec3 up;
    glm::vec3 direction;

    float rotation_speed;
    float movement_speed;
};

