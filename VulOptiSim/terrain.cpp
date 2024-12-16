#include "pch.h"
#include "terrain.h"

Terrain::Terrain(const std::filesystem::path& path_to_height_map)
{
    int image_width;
    int image_height;
    int channels;

    unsigned char* data = stbi_load(path_to_height_map.string().c_str(), &image_width, &image_height, &channels, 1);

    tiles_x = image_width;
    tiles_y = image_height;

    terrain_width = tiles_x * tile_width;
    terrain_length = tiles_y * tile_length;

    terrain_transforms.reserve(tiles_y * tiles_x);
    texture_indices.reserve(tiles_y * tiles_x);
    terrain_heights.reserve(tiles_y * tiles_x);
    tile_types.reserve(tiles_y * tiles_x);

    int lowest = std::numeric_limits<int>::max();
    for (size_t i = 0; i < tiles_x * tiles_y; i++)
    {
        if (data[i] < lowest)
        {
            lowest = data[i];
        }
    }

    for (int z = 0; z < tiles_y; z++)
    {
        for (int x = 0; x < tiles_x; x++)
        {

            int height = data[(z * image_width) + x] - lowest + 1;

            glm::mat4& voxel_transform = terrain_transforms.emplace_back(1.0f);
            terrain_heights.emplace_back(height);
            voxel_transform = glm::translate(voxel_transform, glm::vec3(x * tile_width + tile_width / 2, height / 2, z * tile_length + tile_width / 2));
            voxel_transform = glm::scale(voxel_transform, glm::vec3(tile_width, height, tile_length));


            if (height < 1.f)
            {
                texture_indices.push_back(0);
                tile_types.push_back(Terrain_Types::Sea);
            }
            else if (height < 40.f)
            {
                texture_indices.push_back(1);
                tile_types.push_back(Terrain_Types::Grass);
            }
            else
            {
                texture_indices.push_back(2);
                tile_types.push_back(Terrain_Types::Mountain);
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

    int index = get_tile_index(x, y);

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

bool Terrain::in_bounds(const glm::vec2& position2d) const
{
    if (position2d.x > 0.f && position2d.y > 0.f && position2d.x < terrain_width && position2d.y < terrain_length)
    {
        return true;
    }
    return false;
}

void Terrain::clamp_to_bounds(glm::vec2& position2d) const
{
    position2d.x = std::clamp(position2d.x, 0.f, terrain_width);
    position2d.y = std::clamp(position2d.y, 0.f, terrain_length);
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
/// Returns a list of neighbours of a given node, if they exist and are accessible.
/// </summary>
std::vector<glm::ivec2> Terrain::get_neighbours(const glm::ivec2& node) const
{
    std::vector<glm::ivec2> neighbours;

    if (node.x > 0 && is_accessible({ node.x - 1, node.y }, node))
    {
        neighbours.push_back({ node.x - 1, node.y });
    }
    if (node.y > 0 && is_accessible({ node.x, node.y - 1 }, node))
    {
        neighbours.push_back({ node.x, node.y - 1 });
    }
    if (node.x < tiles_x - 1 && is_accessible({ node.x + 1, node.y }, node))
    {
        neighbours.push_back({ node.x + 1, node.y });
    }
    if (node.y < tiles_y - 1 && is_accessible({ node.x, node.y + 1 }, node))
    {
        neighbours.push_back({ node.x, node.y + 1 });
    }

    return neighbours;
}

/// <summary>
/// Checks if a tile is accessible from a given tile based on the height difference and tile type.
/// </summary>
bool Terrain::is_accessible(const glm::ivec2& tile, const glm::ivec2& from) const
{
    int tile_index = get_tile_index(tile.x, tile.y);
    int from_index = get_tile_index(from.x, from.y);

    if (tile_types[tile_index] == Terrain_Types::Sea || tile_types[tile_index] == Terrain_Types::Mountain)
    {
        return false;
    }

    if (terrain_heights[tile_index] - terrain_heights[from_index] > 3)
    {
        return false;
    }

    return true;
}

/// <summary>
/// Helper function that provides the internal vector index based on tile coordinates.
/// </summary>
int Terrain::get_tile_index(const int x, const int y) const
{
    return (y * tiles_x) + x;
}
