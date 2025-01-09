#pragma once

//This class is kinda scuffed but it works..

template <typename T>
class Sprite_Manager
{
public:

    Sprite_Manager() = default;

    Sprite_Manager(std::string texture_name) : texture_name(texture_name)
    {
    }

    ~Sprite_Manager() = default;

    void register_draw(const T& sprite_object)
    {
        model_matrices.push_back(sprite_object.get_model_matrix());
        texture_indices.push_back(sprite_object.get_texture_index());

        //I'm just going to assume every sprite renders complete frames
        min_max_uvs.push_back(glm::vec4{ 0.f, 0.f, 1.f, 1.f });
    }

    void draw(vulvox::Renderer* renderer)
    {
        if (model_matrices.empty())
        {
            return;
        }

        renderer->draw_planes(texture_name, model_matrices, texture_indices, min_max_uvs);
    }

    void reset()
    {
        model_matrices.clear();
        texture_indices.clear();
        min_max_uvs.clear();
    }

private:

    std::string texture_name;
    std::vector<glm::mat4> model_matrices;
    std::vector<glm::uint32_t> texture_indices;
    std::vector<glm::vec4> min_max_uvs;
};
