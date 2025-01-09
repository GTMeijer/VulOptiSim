#pragma once

class Sprite_Animation
{
public:

    Sprite_Animation() = default;
    Sprite_Animation(const std::string& texture_name, int start_frame, int end_frame, float frame_time);

    void update(const float delta_time);

    glm::uint32 get_current_frame() const;


private:

    std::string texture_name;

    int start_frame;
    int end_frame;

    glm::uint32 current_frame = 0;
    float frame_time = 0.25f;
    float current_frame_time = 0.f;
};