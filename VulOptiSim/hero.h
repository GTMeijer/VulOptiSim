#pragma once

class Hero
{
public:

    Hero(const std::string& model, const std::string& texture, const Transform& transform, const float speed);

    void update(const float delta_time, const Terrain& terrain);
    void draw(vulvox::Renderer* renderer) const;

    void set_route(const std::vector<glm::vec2>& new_route);

    void push(glm::vec2 direction, float magnitude);

    void take_damage(int damage);
    void drain_mana(int cost);

    /// <summary>
    /// Collision check with a sphere
    /// </summary>
    bool collision(const glm::vec3& position, float radius) const;

    /// <summary>
    /// Collision check with a 2d collision box
    /// </summary>
    bool collision(const glm::vec2& min, const glm::vec2 max) const;

    glm::vec3 get_position() const;
    glm::vec2 get_position2d() const;

    float get_collision_radius() const;

    int get_health() const { return health; };
    int get_mana() const { return mana; };

    bool is_active() const { return active; };


private:

    void face_target(const glm::vec2& target);

    bool active = true;
    int health = 1000;
    int mana = 1000;

    std::string model;
    std::string texture;

    Transform transform;
    float speed;

    float collision_radius = 0.5f;
    glm::vec2 force = glm::vec2{ 0.f,0.f };

    std::vector<glm::vec2> route;

};

