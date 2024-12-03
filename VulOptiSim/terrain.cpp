#include "pch.h"
#include "terrain.h"

Terrain::Terrain(const std::filesystem::path& path_to_height_map)
{
    int image_width;
    int image_height;
    int channels;

    unsigned char* data = stbi_load(path_to_height_map.string().c_str(), &image_width, &image_height, &channels, 1);

    width = image_width;
    length = image_height;

    terrain_transforms.reserve((length / 8) * (width / 8));
    texture_indices.reserve((length / 8) * (width / 8));

    int lowest = std::numeric_limits<int>::max();
    for (size_t i = 0; i < width * length; i++)
    {
        if (data[i] < lowest)
        {
            lowest = data[i];
        }
    }

    for (int z = 0; z < length; z += 8)
    {
        for (int x = 0; x < width; x += 8)
        {

            int height = data[(z * image_width) + x] - lowest + 1;

            //for (int i;  < length; ++)
            //{

            //}
            glm::mat4& voxel_transform = terrain_transforms.emplace_back(1.0f);
            voxel_transform = glm::translate(voxel_transform, glm::vec3(x / 8, height, z / 8));
            voxel_transform = glm::scale(voxel_transform, glm::vec3(1.f, height, 1.f));


            if (height < 15.f)
            {
                texture_indices.push_back(0);
            }
            else if (height < 40.f)
            {
                texture_indices.push_back(1);
            }
            else
            {
                texture_indices.push_back(2);

            }

        }
    }

    stbi_image_free(data);
}

void Terrain::draw(vulvox::Renderer* renderer) const
{
    renderer->draw_instanced_with_texture_array("cube", "texture_array_test", terrain_transforms, texture_indices);

}

/// <summary>
/// Uses a pathfinding algorithm to find the shortest path from given start_position to target_position.
/// Note: Path is stored from end to start point.
/// </summary>
std::vector<glm::uvec2> Terrain::find_route(const glm::uvec2& start_position, const glm::uvec2& target_position) const
{
    std::queue<glm::uvec2> queue;
    queue.push(start_position);

    std::unordered_set<glm::uvec2> visited;
    visited.insert(start_position);

    //This hash map is used to track the parents in the shortest path of each visited node
    std::unordered_map<glm::uvec2, glm::uvec2> parents;

    while (!queue.empty())
    {
        glm::uvec2 current = queue.front();
        queue.pop();

        if (current == target_position)
        {
            return reconstruct_path(parents, start_position, current);
        }

        std::vector<glm::uvec2> neighbours = get_neighbours(current);

        for (const glm::uvec2& neighbour : neighbours)
        {
            if (!visited.contains(neighbour))
            {
                visited.insert(neighbour);
                parents[neighbour] = current;
                queue.emplace(neighbour);
            }
        }
    }

    //Return empty list if we didn't reach the target position
    return std::vector<glm::uvec2>();
}

/// <summary>
/// Trace back a route from target to start by following the parents in the given parent list.
/// </summary>
std::vector<glm::uvec2> Terrain::reconstruct_path(const std::unordered_map<glm::uvec2, glm::uvec2>& parents, const glm::uvec2& start_position, const glm::uvec2& target_position) const
{
    std::vector<glm::uvec2> path;

    for (glm::uvec2 current = target_position; current != start_position; current = parents.at(current))
    {
        path.push_back(current);
    }

    return path;
}

/// <summary>
/// Returns a list of neighbours of a given node (if they exist).
/// </summary>
std::vector<glm::uvec2> Terrain::get_neighbours(const glm::uvec2& node) const
{
    std::vector<glm::uvec2> neighbours;

    if (node.x > 0) neighbours.push_back({ node.x - 1, node.y });
    if (node.y > 0) neighbours.push_back({ node.x, node.y - 1 });
    if (node.x < width - 1) neighbours.push_back({ node.x + 1, node.y });
    if (node.y < length - 1) neighbours.push_back({ node.x, node.y + 1 });

    return neighbours;
}
