#pragma once

#include <iostream>
#include <vector>
#include <array>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <filesystem>
#include <ranges>
#include <algorithm>

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

#include "utils.h"

#include "camera.h"

#include "transform.h"
#include "terrain.h"
#include "slime.h"
#include "shield.h"
#include "lightning.h"

const std::filesystem::path TERRAIN_PATH = "../models/map.png";
const std::filesystem::path TERRAIN_MESH_PATH = "../models/terrain.obj";

const std::filesystem::path MODEL_PATH = "../models/konata.obj";
const std::filesystem::path KONATA_MODAL_TEXTURE_PATH = "../textures/konata_texture.png";
const std::filesystem::path KONATA_TEXTURE_PATH = "../textures/konata.png";

const std::filesystem::path FRIEREN_BLOB_PATH = "../models/frieren_blob.obj";
const std::filesystem::path FRIEREN_BLOB_TEXTURE_PATH = "../textures/frieren-blob-texture.png";
const std::filesystem::path SHIELD_TEXTURE_PATH = "../textures/shield.png";
const std::filesystem::path LIGHTNING_TEXTURE_PATH = "../textures/lightning.png";

const std::filesystem::path STAFF_PATH = "../models/staff.obj";
const std::filesystem::path STAFF_TEXTURE_PATH = "../textures/staff.png";

const std::filesystem::path CUBE_MODEL_PATH = "../models/cube-tex.obj";
const std::filesystem::path CUBE_SEA_TEXTURE_PATH = "../textures/cube-tex-sea.png";
const std::filesystem::path CUBE_GRASS_TEXTURE_PATH = "../textures/cube-tex-grass.png";
const std::filesystem::path CUBE_MOUNTAIN_TEXTURE_PATH = "../textures/cube-tex-mountain.png";

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