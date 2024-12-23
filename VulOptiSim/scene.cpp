#include "pch.h"
#include "scene.h"


Scene::Scene(vulvox::Renderer& renderer) : renderer(&renderer)
{
    glfwGetCursorPos(this->renderer->get_window(), &prev_mouse_pos.x, &prev_mouse_pos.y);

    glm::vec3 camera_pos{ -28.2815380f, 305.485260f, -30.0800228f };
    glm::vec3 camera_up{ 0.338442326f, 0.869414926f, 0.359964609f };
    glm::vec3 camera_direction{ 0.595541596f, -0.494082689f, 0.633413374f };

    camera = Camera(camera_pos, camera_up, camera_direction, 100.f, 100.f);

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

    for (size_t i = 0; i < 80; i++)
    {
        for (size_t j = 0; j < 80; j++)
        {
            float x = (i * terrain.tile_width) + terrain.tile_width * 0.5;
            float z = (j * terrain.tile_length) + terrain.tile_length * 0.5;
            float y = terrain.get_height(glm::vec2(x, z));


            slime_transform.position = glm::vec3(x, y, z);


            slimes.emplace_back("frieren-blob", "frieren-blob", slime_transform, 10.f);
            //auto r = terrain.find_route(glm::uvec2(x, z), glm::uvec2(x + 100, z + 100));
            //auto r = terrain.find_route(glm::uvec2(x, z), glm::uvec2(295, 1690));
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
    handle_input(delta_time);

    if (follow_mode)
    {
        auto it = std::max_element(slimes.begin(), slimes.end(),
            [](const Slime& a, const Slime& b) {
                return a.get_position().z < b.get_position().z;
            }
        );

        glm::vec3 new_camera_position{ -28.0f, 305.5f, it->get_position().z };
        camera.set_position(new_camera_position);

        glm::vec3 camera_to_furthest = it->get_position() - camera.get_position();
        camera.set_direction(camera_to_furthest);
    }


    renderer->set_view_matrix(camera.get_view_matrix());

    //Make slimes collide with each other
    //for (size_t i = 0; i < slimes.size(); i++)
    //{
    //    for (size_t j = 0; j < slimes.size(); j++)
    //    {
    //        if (i == j)
    //        {
    //            continue;
    //        }

    //        //If the collision radii of the two slimes overlap, push them away
    //        //We can already check this with the squared distance, this prevents two square roots (expensive)
    //        glm::vec2 direction = (slimes[i].get_position2d() - slimes[j].get_position2d());
    //        float distance_sqrd = glm::dot(direction, direction); //x^2 + y^2
    //        float collision_radius_sqrd = (slimes[i].get_collision_radius() + slimes[j].get_collision_radius());
    //        collision_radius_sqrd *= collision_radius_sqrd;

    //        if (distance_sqrd < collision_radius_sqrd)
    //        {
    //            slimes[i].push(glm::normalize(direction), 1.0f);
    //        }
    //    }
    //}

    for (auto& slime : slimes)
    {
        slime.update(delta_time, terrain);
    }

    //std::vector<glm::vec3> slime_positions;
    //for (const auto& slime : slimes)
    //{
    //    slime_positions.emplace_back(slime.get_position());
    //}

    //shield = Shield{ "shield", slime_positions };


}

void Scene::draw()
{
    //On using concurrency here:
    //  The graphics library copies the data to GPU memory so you can change the data after the draw functions of the renderer return.
    //  The actual drawing runs parallel to the host (CPU) execution.
    //  Make sure the data needed for drawing (position etc.) is ready before calling the corresponding draw functions or weird things happen.
    //  Calling draw functions outside of this functions lifetime will crash the program!

    glm::mat4 test_model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(2.f, 0.f, 0.f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.f, 1.f, 1.f));
    renderer->draw_model("frieren-blob", "frieren-blob", test_model_matrix);

    terrain.draw(renderer);

    for (const auto& slime : slimes)
    {
        slime.draw(renderer);
    }

    glm::mat4 instance_model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.f, 0.f, 0.f)) * glm::scale(glm::mat4(1.0f), glm::vec3(50.f, 100.f, 50.f));

    shield.draw(renderer);

    show_health_values();
    show_controls();
}

