#pragma once

inline float cross(const glm::vec2& p1, const glm::vec2& p2)
{
    return p1.x * p2.y - p2.x * p1.y;
}

/// <summary>
/// Calculate the side r lies with respect to the segment running from p to q.
/// >0 is clockwise
/// <0 is counter-clockwise
/// 0 is collinear
/// </summary>
inline float orientation(const glm::vec2& p, const glm::vec2& q, const glm::vec2& r)
{
    return (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);
}