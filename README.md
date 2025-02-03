# VulOptiSim

How to compile the project:

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

These are places in the following lib folders (make sure to use the appropriate configuration (debug/release):

```
includes\glfw-3.4\WIN64\lib-vc2022
includes\VulVoxRenderer\lib\[Debug or Release]
```

Again, Visual Studio is the easiest option to get started quickly. We can try and help with other IDEs but we'd rather not spend too much time on it.

# MacOS

For those of you are working on MacOS, the project was also tested on this platform.
We will help you get it working, it might take a bit more effort the first few times.

## Install the Vulkan SDK
Download and install the Vulkan SDK from the [LungarG website](https://vulkan.lunarg.com/).
The MacOS version uses MoltenVK internally, which supports up to Vulkan 1.2. This project only uses Vulkan 1.0 features so that shouldn't be a problem.

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

Good luck :)
