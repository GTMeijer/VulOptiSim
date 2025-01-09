#include "pch.h"
#include "sprite_animation.h"

Sprite_Animation::Sprite_Animation(const std::string& texture_name, int start_frame, int end_frame, float frame_time)
    : texture_name(texture_name), start_frame(start_frame), end_frame(end_frame), frame_time(frame_time)
{
}

void Sprite_Animation::update(const float delta_time)
{
    current_frame_time += delta_time; //accumulate delta time

    //check if it's time to switch to the next frame
    if (current_frame_time >= frame_time)
    {
        current_frame_time -= frame_time; //subtract the frame time (handles overflows)

        //move to the next frame, loop if needed
        current_frame = (current_frame + 1);

        if (current_frame > end_frame)
        {
            current_frame = start_frame;
        }
    }
}

glm::uint32 Sprite_Animation::get_current_frame() const
{
    return current_frame;
}
