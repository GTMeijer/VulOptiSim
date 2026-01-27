# VulOptiSim

How to compile the project:

##### Table of Contents
[Windows](#windows)  
[MacOS](#macos)  
[Linux](#linux)

# Windows

## Visual Studio 2022
By far the easiest way to get the project running is Visual Studio.
The project comes with a preconfigured solution (.sln) file for Visual Studio, just open that and it should work.

## Other IDEs
If you're using another IDE you need to configure the project yourself, how this works differs per IDE.


Set the C++ Standard to C++20
Also, make sure you have a Release build profile that has optimization set to at least -O2.

The project directory comes with the required libraries.
The following include directories contain the referenced header files:

```
includes\glfw-3.4\WIN64\include
includes\glm
includes\VulVoxRenderer\include
includes\stb-image
```

You also need to reference the following library files:

```
glfw3.lib
VulVoxOptimizationProject.lib
```

These are located in the following lib folders:

```
includes\glfw-3.4\WIN64\lib-vc2022
includes\VulVoxRenderer\lib\[Debug or Release]
```
Make sure you are linking the correct version of VulVoxOptimizationProject.lib based on whether you're building Debug or Release.

Again, Visual Studio is the easiest option to get started quickly. We can try and help with other IDEs but we'd rather not spend too much time on it.

## Error: "The procedure entry point vkGetDeviceImageMemoryRequirements could not be located"

If you're getting the above error it probably means your GPU driver doesn't support Vulkan 1.3. 
First try updating your GPU driver. If that doesn't work I've included compatibility versions of the renderer lib.
Simply overwrite the libs in include/VulVoxRenderer/lib release and debug with their corresponding compat versions.

# MacOS

For those of you are working on MacOS, the project was also tested on this platform. 

Students from the 2024/2025 course (thanks Max & Niels!) got it working with the following steps:

## Requirements
- [MacOS Homebrew](https://brew.sh)
- [CMake](https://formulae.brew.sh/formula/cmake#default)
- [Vulkan SDK](https://vulkan.lunarg.com/sdk/home#mac)

## Install the Vulkan SDK
Download and install the Vulkan SDK from the [LunarG website](https://vulkan.lunarg.com/).
The MacOS version uses MoltenVK internally, which supports up to Vulkan 1.2. This project only uses Vulkan 1.0 features so that shouldn't be a problem.

> *Make sure to note which VulkanSDK version you installed!*
> *If using default installation and location is in home:*
```bash
sudo python3 ~/VulkanSDK/[VERSION]/install_vulkan.py
```

> ***Example for Vulkan 1.3.296.0***
```bash
sudo python3 ~/VulkanSDK/1.3.296.0/install_vulkan.py
```

This ensures Vulkan is properly installed.

## VulVox Renderer
The next step is to download and replace the correct renderers with the current ones in the project.

The compiled libraries can be found on the [renderers release page](https://github.com/GTMeijer/VulVoxOptimizationProject/releases).
Check which architecture your device is running on and download the correct lib files for you architecture.

To place these renderers in the project, extract the downloaded zip files and put them in the correct folder in the project.

>***Be careful to place the libraries in the right folders!***

>*Debug*
```bash
/includes/VulVoxRenderer/lib/Debug/libVulVoxOptimizationProject.a
```
>*Release*
```bash
/includes/VulVoxRenderer/lib/Release/libVulVoxOptimizationProject.a
```

## CMake for MacOS

To configure the project, follow these steps.

Create a `CMakeLists.txt` file with the content below.

>Replace `[USER]` in `include_directories` and `link_directories` with your system name  
>(for example, `/User/johndoe/`).
>
>*Make sure to note which Vulkan version you are using! The CMakeLists file below expects **1.3.296.0**. If you're using a newer (or older) version, update this in the file accordingly.*
```cmake
cmake_minimum_required(VERSION 3.20)
project(VulOptiSim)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set architecture to x86_64
# set(CMAKE_OSX_ARCHITECTURES x86_64)

# Define source files
file(GLOB_RECURSE SRC_FILES "./VulOptiSim/*.cpp")

# Include directories
include_directories(
        ${CMAKE_SOURCE_DIR}/includes/glm
        ${CMAKE_SOURCE_DIR}/includes/VulVoxRenderer/include
        ${CMAKE_SOURCE_DIR}/includes/stb-image
        ${CMAKE_SOURCE_DIR}/includes/glfw-3.4/MACOS/include
        /Users/[USER]/VulkanSDK/1.3.296.0/macOS/include
)

# Link directories
link_directories(
        ${CMAKE_SOURCE_DIR}/includes/glfw-3.4/MACOS/lib-universal
        ${CMAKE_SOURCE_DIR}/includes/VulVoxRenderer/lib/${CMAKE_BUILD_TYPE}
        /Users/[USER]/VulkanSDK/1.3.296.0/macOS/lib
)

# Add the executable
add_executable(vuloptisim ${SRC_FILES})

# Link libraries
target_link_libraries(vuloptisim
        ${CMAKE_SOURCE_DIR}/includes/VulVoxRenderer/lib/${CMAKE_BUILD_TYPE}/libVulVoxOptimizationProject.a
        glfw3
        vulkan
        "-framework Cocoa"
        "-framework OpenGL"
        "-framework IOKit"
        "-framework CoreVideo"
)

# Set optimization flags and library paths
if(CMAKE_BUILD_TYPE MATCHES Release)
    target_compile_options(vuloptisim PRIVATE -O3)
elseif(CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_options(vuloptisim PRIVATE -O0)
endif()
```

Place the `CMakeLists.txt` in the project root.

## IDE Config

Open the project in your IDE (CLion for example) and select the CMakeLists.txt in your project root folder.
Make sure both the Debug and Release profiles are added, use debug for development work and release for profiling!

### CLion specific

- Open the project folder
- Click the warning icon at the bottom and click "Select CMakeLists.txt"
- Select the CMakelists.txt file in your project root
- CLion adds the debug profile by default, in the 'Edit CMake Profiles' window you can add the release profile using the + icon.
---

## To build the project with clang

The following scripts build the project with the right dependencies, makes sure to change the [USERNAME], and vulkan SDK version number if needed.
Note: Add -O0 for a debug build and -O2 or -O3 for an optimized build, use the latter for profiling and checking your speedup!

Release build:
```
rm -rf build

mkdir -p build

clang++ \
    -std=c++20 \
    -I./includes/glm \
    -I./includes/VulVoxRenderer/include \
    -I./includes/stb-image \
    -I./includes/glfw-3.4/MACOS/include \
    -I/Users/[USERNAME]/VulkanSDK/1.3.296.0/macOS/include \
    -L./includes/glfw-3.4/MACOS/lib-universal \
    -L.includes/VulVoxRenderer/lib/Release \
    -L/Users/[USERNAME]/VulkanSDK/1.3.296.0/macOS/lib \
    -lvulvox \
    -lglfw3 \
    -lvulkan \
    -framework Cocoa \
    -framework OpenGL \
    -framework IOKit \
    -framework CoreVideo \
    -o build/vuloptisim \
    $(find ./VulOptiSim -name '*.cpp') \
    -O2 \

```

Debug build:
```
rm -rf build

mkdir -p build

clang++ \
    -std=c++20 \
    -I./includes/glm \
    -I./includes/VulVoxRenderer/include \
    -I./includes/stb-image \
    -I./includes/glfw-3.4/MACOS/include \
    -I/Users/[USERNAME]/VulkanSDK/1.3.296.0/macOS/include \
    -L./includes/glfw-3.4/MACOS/lib-universal \
    -L.includes/VulVoxRenderer/lib/Debug \
    -L/Users/[USERNAME]/VulkanSDK/1.3.296.0/macOS/lib \
    -lvulvox \
    -lglfw3 \
    -lvulkan \
    -framework Cocoa \
    -framework OpenGL \
    -framework IOKit \
    -framework CoreVideo \
    -o build/vuloptisim \
    $(find ./VulOptiSim -name '*.cpp') \
    -O0 \

```

# Linux

If you're a Linux warrior, you have to build the renderer yourself and set the right configuration to get the project working.
This is considered advanced, so we won't spend much time on getting this working.
You can find the engine project in the following repo: https://github.com/GTMeijer/VulVoxOptimizationProject

Good luck with the project :)
