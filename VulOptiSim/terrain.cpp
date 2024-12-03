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

    terrain_transforms.reserve(length * width);
    texture_indices.reserve(length * width);
    terrain_heights.reserve(length * width);

    int lowest = std::numeric_limits<int>::max();
    for (size_t i = 0; i < width * length; i++)
    {
        if (data[i] < lowest)
        {
            lowest = data[i];
        }
    }

    for (int z = 0; z < length; z++)
    {
        for (int x = 0; x < width; x++)
        {

            int height = data[(z * image_width) + x] - lowest + 1;

            glm::mat4& voxel_transform = terrain_transforms.emplace_back(1.0f);
            terrain_heights.emplace_back(height);
            voxel_transform = glm::translate(voxel_transform, glm::vec3(x * tile_width + tile_width / 2, height / 2, z * tile_length + tile_width / 2));
            voxel_transform = glm::scale(voxel_transform, glm::vec3(tile_width, height, tile_length));


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

float Terrain::get_height(const glm::vec2& position2d) const
{
    int x = static_cast<int>(position2d.x / tile_width);
    int y = static_cast<int>(position2d.y / tile_length);

    int index = (y * width) + x;

    return terrain_heights.at(index);
}

/// <summary>
/// Uses a pathfinding algorithm to find the shortest path from given start_position to target_position.
/// Note: Path is stored from end to start point.
/// </summary>
std::vector<glm::vec2> Terrain::find_route(const glm::vec2& start_position, const glm::vec2& target_position) const
{
    glm::ivec2 start_tile{ start_position.x / tile_width, start_position.y / tile_length };
    glm::ivec2 target_tile{ target_position.x / tile_width, target_position.y / tile_length };

    std::queue<glm::ivec2> queue;
    queue.push(start_tile);

    std::unordered_set<glm::ivec2> visited;
    visited.insert(start_tile);

    //This hash map is used to track the parents in the shortest path of each visited node
    std::unordered_map<glm::ivec2, glm::ivec2> parents;

    while (!queue.empty())
    {
        glm::ivec2 current = queue.front();
        queue.pop();

        if (current == target_tile)
        {
            return reconstruct_path(parents, start_tile, current);
        }

        std::vector<glm::ivec2> neighbours = get_neighbours(current);

        for (const glm::ivec2& neighbour : neighbours)
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
    return std::vector<glm::vec2>();
}

/// <summary>
/// Trace back a route from target to start by following the parents in the given parent list.
/// Also, scale for tile size.
/// </summary>
std::vector<glm::vec2> Terrain::reconstruct_path(const std::unordered_map<glm::ivec2, glm::ivec2>& parents, const glm::ivec2& start_position, const glm::ivec2& target_position) const
{
    std::vector<glm::vec2> path;

    for (glm::ivec2 current = target_position; current != start_position; current = parents.at(current))
    {
        path.emplace_back(static_cast<float>(current.x) * tile_width + tile_width / 2.f, static_cast<float>(current.y) * tile_length + tile_length / 2.f);
    }

    return path;
}

/// <summary>
/// Returns a list of neighbours of a given node (if they exist).
/// </summary>
std::vector<glm::ivec2> Terrain::get_neighbours(const glm::ivec2& node) const
{
    std::vector<glm::ivec2> neighbours;

    if (node.x > 0) neighbours.push_back({ node.x - 1, node.y });
    if (node.y > 0) neighbours.push_back({ node.x, node.y - 1 });
    if (node.x < width - 1) neighbours.push_back({ node.x + 1, node.y });
    if (node.y < length - 1) neighbours.push_back({ node.x, node.y + 1 });

    return neighbours;
}
