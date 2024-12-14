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

    renderer->set_view_matrix(camera.get_view_matrix());

    //Make slimes collide with each other
    for (size_t i = 0; i < slimes.size(); i++)
    {
        for (size_t j = 0; j < slimes.size(); j++)
        {
            if (i == j)
            {
                continue;
            }

            //If the collision radii of the two slimes overlap, push them away
            //We can already check this with the squared distance, this prevents two square roots (expensive)
            glm::vec2 direction = (slimes[i].get_position2d() - slimes[j].get_position2d());
            float distance_sqrd = glm::dot(direction, direction); //x^2 + y^2
            float collision_radius_sqrd = (slimes[i].get_collision_radius() + slimes[j].get_collision_radius());
            collision_radius_sqrd *= collision_radius_sqrd;

            if (distance_sqrd < collision_radius_sqrd)
            {
                slimes[i].push(glm::normalize(direction), 1.0f);
            }
        }
    }

    for (auto& slime : slimes)
    {
        slime.update(delta_time, terrain);
    }

    std::vector<glm::vec3> slime_positions;
    for (const auto& slime : slimes)
    {
        slime_positions.emplace_back(slime.get_position());
    }

    shield = Shield{ "shield", slime_positions };


}

void Scene::draw()
{
    glm::mat4 test_model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(2.f, 0.f, 0.f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.f, 1.f, 1.f));
    renderer->draw_model("frieren-blob", "frieren-blob", test_model_matrix);

    terrain.draw(renderer);

    for (const auto& slime : slimes)
    {
        slime.draw(renderer);
    }

    glm::mat4 instance_model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.f, 0.f, 0.f)) * glm::scale(glm::mat4(1.0f), glm::vec3(50.f, 100.f, 50.f));

    shield.draw(renderer);
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

