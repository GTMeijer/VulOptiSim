#pragma once

//Based on the simplest logger example in imgui: https://github.com/ocornut/imgui/blob/e9743d85ddc0986c6babaa01fd9e641a47b282f3/imgui_demo.cpp#L6523-L6884

//Singleton log class, it's ugly but it works..
class Log
{

public:

    static Log* get_instance();

    void clear();

    void add_log(const char* fmt, ...) IM_FMTARGS(2);

    void draw(const char* title, bool* p_open = NULL);

    //Log(Log& other) = delete;
    //void operator=(const Log&) = delete;

private:

    ImGuiTextBuffer     text_buffer;
    ImGuiTextFilter     text_filter;
    ImVector<int>       line_offsets; // Index to lines offset. We maintain this with add_log() calls.
    bool                auto_scroll;  // Keep scrolling if already at the bottom.

protected:

    Log();

    static std::unique_ptr<Log> log_instance;

};