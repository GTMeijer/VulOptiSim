#include "pch.h"
#include "scene.h"


Scene::Scene(vulvox::Renderer& renderer) : renderer(&renderer)
{
    glm::vec3 camera_pos{ 0.0f, 0.0f, 0.0f };
    glm::vec3 camera_up{ 0.0f, 1.0f, 0.0f };
    glm::vec3 camera_direction{ 0.0f, 0.0f, 1.0f };

    camera = Camera(camera_pos, camera_up, camera_direction);

    terrain = Terrain(TERRAIN_PATH);

    renderer.load_model("Konata", MODEL_PATH);
    renderer.load_texture("Konata", KONATA_MODAL_TEXTURE_PATH);

    renderer.load_model("frieren-blob", FRIEREN_BLOB_PATH);
    renderer.load_texture("frieren-blob", FRIEREN_BLOB_TEXTURE_PATH);

    renderer.load_model("cube", CUBE_MODEL_PATH);
    renderer.load_texture("cube", CUBE_SEA_TEXTURE_PATH);

    std::vector<std::filesystem::path> texture_paths{ CUBE_SEA_TEXTURE_PATH, CUBE_GRASS_TEXTURE_PATH, CUBE_MOUNTAIN_TEXTURE_PATH };
    renderer.load_texture_array("texture_array_test", texture_paths);

    std::vector<std::filesystem::path> shield_path{ SHIELD_TEXTURE_PATH };
    renderer.load_texture_array("shield", shield_path);

    Transform slime_transform;
    slime_transform.rotation = glm::quatLookAt(glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, 1.f, 0.f));
    slime_transform.offset = glm::vec3(0.f, 0.5f, 0.f);
    slime_transform.scale = glm::vec3(3.f);

    for (size_t i = 0; i < 20; i++)
    {
        for (size_t j = 0; j < 20; j++)
        {
            float x = i * 10.f + terrain.tile_width / 2;
            float z = j * 10.f + terrain.tile_length / 2;
            float y = terrain.get_height(glm::vec2(x, z));


            slime_transform.position = glm::vec3(x, y, z);


            slimes.emplace_back("frieren-blob", "frieren-blob", slime_transform, 10.f);
            //auto r = terrain.find_route(glm::uvec2(x, z), glm::uvec2(x + 100, z + 100));
            //slimes.back().set_route(r);
        }
    }

    for (size_t i = 0; i < konata_matrices.size(); i++)
    {
        texture_indices.push_back(i % 2);
    }
}

void Scene::update(const float delta_time)
{
    //Update camera on key presses
    float camera_speed = 100.0f;
    if (glfwGetKey(renderer->get_window(), GLFW_KEY_W) == GLFW_PRESS) { camera.move_forward(delta_time * camera_speed); }
    if (glfwGetKey(renderer->get_window(), GLFW_KEY_S) == GLFW_PRESS) { camera.move_backward(delta_time * camera_speed); }
    if (glfwGetKey(renderer->get_window(), GLFW_KEY_Q) == GLFW_PRESS) { camera.move_left(delta_time * camera_speed); }
    if (glfwGetKey(renderer->get_window(), GLFW_KEY_E) == GLFW_PRESS) { camera.move_right(delta_time * camera_speed); }
    if (glfwGetKey(renderer->get_window(), GLFW_KEY_A) == GLFW_PRESS) { camera.rotate_left(camera_speed * delta_time); }
    if (glfwGetKey(renderer->get_window(), GLFW_KEY_D) == GLFW_PRESS) { camera.rotate_right(camera_speed * delta_time); }
    if (glfwGetKey(renderer->get_window(), GLFW_KEY_SPACE) == GLFW_PRESS) { camera.move_up(delta_time * camera_speed); }
    if (glfwGetKey(renderer->get_window(), GLFW_KEY_Z) == GLFW_PRESS) { camera.move_down(delta_time * camera_speed); }

    //if (glfwGetKey(renderer->get_window(), GLFW_KEY_KP_ADD) == GLFW_PRESS)
    //{
    //    const int x_count = 251;
    //    const int y_count = 251;
    //    num_layers++;
    //
    //    konata_matrices.reserve(x_count * y_count * num_layers);
    //    texture_indices.reserve(x_count * y_count * num_layers);
    //
    //    for (size_t i = 0; i < y_count; i++)
    //    {
    //        for (size_t j = 0; j < x_count; j++)
    //        {
    //            glm::mat4 instance_model_matrix;
    //            instance_model_matrix = glm::translate(konata_matrix, glm::vec3(i * 10.f, (num_layers + 1) * 10.0f + 125.0f, j * 10.f));
    //            instance_model_matrix = glm::scale(instance_model_matrix, glm::vec3(10.f, 10.f, 10.f));
    //
    //            konata_matrices.push_back(instance_model_matrix);
    //
    //            texture_indices.push_back(((i * y_count) + j) % 2);
    //        }
    //    }
    //}

    for (auto& slime : slimes)
    {
        slime.update(delta_time, terrain);
    }

    renderer->set_view_matrix(camera.get_view_matrix());
}

void Scene::draw()
{
    renderer->draw_model("frieren-blob", "frieren-blob", glm::mat4{ 1.0f });

    terrain.draw(renderer);

    for (const auto& slime : slimes)
    {
        slime.draw(renderer);
    }

    glm::mat4 instance_model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(4.f, 33.f, 4.f)) * glm::scale(glm::mat4(1.0f), glm::vec3(50.f, 50.f, 50.f));

    renderer->draw_planes("shield", { instance_model_matrix }, { 0 }, { {0.f,2.5f, 0.f, 2.5f} });
}

std::vector<int> Scene::sort(const std::vector<int>& to_sort) const
{
    std::vector<int> sorted_list = to_sort;

    for (size_t i = 0; i < sorted_list.size(); i++)
    {
        int current_value = sorted_list.at(i);

        //For all values before the current index,
        //move all bigger values than current value one index forward
        size_t j = i;
        for (; j > 0 && sorted_list.at(j - 1) > current_value; j--)
        {
            sorted_list.at(j) = sorted_list.at(j - 1);
        }
        //Place the current value in the created gap
        sorted_list.at(j) = current_value;
    }

    return sorted_list;
}

std::vector<glm::vec2> Scene::convex_hull(const std::vector<glm::vec2>& points) const
{
    //It was the holiday season so I figured I'd use the gift wrapping algorithm :)

    if (points.size() <= 3)
    {
        return points;
    }

    size_t left_most_point = 0;

    //Find left most point, it is guaranteed to be part of the hull
    for (size_t p = 0; p < points.size(); p++)
    {
        if (points[p].x < points[left_most_point].x)
        {
            left_most_point = p;
        }
    }

    std::vector<glm::vec2> convex_hull;


    size_t point_on_hull = left_most_point;

    do
    {
        //Add last found point to the convex hull
        convex_hull.push_back(points[point_on_hull]);

        //We test all points until we found the most count-clockwise point with respect to the current edge
        size_t end_point = (point_on_hull + 1) % points.size();
        for (size_t test_point = 0; test_point < points.size(); test_point++)
        {
            if (orientation(points[point_on_hull], points[end_point], points[test_point]) < 0)
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

