#pragma once

class Shield
{
public:
    Shield() = default;
    Shield(const std::string& texture_array_name, const std::vector<glm::vec3>& points);

    void draw(vulvox::Renderer* renderer) const;

    std::vector<glm::vec2> convex_hull(const std::vector<glm::vec2>& points) const;

private:

    static constexpr float shield_height = 50.f;
    static constexpr float shield_texture_scalar = 12.5f; // One full texture per this value

    std::string texture_name;
    uint32_t max_height = 10;
    uint32_t min_height = 0;
    std::vector<glm::vec2> convex_points;
};