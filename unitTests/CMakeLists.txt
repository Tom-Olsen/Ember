# Ember/unitTests/CMakeLists.txt
cmake_minimum_required(VERSION 3.30)
project(EmberAllTests)
enable_testing()

# Cache directories:
get_filename_component(ENGINE_DIR "${CMAKE_CURRENT_LIST_DIR}/../engine" ABSOLUTE)   # engine
get_filename_component(ENGINE_BACKENDS_DIR "${ENGINE_DIR}/backends" ABSOLUTE)       # backends
get_filename_component(ENGINE_LIBS_DIR "${ENGINE_DIR}/libs" ABSOLUTE)               # libs
get_filename_component(ENGINE_EXTERN_DIR "${ENGINE_DIR}/extern" ABSOLUTE)           # libs

# Pass googletest to subprojects:
add_subdirectory("${ENGINE_EXTERN_DIR}/googletest" googletest_build)
set(USE_EXTERNAL_GTEST ON CACHE BOOL "Use externally provided googletest" FORCE)



# ------------------ Test Backends ------------------
add_subdirectory(${ENGINE_BACKENDS_DIR}/vulkanRenderer/unitTests vulkanRendererTests)
# ---------------------------------------------------



# ------------------ Test Libraries -----------------
add_subdirectory(${ENGINE_LIBS_DIR}/math/unitTests mathTests)
# ---------------------------------------------------