/// <summary>
/// Sorts all health values and displays them in a window.
/// </summary>
void Scene::show_health_values() const
{
    std::vector<int> health_values;
    for (const auto& s : slimes)
    {
        health_values.push_back(s.health);
    }

    health_values = sort(health_values);

    ImGui::Begin("Slimes Status Bars");

    ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.90f);
    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, { 0.f, 0.5f, 0.f, 1.0f }); //Green
    for (const int& hp : health_values)
    {
        std::string hp_text = std::format("{}/{}", hp, 1000);
        ImGui::ProgressBar((float)hp / 1000, ImVec2(-FLT_MIN, 0.0f), hp_text.c_str());
    }
    ImGui::PopStyleColor(1);
    ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);

    ImGui::End();
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

void Scene::handle_input(const float delta_time)
{
    //Toggle follow mode
    if (glfwGetKey(renderer->get_window(), GLFW_KEY_TAB) == GLFW_PRESS) { follow_mode = !follow_mode; }

    if (!follow_mode)
    {
        //Update camera on key presses
        if (glfwGetKey(renderer->get_window(), GLFW_KEY_W) == GLFW_PRESS) { camera.move_forward(delta_time); }
        if (glfwGetKey(renderer->get_window(), GLFW_KEY_S) == GLFW_PRESS) { camera.move_backward(delta_time); }
        if (glfwGetKey(renderer->get_window(), GLFW_KEY_Q) == GLFW_PRESS) { camera.move_left(delta_time); }
        if (glfwGetKey(renderer->get_window(), GLFW_KEY_E) == GLFW_PRESS) { camera.move_right(delta_time); }
        if (glfwGetKey(renderer->get_window(), GLFW_KEY_A) == GLFW_PRESS) { camera.rotate_left(delta_time); }
        if (glfwGetKey(renderer->get_window(), GLFW_KEY_D) == GLFW_PRESS) { camera.rotate_right(delta_time); }
        if (glfwGetKey(renderer->get_window(), GLFW_KEY_SPACE) == GLFW_PRESS) { camera.move_up(delta_time); }
        if (glfwGetKey(renderer->get_window(), GLFW_KEY_Z) == GLFW_PRESS) { camera.move_down(delta_time); }

        glm::dvec2 mouse_pos;
        glfwGetCursorPos(renderer->get_window(), &mouse_pos.x, &mouse_pos.y);

        glm::dvec2 mouse_offset = mouse_pos - prev_mouse_pos;
        prev_mouse_pos = mouse_pos;

        mouse_offset.x *= delta_time;
        mouse_offset.y *= delta_time;

        //Only move the camera using the mouse when shift is pressed
        if (glfwGetKey(renderer->get_window(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        {
            camera.update_direction(mouse_offset);
        }

    }
}

void Scene::show_controls()
{
    ImGui::Begin("Camera Controls Guide");

    // Follow Mode Toggle
    ImGui::Text("Follow Mode: %s", follow_mode ? "Enabled" : "Disabled");
    ImGui::Separator();

    // Movement Controls
    ImGui::Text("Movement Controls (When Follow Mode is Disabled):");
    ImGui::BulletText("[W] - Move Forward");
    ImGui::BulletText("[S] - Move Backward");
    ImGui::BulletText("[Q] - Move Left");
    ImGui::BulletText("[E] - Move Right");
    ImGui::BulletText("[A] - Rotate Left");
    ImGui::BulletText("[D] - Rotate Right");
    ImGui::BulletText("[SPACE] - Move Up");
    ImGui::BulletText("[Z] - Move Down");

    ImGui::Separator();

    // Mouse Controls
    ImGui::Text("Mouse Controls:");
    ImGui::BulletText("[Mouse + SHIFT] - Look Around");

    ImGui::Separator();

    // Additional Info
    ImGui::Text("Current Mouse Position:");
    ImGui::Text("X: %.2f, Y: %.2f", prev_mouse_pos.x, prev_mouse_pos.y);

    ImGui::Text("Camera Position:");
    glm::vec3 camera_pos = camera.get_position();
    ImGui::Text("X: %.2f, Y: %.2f, Z: %.2f", camera_pos.x, camera_pos.y, camera_pos.z);

    ImGui::Text("Camera Direction:");
    glm::vec3 camera_dir = camera.get_direction();
    ImGui::Text("X: %.2f, Y: %.2f, Z: %.2f", camera_dir.x, camera_dir.y, camera_dir.z);

    ImGui::Separator();

    ImGui::Text("Toggle Follow Mode: [TAB]");

    ImGui::End();
}
