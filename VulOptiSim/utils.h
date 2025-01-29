#pragma once

/// <summary>
/// Calculate the side r lies with respect to the segment running from p to q.
/// >0 is clockwise
/// <0 is counter-clockwise
/// 0 is collinear
/// </summary>
inline float orientation(const glm::vec2& p, const glm::vec2& q, const glm::vec2& r)
{
    float value = ((q.y - p.y) * (r.x - q.x)) - ((q.x - p.x) * (r.y - q.y));

    return value;
}

/// <summary>
/// Checks for an intersection between two circles
/// </summary>
inline bool circle_collision(const glm::vec2& circle_position, float radius, const glm::vec2& other_circle_position, float other_radius)
{
    float combined_radius = radius + other_radius;
    float combined_radius_squared = combined_radius * combined_radius;
    float distance_squared = glm::length2(other_circle_position - circle_position); //Squared length

    return distance_squared <= combined_radius_squared;
}

/// <summary>
/// Checks for an intersection between two spheres
/// </summary>
inline bool circle_collision(const glm::vec3& circle_position, float radius, const glm::vec3& other_circle_position, float other_radius)
{
    float combined_radius = radius + other_radius;
    float combined_radius_squared = combined_radius * combined_radius;
    float distance_squared = glm::length2(other_circle_position - circle_position); //Squared length

    return distance_squared <= combined_radius_squared;
}

/// <summary>
/// Checks for an intersection between an axis-aligned bounding box and a circle
/// </summary>
inline bool aabb_circle_collision(const glm::vec2& min, const glm::vec2& max, glm::vec2& position, float radius)
{
    //find the closest point on the collision box to the circles center
    glm::vec2 closest_point = { glm::clamp(position.x, min.x, max.x), glm::clamp(position.y, min.y, max.y) };

    //check if the distance from the closest point to the circles center is less than or equal to the circles radius
    float distance_squared = glm::length2(closest_point - position);
    return distance_squared <= radius * radius;
}