#pragma once

#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <filesystem>

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

#include <stb_image.h>

#include <renderer.h>

#include "camera.h"
#include "terrain.h"


const std::string TERRAIN_PATH = "../models/rocky_peaks_terrain_heightmap.png";
const std::string TERRAIN_MESH_PATH = "../models/terrain.obj";

const std::string MODEL_PATH = "../models/konata.obj";
const std::string KONATA_MODAL_TEXTURE_PATH = "../textures/konata_texture.png";
const std::string KONATA_TEXTURE_PATH = "../textures/konata.png";

const std::string FRIEREN_BLOB_PATH = "../models/frieren_blob.obj";
const std::string FRIEREN_BLOB_TEXTURE_PATH = "../textures/frieren-blob-texture.png";

const std::string CUBE_MODEL_PATH = "../models/cube-tex.obj";
const std::string CUBE_SEA_TEXTURE_PATH = "../textures/cube-tex-sea.png";
const std::string CUBE_GRASS_TEXTURE_PATH = "../textures/cube-tex-grass.png";
const std::string CUBE_MOUNTAIN_TEXTURE_PATH = "../textures/cube-tex-mountain.png";