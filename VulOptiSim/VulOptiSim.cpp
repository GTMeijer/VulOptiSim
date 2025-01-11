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
        renderer.init_imgui();

        renderer.set_dark_theme();
        //renderer.set_light_theme(); //Bleh

        Scene scene(renderer);

        std::array<float, 60> frames;

        int current_frame = 0;

        auto previous_frame_time = std::chrono::high_resolution_clock::now();
        while (!renderer.should_close())
        {
            auto frame_time = std::chrono::high_resolution_clock::now();
            float delta_time = std::chrono::duration<float, std::chrono::seconds::period>(frame_time - previous_frame_time).count();
            previous_frame_time = frame_time;

            frames[current_frame % 60] = ImGui::GetIO().Framerate;

            //Required to update input state
            glfwPollEvents();

            renderer.set_imgui_callback(
                [&renderer, &frames]()
                {
                    ImGui::Begin("Performance");
                    ImGui::Text("fps: %f", ImGui::GetIO().Framerate);
                    ImGui::PlotLines("##FPS Plot", &frames.data()[0], (int)frames.size(), 0, nullptr, 0.0f, std::ranges::max(frames), ImVec2(300, 100));
                    ImGui::Text(renderer.get_memory_statistics().c_str());
                    ImGui::End();
                });

#ifdef variable_time_step
            //Variable time step
            scene.update(delta_time);
#else
            //Fixed timestep
            scene.update(1.f / 60.f);
#endif // variable_time_step

            //Only call draw and imgui functions in between start and end draw
            renderer.start_draw();
            scene.draw();
            renderer.end_draw();

            current_frame++;
        }

        renderer.destroy();
    }
    //catch (const std::exception& ex)
    //{
    //    std::cout << ex.what() << std::endl;
    //}
}