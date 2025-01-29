#include "pch.h"
#include "shield.h"

Shield::Shield(const std::string& texture_array_name, const std::vector<Hero>& heroes)
    : texture_name(texture_array_name)
{
    //Gather all hero positions if they have mana left
    std::vector<glm::vec3> points;
    for (const auto& hero : heroes)
    {
        if (hero.get_mana() > 0 && hero.is_active())
        {
            points.emplace_back(hero.get_position());
        }
    }

    if (points.empty())
    {
        return;
    }

    std::vector<glm::vec2> points_2d{};
    points_2d.reserve(points.size() * 4);

    float lowest_point = points[0].y;
    float highest_point = points[0].y;
    for (auto& p : points)
    {
        //convert to 2d: 3d y-axis points up, so use z
        //create square around point, this way the shield fits around the character nicely
        points_2d.emplace_back(p.x + .5f, p.z + .5f);
        points_2d.emplace_back(p.x + .5f, p.z - .5f);
        points_2d.emplace_back(p.x - .5f, p.z + .5f);
        points_2d.emplace_back(p.x - .5f, p.z - .5f);

        if (p.y < lowest_point)
        {
            lowest_point = p.y;
        }

        if (p.y > highest_point)
        {
            highest_point = p.y;
        }
    }

    min_height = lowest_point;
    max_height = highest_point;

    convex_hull_points = convex_hull(points_2d);

    //Make the shield a bit larger so it doesn't clip the heroes
    grow_from_centroid();
}

void Shield::draw(vulvox::Renderer* renderer) const
{
    if (convex_hull_points.size() <= 1)
    {
        return;
    }

    std::vector<glm::mat4> transforms;
    transforms.reserve(convex_hull_points.size());

    std::vector<uint32_t> texture_indices(convex_hull_points.size(), 0);

    std::vector<glm::vec4> uvs;
    uvs.reserve(convex_hull_points.size());

    //Place the shield planes between each consecutive two points in the convex hull
    float shield_start = 0.f;
    for (size_t i = 0; i < convex_hull_points.size(); i++)
    {
        size_t next_index = (i + 1) % convex_hull_points.size();

        //Calculate shield plane position, rotation, and size
        //https://www.geogebra.org/3d/n2w444tn
        glm::vec2 position = convex_hull_points.at(i); //Position is based on bottom left corner
        glm::vec2 distance_vec = convex_hull_points.at(next_index) - convex_hull_points.at(i);

        //Get outward vector by calculating the perpendicular vector
        glm::vec2 line_vec = glm::normalize(convex_hull_points.at(next_index) - convex_hull_points.at(i));

        glm::mat4 rotation{ 1.0f };
        rotation[0] = glm::vec4(line_vec.x, 0.f, line_vec.y, 0.f); //right
        rotation[1] = glm::vec4(0.f, 1.f, 0.f, 0.f); //up
        rotation[2] = glm::vec4(line_vec.y, 0.f, -line_vec.x, 0.f); //forward

        float shield_length = glm::length(distance_vec);

        //Move the plane in between the two points
        glm::mat4 translate = glm::translate(glm::mat4(1.f), glm::vec3(position.x, min_height + ((max_height + shield_height) - min_height) / 2, position.y) + glm::vec3(distance_vec.x, 0.f, distance_vec.y) / 2.f);
        glm::mat4 scale = glm::scale(glm::mat4(1.f), glm::vec3(shield_length, (max_height + shield_height) - min_height, 1));
        glm::mat4 model_matrix = translate * rotation * scale;

        transforms.push_back(model_matrix);

        uvs.emplace_back(glm::vec4{ shield_start / shield_texture_scalar, 0.f, (shield_start + shield_length) / shield_texture_scalar, ((max_height + shield_height) - min_height) / shield_texture_scalar });

        shield_start += shield_length;
    }

    renderer->draw_planes(texture_name, transforms, texture_indices, uvs);
}

