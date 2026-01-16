#include "pch.h"
#include "log.h"

std::unique_ptr<Log> Log::log_instance = nullptr;

Log::Log()
{
    auto_scroll = true;
    clear();
}

Log* Log::get_instance()
{
    if (!log_instance)
    {
        log_instance = std::unique_ptr<Log>(new Log());
    }

    return log_instance.get();
}

void Log::clear()
{
    text_buffer.clear();
    line_offsets.clear();
    line_offsets.push_back(0);
}

void Log::add_log(const char* fmt, ...) IM_FMTARGS(2)
{
    int old_size = text_buffer.size();
    va_list args;
    va_start(args, fmt);
    text_buffer.appendfv(fmt, args);
    va_end(args);
    for (int new_size = text_buffer.size(); old_size < new_size; old_size++)
    {
        if (text_buffer[old_size] == '\n')
        {
            line_offsets.push_back(old_size + 1);
        }
    }
}

void Log::draw(const char* title, bool* p_open)
{
    if (!ImGui::Begin(title, p_open))
    {
        ImGui::End();
        return;
    }

    // Options menu
    if (ImGui::BeginPopup("Options"))
    {
        ImGui::Checkbox("Auto-scroll", &auto_scroll);
        ImGui::EndPopup();
    }

    // Main window
    if (ImGui::Button("Options"))
    {
        ImGui::OpenPopup("Options");
    }

    ImGui::SameLine();
    bool should_clear = ImGui::Button("Clear");
    ImGui::SameLine();
    bool should_copy = ImGui::Button("Copy");
    ImGui::SameLine();
    text_filter.Draw("Filter", -100.0f);

    ImGui::Separator();

    if (ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar))
    {
        if (should_clear)
        {
            clear();
        }

        if (should_copy)
        {
            ImGui::LogToClipboard();
        }

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        const char* buf = text_buffer.begin();
        const char* buf_end = text_buffer.end();

        if (text_filter.IsActive())
        {
            for (int line_no = 0; line_no < line_offsets.Size; line_no++)
            {
                const char* line_start = buf + line_offsets[line_no];
                const char* line_end = (line_no + 1 < line_offsets.Size) ? (buf + line_offsets[line_no + 1] - 1) : buf_end;
                if (text_filter.PassFilter(line_start, line_end))
                    ImGui::TextUnformatted(line_start, line_end);
            }
        }
        else
        {
            ImGuiListClipper clipper;
            clipper.Begin(line_offsets.Size);
            while (clipper.Step())
            {
                for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
                {
                    const char* line_start = buf + line_offsets[line_no];
                    const char* line_end = (line_no + 1 < line_offsets.Size) ? (buf + line_offsets[line_no + 1] - 1) : buf_end;
                    ImGui::TextUnformatted(line_start, line_end);
                }
            }
            clipper.End();
        }

        ImGui::PopStyleVar();

        // Keep up at the bottom of the scroll region if we were already at the bottom at the beginning of the frame.
        // Using a scrollbar or mouse-wheel will take away from the bottom edge.
        if (auto_scroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        {
            ImGui::SetScrollHereY(1.0f);
        }
    }

    ImGui::EndChild();
    ImGui::End();
}