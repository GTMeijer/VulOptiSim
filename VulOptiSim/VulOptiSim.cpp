#include "pch.h"
#include "scene.h"

// -----------------------------------------------------------
// When we reach max_frames print the duration and speedup multiplier
// Updating REF_PERFORMANCE at the top of this file with the value
// on your machine gives you an idea of the speedup your optimizations give
// -----------------------------------------------------------
void measure_performance(const vulvox::Renderer& renderer, const Scene& scene, float delta_time, std::array<float, 60>& frames, int& frame_count, bool& lock_update, const std::chrono::steady_clock::time_point& start_time)
{
    const int max_frames = 6000;

    static float duration = 0;

    float REF_PERFORMANCE = 51069.324f;

    if (frame_count >= max_frames)
    {
        if (!lock_update)
        {
            auto elapsed_time = std::chrono::high_resolution_clock::now() - start_time;
            auto duration_us = std::chrono::duration_cast<std::chrono::microseconds>(elapsed_time);
            duration = static_cast<float>(duration_us.count()) / 1000.0f;

            lock_update = true;

            // Write statistics to a file
            std::ofstream log_file("performance_log.txt", std::ios::trunc); // Overwrite mode
            if (log_file.is_open())
            {
                log_file << "Elapsed Time(ms): " << duration << "\n";
                log_file << "Speedup Multiplier: " << REF_PERFORMANCE / duration << "\n";
                log_file << "FPS: " << ImGui::GetIO().Framerate << "\n";
                log_file << "Frame count: " << frame_count << "\n";
                log_file << "Characters in Scene: " << scene.get_character_count() << "\n";
                log_file << "Staff Count in Scene: " << scene.get_staff_count() << "\n";
                log_file.close();
            }
        }

        frame_count--;
    }

    ImGui::Begin("Performance");
    ImGui::Text("Frame: %i", frame_count);
    ImGui::Text("Frame delta: %f", delta_time);
    ImGui::Text("fps: %f", ImGui::GetIO().Framerate);
    ImGui::PlotLines("##FPS Plot", &frames.data()[0], (int)frames.size(), 0, nullptr, 0.0f, std::ranges::max(frames), ImVec2(300, 100));
    //ImGui::Text(renderer.get_memory_statistics().c_str());
    ImGui::End();

    if (lock_update)
    {
        int ms = (int)duration % 1000;
        int sec = ((int)duration / 1000) % 60;
        int min = ((int)duration / 60000);

        std::stringstream ss;
        ss << std::fixed << std::setprecision(3) << duration;
        std::string ref_performance_str = ss.str();

        // Centering the window
        const ImGuiIO& io = ImGui::GetIO();
        ImVec2 window_size(300, 150);
        ImVec2 window_pos((io.DisplaySize.x - window_size.x) * 0.5f, (io.DisplaySize.y - window_size.y) * 0.4f);
        ImGui::SetNextWindowPos(window_pos, ImGuiCond_Once);
        ImGui::SetNextWindowSize(window_size);

        // Custom Styling
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 10.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 2.0f);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 0.85f));
        ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.3f, 0.3f, 0.8f, 1.0f));

        // Window Content
        ImGui::Begin("Performance Score");

        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Elapsed Time:");
        ImGui::Text("%02i:%02i:%03i", min, sec, ms);
        ImGui::Separator();

        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Speedup: %.1f", REF_PERFORMANCE / duration);
        ImGui::Separator();

        ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.5f, 1.0f), "New REF_PERFORMANCE:");
        ImGui::InputText("##refperf", ref_performance_str.data(), sizeof(ref_performance_str), ImGuiInputTextFlags_ReadOnly);

        // Countdown before closing the program
        static auto countdown_start = std::chrono::high_resolution_clock::now();
        auto countdown_elapsed = std::chrono::high_resolution_clock::now() - countdown_start;
        long long countdown_seconds = (long long)10 - std::chrono::duration_cast<std::chrono::seconds>(countdown_elapsed).count();

        if (countdown_seconds > 0)
        {
            ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "Closing in %d seconds...", countdown_seconds);
        }
        else
        {
            // Exit application
            std::exit(0); // Standard way to terminate the program
        }

        ImGui::End();

        // Restore styling
        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor(2);


    }
}

int main()
{
    constexpr uint32_t width = 1024;
    constexpr uint32_t height = 720;

    //try
    {
        vulvox::Renderer renderer;

        auto start_time = std::chrono::high_resolution_clock::now();

        renderer.init(width, height, glm::radians(45.0f), 0.1f, 1000.0f);
        renderer.init_imgui();

        renderer.set_dark_theme();
        //renderer.set_light_theme(); //Bleh

        Scene scene(renderer);

        //Track last 60 frame timings
        std::array<float, 60> frames;
        int current_frame = 0;
        auto previous_frame_time = std::chrono::high_resolution_clock::now();

        bool lock_update = false;

        while (!renderer.should_close())
        {
            auto frame_time = std::chrono::high_resolution_clock::now();
            float delta_time = std::chrono::duration<float, std::chrono::seconds::period>(frame_time - previous_frame_time).count();
            previous_frame_time = frame_time;

            frames[current_frame % 60] = ImGui::GetIO().Framerate;

            //Required to update input state
            glfwPollEvents();

            if (!lock_update)
            {
                //Fixed timestep
                scene.update(1.f / 60.f);
            }

            //Only call draw and imgui functions in between start and end draw
            renderer.start_draw();
            scene.draw();
            measure_performance(renderer, scene, delta_time, frames, current_frame, lock_update, start_time);
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