std::vector<glm::vec2> Shield::convex_hull(std::vector<glm::vec2> all_points) const
{
    all_points.erase(std::ranges::unique(all_points, [](const glm::vec2& a, const glm::vec2& b)
        {
            //To prevent float rounding errors use epsilon (removes points nearly on top of each other)
            return glm::abs(a.x - b.x) < 0.001f && glm::abs(a.y - b.y) < 0.001f;
        }).begin(), all_points.end());

    glm::vec2 point_on_hull = all_points.at(0);

    //Find left most position, when equal, select lowest y
    for (const glm::vec2& point : all_points)
    {
        if (point.x < point_on_hull.x || (point.x == point_on_hull.x && point.y < point_on_hull.y))
        {
            point_on_hull = point;
        }
    }

    std::vector<glm::vec2> forcefield_hull;

    while (true)
    {
        //Add last found point
        forcefield_hull.push_back(point_on_hull);

        //Loop through all points replacing the endpoint with the current iteration every time 
        //it lies left of the current segment formed by point_on_hull and the current endpoint.
        //By the end we have a segment with no points on the left and thus a point on the convex hull.
        glm::vec2 endpoint = all_points.at(0);
        for (const glm::vec2& point : all_points)
        {
            if ((endpoint == point_on_hull) || orientation(point_on_hull, endpoint, point) < 0.f)
            {
                endpoint = point;
            }
        }

        //Set the starting point of the next segment to the found endpoint.
        point_on_hull = endpoint;

        //If we went all the way around we are done.
        if (endpoint == forcefield_hull.at(0))
        {
            break;
        }
    }

    return forcefield_hull;
}

bool Shield::intersects(const glm::vec2& circle_center, float radius) const
{
    if (convex_hull_points.size() < 2)
    {
        return false;
    }

    for (size_t i = 0; i < convex_hull_points.size(); i++)
    {
        //See: https://www.geogebra.org/calculator/ccfyg8bh

        glm::vec2 A = convex_hull_points[i];
        glm::vec2 B = convex_hull_points[(i + 1) % convex_hull_points.size()];

        //Compute the closest point on segment AB to the circle's center
        glm::vec2 AB = B - A;
        float t = glm::dot(circle_center - A, AB) / glm::dot(AB, AB);
        t = glm::clamp(t, 0.0f, 1.0f); //Clamp t to [0, 1] so we stay on the segment

        glm::vec2 closest_point = A + t * AB;

        // Check if the closest point is within the circle
        if (glm::distance2(circle_center, closest_point) <= radius * radius)
        {
            return true;
        }
    }

    return false; //No intersection
}

void Shield::absorb(std::vector<Hero>& heroes, glm::vec2 point) const
{
    std::vector<Hero*> closest_heroes;
    std::vector<float> closest_distances(n_to_sustain, std::numeric_limits<float>::max());

    for (auto& hero : heroes)
    {
        float distance_squared = glm::length2(hero.get_position2d() - point);

        //If we haven't filled the closest list yet, add this hero
        if (closest_heroes.size() < n_to_sustain)
        {
            closest_heroes.push_back(&hero);
            closest_distances[closest_heroes.size() - 1] = distance_squared;
        }
        else
        {
            //Check if this hero is closer than any in the current closest list
            size_t farthest = 0;
            for (size_t i = 1; i < closest_heroes.size(); i++)
            {
                if (closest_distances[farthest] < closest_distances[i])
                {
                    farthest = i;
                }
            }

            if (distance_squared < closest_distances[farthest])
            {
                //Replace the farthest hero with the current closer hero
                closest_heroes[farthest] = &hero;
                closest_distances[farthest] = distance_squared;
            }
        }
    }

    for (auto& hero : closest_heroes)
    {
        hero->drain_mana(mana_cost);
    }
}

/// <summary>
/// Calculate the center (centroid) of a set of points.
/// </summary>
glm::vec2 Shield::calculate_centroid()
{
    glm::vec2 sum = std::accumulate(convex_hull_points.begin(), convex_hull_points.end(), glm::vec2(0.0f, 0.0f));
    return sum / static_cast<float>(convex_hull_points.size());
}

void Shield::grow_from_centroid()
{
    //Push out the convex hull away from its centroid by a unit vector
    glm::vec2 centroid = calculate_centroid();

    for (auto& convex_point : convex_hull_points)
    {
        glm::vec2 grow_vector = glm::normalize(convex_point - centroid);
        convex_point += 2.f * grow_vector;
    }
}