#pragma once

class Shield
{
public:
    Shield() = default;
    Shield(const std::string& texture_array_name, const std::vector<Hero>& heroes);

    void draw(vulvox::Renderer* renderer) const;

    std::vector<glm::vec2> convex_hull(std::vector<glm::vec2> points) const;

    bool intersects(const glm::vec2& circle_center, float radius) const;
    void absorb(std::vector<Hero>& heroes, glm::vec2 point) const;

private:

    glm::vec2 calculate_centroid();
    void grow_from_centroid();

    int mana_cost = 100;
    int n_to_sustain = 10;

    static constexpr float shield_height = 50.f;
    static constexpr float shield_texture_scalar = 12.5f; // One full texture per this value

    std::string texture_name;
    float max_height = 10;
    float min_height = 0;
    std::vector<glm::vec2> convex_hull_points;
};