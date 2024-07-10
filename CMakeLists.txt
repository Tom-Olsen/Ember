cmake_minimum_required(VERSION 3.10)

# Project name:
project(MyEngine)

# C++ standard:
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output directory:
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")

# Source & header files:
file(GLOB SOURCE_FILES "${PROJECT_SOURCE_DIR}/src/*.cpp")
file(GLOB HEADER_FILES "${PROJECT_SOURCE_DIR}/src/*.h")

# Required libraries:
add_subdirectory(libs/SDL)      # SDL submodule
add_subdirectory(libs/glm)      # glm submodule
add_subdirectory(libs/spdlog)   # spdlog submodule
find_package(Vulkan REQUIRED)   # required to be installed

# # Shader Language (pick one):
# set(SHADER_LANGUAGE "GLSL") # HLSL or GLSL
# 
# # Compile shaders:
# if(UNIX)
#     add_custom_target(build_shaders ALL
#         COMMAND "{PROJECT_SOURCE_DIR}/shaders/compile${SHADER_LANGUAGE}.sh"
#         WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/shaders")
# endif(UNIX)
# 
# if(WIN32)
#     add_custom_target(build_shaders ALL
#         COMMAND "${PROJECT_SOURCE_DIR}/shaders/compile${SHADER_LANGUAGE}.bat"
#         WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/shaders")
# endif(WIN32)

# Configure MyEngine executable:
add_executable(MyEngine ${SOURCE_FILES} ${HEADER_FILES})
# add_dependencies(MyEngine build_shaders)

# Add headers:
target_include_directories(MyEngine PUBLIC libs/SDL/include)
target_include_directories(MyEngine PUBLIC libs/glm)
target_include_directories(MyEngine PUBLIC libs/spdlog/include)
target_include_directories(MyEngine PUBLIC ${Vulkan_INCLUDE_DIRS})

# Add dlls:
target_link_libraries(MyEngine PUBLIC SDL3::SDL3-shared)
target_link_libraries(MyEngine PUBLIC ${Vulkan_LIBRARIES})