#include "pch.h"
#include "magic_staff.h"

Magic_Staff::Magic_Staff(const glm::vec3& position, const Terrain* terrain) : transform(position), terrain(terrain)
{
    transform.scale = glm::vec3(0.5f);
}

void Magic_Staff::update(const float delta_time, const Camera& camera, std::vector<Slime>& slimes)
{
    current_lightning_cooldown += delta_time;

    //check if it's time to switch to the next frame
    if (current_lightning_cooldown >= lightning_cooldown)
    {
        current_lightning_cooldown -= lightning_cooldown; //subtract the frame time (handles overflows)
        spawn_lightning();
    }

    if (!active_lightning.empty())
    {
        current_lightning_uptime += delta_time;

        if (current_lightning_uptime > lightning_uptime)
        {
            active_lightning.clear();
            current_lightning_uptime = 0.f;
        }
    }

    for (auto& lightning : active_lightning)
    {
        lightning.update(delta_time, camera);
        lightning.check_hit(slimes);
    }
}

void Magic_Staff::draw(vulvox::Renderer* renderer) const
{
    renderer->draw_model("staff", "staff", transform.get_matrix());

    for (auto& lightning : active_lightning)
    {
        lightning.draw(renderer);
    }
}

void Magic_Staff::spawn_lightning()
{
    glm::vec2 staff_position = transform.get_position2d();

    //Spawn three lightning storms in a line under the staff
    for (int i = -1; i < 2; i++)
    {
        glm::vec2 lightning_position_2d = staff_position;
        lightning_position_2d.y += (float)i * (Lightning::radius * 2.f);

        float height = terrain->get_height(lightning_position_2d);

        glm::vec3 lightning_position{ lightning_position_2d.x, height, lightning_position_2d.y };

        active_lightning.emplace_back(lightning_position);
    }
}
