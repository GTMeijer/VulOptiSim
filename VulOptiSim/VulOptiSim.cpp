#include "pch.h"
#include "scene.h"

#define variable_time_step

int main()
{
    constexpr uint32_t width = 1024;
    constexpr uint32_t height = 720;

    //try
    {
        vulvox::Renderer renderer;

        renderer.init(width, height, glm::radians(45.0f), 0.1f, 1000.0f);


        Scene scene(renderer);

        auto previous_frame_time = std::chrono::high_resolution_clock::now();
        while (!renderer.should_close())
        {
            auto frame_time = std::chrono::high_resolution_clock::now();
            float delta_time = std::chrono::duration<float, std::chrono::seconds::period>(frame_time - previous_frame_time).count();
            previous_frame_time = frame_time;

            //Required to update input state
            glfwPollEvents();

#ifdef variable_time_step
            //Variable time step
            scene.update(delta_time);
#else
            //Fixed timestep
            scene.update(1.f / 60.f);
#endif // variable_time_step

            renderer.start_draw();
            scene.draw();
            renderer.end_draw();
        }

        renderer.destroy();
    }
    //catch (const std::exception& ex)
    //{
    //    std::cout << ex.what() << std::endl;
    //}
}