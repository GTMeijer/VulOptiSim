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

    for (size_t i = 0; i < convex_points.size(); i++)
    {
        size_t next_index = i + 1 % convex_points.size();

        glm::vec2 midpoint = convex_points.at(i) + convex_points.at(next_index) / 0.5f;

        //Get outward vector by calculating the perpendicular (counter-clockwise) vector
        glm::vec2 line_vec = convex_points.at(next_index) - convex_points.at(i);
        glm::vec2 outward_vec = glm::vec2(-line_vec.y, line_vec.x);

        glm::mat4 rotation{ 1.0f };
        rotation[0] = glm::vec4(line_vec.x, 0.f, line_vec.y, 0.f); //right
        rotation[1] = glm::vec4(0.f, 1.f, 0.f, 0.f); //up
        rotation[2] = glm::vec4(outward_vec.x, 0.f, outward_vec.y, 0.f); //forward


        //Move the plane in between the two points
        glm::mat4 translate = glm::translate(glm::mat4(1.f), glm::vec3(midpoint.x, min_height, midpoint.y));
        glm::mat4 model_matrix = translate * rotation;

        transforms.push_back(model_matrix);

        uvs.emplace_back(glm::vec4{ 0.f, 1.f, 0.f, 1.f });
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
