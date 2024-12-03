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
            auto r = terrain.find_route(glm::uvec2(x, z), glm::uvec2(x + 100, z + 100));
            slimes.back().set_route(r);
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

