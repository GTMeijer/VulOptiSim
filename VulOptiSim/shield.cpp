#include "pch.h"
#include "shield.h"

Shield::Shield(const std::string& texture_array_name, const std::vector<glm::vec3>& points)
    : texture_name(texture_array_name)
{

    if (points.empty())
    {
        return;
    }

    std::vector<glm::vec2> points_2d{};
    points_2d.reserve(points.size());

    float highest_point = points[0].y;
    for (auto& p : points)
    {
        points_2d.emplace_back(p.x, p.z); //convert to 2d: 3d y-axis points up, so use z

        if (highest_point < p.y)
        {
            highest_point = p.y;
        }
    }

    convex_points = convex_hull(points_2d);
}

void Shield::draw(vulvox::Renderer* renderer) const
{
    if (convex_points.size() <= 1)
    {
        return;
    }

    std::vector<glm::mat4> transforms;
    transforms.reserve(convex_points.size());

    std::vector<uint32_t> texture_indices(convex_points.size(), 0);

    std::vector<glm::vec4> uvs;
    uvs.reserve(convex_points.size());

    //Place the shield planes between each consecutive two points in the convex hull
    float shield_start = 0.f;
    for (size_t i = 0; i < convex_points.size(); i++)
    {
        size_t next_index = (i + 1) % convex_points.size();

        //Calculate shield plane position, rotation, and size
        //https://www.geogebra.org/3d/n2w444tn
        glm::vec2 position = convex_points.at(i); //Position is based on bottom left corner
        glm::vec2 distance_vec = convex_points.at(next_index) - convex_points.at(i);

        //Get outward vector by calculating the perpendicular vector
        glm::vec2 line_vec = glm::normalize(convex_points.at(next_index) - convex_points.at(i));

        glm::mat4 rotation{ 1.0f };
        rotation[0] = glm::vec4(line_vec.x, 0.f, line_vec.y, 0.f); //right
        rotation[1] = glm::vec4(0.f, 1.f, 0.f, 0.f); //up
        rotation[2] = glm::vec4(line_vec.y, 0.f, -line_vec.x, 0.f); //forward

        float shield_length = glm::length(distance_vec);

        //Move the plane in between the two points
        glm::mat4 translate = glm::translate(glm::mat4(1.f), glm::vec3(position.x, 0.f, position.y));
        glm::mat4 scale = glm::scale(glm::mat4(1.f), glm::vec3(shield_length, shield_height, 1));
        glm::mat4 model_matrix = translate * rotation * scale;

        transforms.push_back(model_matrix);

        uvs.emplace_back(glm::vec4{ shield_start / shield_texture_scalar, 0.f, (shield_start + shield_length) / shield_texture_scalar, shield_height / shield_texture_scalar });

        shield_start += shield_length;
    }

    renderer->draw_planes(texture_name, transforms, texture_indices, uvs);
}


std::vector<glm::vec2> Shield::convex_hull(const std::vector<glm::vec2>& all_points) const
{
    //It was the holiday season so I figured I'd use the gift wrapping algorithm :)

    if (all_points.size() <= 3)
    {
        return all_points;
    }

    size_t left_most_point = 0;

    //Find left most point, it is guaranteed to be part of the hull
    for (size_t p = 0; p < all_points.size(); p++)
    {
        if (all_points[p].x < all_points[left_most_point].x)
        {
            left_most_point = p;
        }
    }

    std::vector<glm::vec2> convex_hull;


    size_t point_on_hull = left_most_point;

    do
    {
        //Add last found point to the convex hull
        convex_hull.push_back(all_points[point_on_hull]);

        //We test all points to find the most counter-clockwise point with respect to the current edge
        size_t end_point = (point_on_hull + 1) % all_points.size();
        for (size_t test_point = 0; test_point < all_points.size(); test_point++)
        {
            if (orientation(all_points[point_on_hull], all_points[end_point], all_points[test_point]) < 0)
            {
                //We found a point that lies further counter-clockwise
                end_point = test_point;
            }
        }

        //Start next edge
        point_on_hull = end_point;

    } while (point_on_hull != left_most_point);

    return convex_hull;
}
