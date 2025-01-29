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

    load_models_and_textures();

    spawn_heroes();

    spawn_staves();

    std::cout << "Scene loaded." << std::endl;
}

void Scene::load_models_and_textures() const
{
    //Load all the models and textures we're going to need into GPU memory

    //Terrain textures
    std::vector<std::filesystem::path> texture_paths{
        CUBE_SEA_TEXTURE_PATH,  //Sea
        CUBE_GRASS_FLOWER_TEXTURE_PATH, //Lab floor
        CUBE_CONCRETE_WALL_TEXTURE_PATH, //Lab walls
        CUBE_MOSS_TEXTURE_PATH }; //Floor
    renderer->load_texture_array("texture_array_test", texture_paths);

    //NPCs
    //renderer->load_model("konata", MODEL_PATH);
    //renderer->load_texture("konata", KONATA_MODAL_TEXTURE_PATH);

    renderer->load_model("frieren-blob", FRIEREN_PATH);
    renderer->load_texture("frieren-blob", FRIEREN_TEXTURE_PATH);

    renderer->load_model("staff", STAFF_PATH);
    renderer->load_texture("staff", STAFF_TEXTURE_PATH);

    renderer->load_model("cube", CUBE_MODEL_PATH);
    renderer->load_texture("cube", CUBE_SEA_TEXTURE_PATH);

    //Effects
    std::vector<std::filesystem::path> shield_path{ SHIELD_TEXTURE_PATH };
    renderer->load_texture_array("shield", shield_path);

    renderer->load_texture_array("lightning", LIGHTNING_TEXTURE_PATHS);

    renderer->load_texture_array("fireball", FIREBALL_TEXTURE_PATHS);
}

void Scene::spawn_heroes()
{
    Transform hero_transform;
    hero_transform.rotation = glm::quatLookAt(glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, 1.f, 0.f));
    hero_transform.scale = glm::vec3(1.f);

    float spawn_offset = terrain.tile_width / 3.f;

    int start_areas = 10;
    float start_area_tile_offset = 12.f;
    float spawn_start_y = terrain.tile_width * 3.f;

    float start_corner_y = 9.f * terrain.tile_width;

    std::cout << "Spawning characters and calculating routes..." << std::endl;

    for (int s = 0; s < start_areas; s++)
    {
        float start_area_offset = static_cast<float>(s) * start_area_tile_offset * terrain.tile_width;

        for (int i = 0; i < 30; i++)
        {
            for (int j = 0; j < 30; j++)
            {
                float x = start_corner_y + start_area_offset + ((float)i * spawn_offset);
                float z = spawn_start_y + ((float)j * spawn_offset);
                float y = terrain.get_height(glm::vec2(x, z));

                hero_transform.position = glm::vec3(x, y, z);

                heroes.emplace_back("frieren-blob", "frieren-blob", hero_transform, 20.f);

                auto r = terrain.find_route(glm::uvec2(x, z), glm::uvec2(69 * terrain.tile_width, 160 * terrain.tile_width));
                heroes.back().set_route(r);
            }
        }
    }
}
void Scene::spawn_staves()
{
    glm::vec2 spawn_start{ terrain.tile_width * 15.f,  terrain.tile_length * 48.f };
    float height = terrain.get_height(spawn_start) + 50.f;

    float spawn_offset_x = 12.f * terrain.tile_height;
    float spawn_offset_y = 40.f * terrain.tile_length;

    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            glm::vec3 position{ spawn_start.x + i * spawn_offset_x, height, spawn_start.y + j * spawn_offset_y };
            staves.emplace_back(position, &terrain);
        }
    }
}

size_t Scene::get_character_count() const
{
    return heroes.size();
}

size_t Scene::get_staff_count() const
{
    return staves.size();

}

