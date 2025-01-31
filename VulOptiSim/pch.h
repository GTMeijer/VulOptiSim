#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <queue>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <filesystem>
#include <numeric>
#include <ranges>
#include <algorithm>
#include <thread>
#include <future>

//GLFW & Vulkan
//#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

//GLM
//Force depth range from 0.0 to 1.0 (Vulkan standard), instead of -1.0 to 1.0
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/hash.hpp>
#include <glm/gtx/norm.hpp>

#include <stb_image.h>

#include <renderer.h>
#include <imgui/imgui.h>

#include "thread_pool.h"

#include "utils.h"

#include "camera.h"

#include "transform.h"
#include "terrain.h"
#include "sprite_animation.h"
#include "sprite_manager.h"
#include "hero.h"
#include "shield.h"
#include "lightning.h"
#include "projectile.h"


const std::filesystem::path TERRAIN_PATH = "../models/map.png";
const std::filesystem::path TERRAIN_MESH_PATH = "../models/terrain.obj";

const std::filesystem::path MODEL_PATH = "../models/konata.obj";
const std::filesystem::path KONATA_MODAL_TEXTURE_PATH = "../textures/konata_texture.png";
const std::filesystem::path KONATA_TEXTURE_PATH = "../textures/konata.png";

const std::filesystem::path FRIEREN_BLOB_PATH = "../models/frieren_blob.obj";
const std::filesystem::path FRIEREN_BLOB_TEXTURE_PATH = "../textures/frieren-blob-texture.png";
const std::filesystem::path FRIEREN_PATH = "../models/Frieren.obj";
const std::filesystem::path FRIEREN_TEXTURE_PATH = "../textures/frieren_party.png";
const std::filesystem::path SHIELD_TEXTURE_PATH = "../textures/shield.png";
const std::filesystem::path LIGHTNING_TEXTURE_PATH = "../textures/lightning.png";

const std::filesystem::path STAFF_PATH = "../models/staff.obj";
const std::filesystem::path STAFF_TEXTURE_PATH = "../textures/staff.png";

const std::filesystem::path CUBE_MODEL_PATH = "../models/cube-tex.obj";
const std::filesystem::path CUBE_SEA_TEXTURE_PATH = "../textures/water.png";
const std::filesystem::path CUBE_GRASS_FLOWER_TEXTURE_PATH = "../textures/grass_flower.png";
const std::filesystem::path CUBE_GRASS_FIELD_TEXTURE_PATH = "../textures/grass_field.png";
const std::filesystem::path CUBE_MOSS_TEXTURE_PATH = "../textures/moss.png";
const std::filesystem::path CUBE_MOUNTAIN_TEXTURE_PATH = "../textures/cube-tex-mountain.png";
const std::filesystem::path CUBE_STONE_WALL_TEXTURE_PATH = "../textures/stone_wall_pattern.png";
const std::filesystem::path CUBE_COBBLE_FLOOR_TEXTURE_PATH = "../textures/cobble_stone.png";
const std::filesystem::path CUBE_CONCRETE_WALL_TEXTURE_PATH = "../textures/concrete_wall.png";
const std::filesystem::path CUBE_BRICK_WALL_TEXTURE_PATH = "../textures/brick_wall.png";

const std::vector<std::filesystem::path> LIGHTNING_TEXTURE_PATHS =
{
    "../textures/lightning/01.png",
    "../textures/lightning/02.png",
    "../textures/lightning/03.png",
    "../textures/lightning/04.png",
    "../textures/lightning/05.png",
    "../textures/lightning/06.png",
    "../textures/lightning/07.png",
    "../textures/lightning/08.png",
    "../textures/lightning/09.png",
    "../textures/lightning/10.png"
};

const std::vector<std::filesystem::path> FIREBALL_TEXTURE_PATHS =
{
    "../textures/blue_fireball/img_0.png",
    "../textures/blue_fireball/img_1.png",
    "../textures/blue_fireball/img_2.png",
    "../textures/blue_fireball/img_3.png",
    "../textures/blue_fireball/img_4.png",
    "../textures/blue_fireball/img_5.png",
    "../textures/blue_fireball/img_6.png",
    "../textures/blue_fireball/img_7.png",
    "../textures/blue_fireball/img_8.png",
    "../textures/blue_fireball/img_9.png",
    "../textures/blue_fireball/img_11.png",
    "../textures/blue_fireball/img_12.png",
    "../textures/blue_fireball/img_13.png",
    "../textures/blue_fireball/img_14.png",
    "../textures/blue_fireball/img_15.png",
    "../textures/blue_fireball/img_16.png",
    "../textures/blue_fireball/img_17.png",
    "../textures/blue_fireball/img_18.png",
    "../textures/blue_fireball/img_19.png",
    "../textures/blue_fireball/img_20.png",
    "../textures/blue_fireball/img_21.png",
    "../textures/blue_fireball/img_22.png",
    "../textures/blue_fireball/img_23.png",
    "../textures/blue_fireball/img_24.png",
    "../textures/blue_fireball/img_25.png",
    "../textures/blue_fireball/img_26.png",
    "../textures/blue_fireball/img_27.png",
    "../textures/blue_fireball/img_28.png",
    "../textures/blue_fireball/img_29.png",
    "../textures/blue_fireball/img_30.png",
    "../textures/blue_fireball/img_31.png",
    "../textures/blue_fireball/img_32.png",
    "../textures/blue_fireball/img_33.png",
    "../textures/blue_fireball/img_34.png",
    "../textures/blue_explosion/img_0.png",
    "../textures/blue_explosion/img_1.png",
    "../textures/blue_explosion/img_2.png",
    "../textures/blue_explosion/img_3.png",
    "../textures/blue_explosion/img_4.png",
    "../textures/blue_explosion/img_5.png",
    "../textures/blue_explosion/img_6.png",
    "../textures/blue_explosion/img_7.png",
    "../textures/blue_explosion/img_8.png",
    "../textures/blue_explosion/img_9.png",
    "../textures/blue_explosion/img_11.png",
    "../textures/blue_explosion/img_12.png",
    "../textures/blue_explosion/img_13.png",
    "../textures/blue_explosion/img_14.png",
    "../textures/blue_explosion/img_15.png",
    "../textures/blue_explosion/img_16.png",
    "../textures/blue_explosion/img_17.png",
    "../textures/blue_explosion/img_18.png",
    "../textures/blue_explosion/img_19.png",
    "../textures/blue_explosion/img_20.png",
    "../textures/blue_explosion/img_21.png",
    "../textures/blue_explosion/img_22.png",
    "../textures/blue_explosion/img_23.png",
    "../textures/blue_explosion/img_24.png",
    "../textures/blue_explosion/img_25.png",
    "../textures/blue_explosion/img_26.png",
    "../textures/blue_explosion/img_27.png",
    "../textures/blue_explosion/img_28.png",
    "../textures/blue_explosion/img_29.png"
};
