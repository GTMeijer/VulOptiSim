#pragma once

class Lightning
{
public:

    Lightning();
    Lightning(glm::vec3 position);

    void update(const float delta_time, const Camera& camera);
    void draw(vulvox::Renderer* renderer) const;

    void update_animation(const float delta_time);

    void check_hit(std::vector<Slime>& slimes) const;

    glm::uint32 current_frame = 0;
    const int total_frames = 10;
    const float frame_time = 0.25f;
    float current_frame_time = 0.f;

    int damage_per_second = 100;
    const static int radius = 12;

    glm::vec2 plane_size{ 24.f, 24.f };

    Transform transform;
};