void Scene::update(const float delta_time)
{
    handle_input(delta_time);

    if (follow_mode)
    {
        auto it = std::ranges::max_element(heroes,
            [](const Hero& a, const Hero& b) {
                return a.get_position().z < b.get_position().z;
            }
        );

        glm::vec3 new_camera_position{ -28.0f, 305.5f, it->get_position().z };
        camera.set_position(new_camera_position);

        glm::vec3 camera_to_furthest = it->get_position() - camera.get_position();
        camera.set_direction(camera_to_furthest);
    }


    renderer->set_view_matrix(camera.get_view_matrix());

    //Make heroes collide with each other
    for (size_t i = 0; i < heroes.size(); i++)
    {
        if (!heroes[i].is_active())
        {
            continue;
        }

        for (size_t j = 0; j < heroes.size(); j++)
        {
            if (i == j || !heroes[j].is_active())
            {
                continue;
            }

            //If the collision radii of the two heroes overlap, push them away
            if (circle_collision(heroes[i].get_position2d(), heroes[i].get_collision_radius(), heroes[j].get_position2d(), heroes[j].get_collision_radius()))
            {
                glm::vec2 direction = heroes[j].get_position2d() - heroes[i].get_position2d();

                heroes[j].push(glm::normalize(direction), (heroes[i].get_collision_radius()) - (glm::length(direction) / 2));
            }
        }
    }

    for (auto& hero : heroes)
    {
        hero.update(delta_time, terrain);
    }

    shield = Shield{ "shield", heroes };

    for (auto& staff : staves)
    {
        staff.update(delta_time, heroes, active_lightning, projectiles);
    }

    for (auto& lightning : active_lightning)
    {
        lightning.update(delta_time, camera, heroes);
    }

    //Remove inactive lightning
    const auto [first_l, last_l] = std::ranges::remove_if(active_lightning, [](const Lightning& l) { return !l.is_active(); });
    active_lightning.erase(first_l, last_l);

    for (auto& projectile : projectiles)
    {
        projectile.update(delta_time, camera, shield, heroes);
    }

    //Remove inactive projectiles
    const auto [first_p, last_p] = std::ranges::remove_if(projectiles, [](const Projectile& p) { return !p.is_active(); });
    projectiles.erase(first_p, last_p);
}

void Scene::draw()
{
    //On using concurrency here:
    //  The graphics library copies the data to GPU memory so you can change the data after the draw functions of the renderer return.
    //  The actual drawing runs parallel to the host (CPU) execution.
    //  Make sure the data needed for drawing (position etc.) is ready before calling the corresponding draw functions or weird things happen.
    //  Calling draw functions outside of this functions lifetime will crash the program!

    for (const auto& hero : heroes)
    {
        hero.draw(renderer);
    }

    terrain.draw(renderer);

    for (const auto& staff : staves)
    {
        staff.draw(renderer);
    }

    for (const auto& lightning : active_lightning)
    {
        lightning.register_draw(lightning_sprite_manager);
    }

    lightning_sprite_manager.draw(renderer);
    lightning_sprite_manager.reset();

    for (const auto& projectile : projectiles)
    {
        projectile.register_draw(projectile_sprite_manager);
    }

    projectile_sprite_manager.draw(renderer);
    projectile_sprite_manager.reset();

    shield.draw(renderer);

    show_health_values();
    show_mana_values();

    show_controls();
}

/// <summary>
/// Sorts all health values and displays them in a window.
/// </summary>
void Scene::show_health_values() const
{
    std::vector<int> health_values;
    for (const auto& h : heroes)
    {
        health_values.push_back(h.get_health());
    }

    health_values = sort(health_values);

    ImGui::Begin("Heroes Health Bars");

    ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.90f);
    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, { 0.f, 0.5f, 0.f, 1.0f }); //Green
    for (const int& hp : health_values)
    {
        std::stringstream hp_text;
        hp_text << hp << "/" << 1000;
        ImGui::ProgressBar((float)hp / 1000, ImVec2(-FLT_MIN, 0.0f), hp_text.str().c_str());
    }
    ImGui::PopStyleColor(1);
    ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);

    ImGui::End();
}

/// <summary>
/// Sorts all mana values and displays them in a window.
/// </summary>
void Scene::show_mana_values() const
{
    std::vector<int> mana_values;
    for (const auto& s : heroes)
    {
        mana_values.push_back(s.get_mana());
    }

    mana_values = sort(mana_values);

    ImGui::Begin("Heroes Mana Bars");

    ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.90f);
    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, { 0.f, 0.f, 0.5f, 1.0f }); //Blue
    for (const int& mana : mana_values)
    {
        std::stringstream mana_text;
        mana_text << mana << "/" << 1000;
        ImGui::ProgressBar((float)mana / 1000, ImVec2(-FLT_MIN, 0.0f), mana_text.str().c_str());
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
