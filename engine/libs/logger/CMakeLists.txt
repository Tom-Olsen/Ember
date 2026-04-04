# Ember/engine/libs/logger/CMakeLists.txt



# ------------------- Project Setup ------------------
# Static (default) vs Shared library option:
set(TARGET_NAME EmberLogger)
option(EMBER_LOGGER_BUILD_SHARED "Build ${TARGET_NAME} as a shared library" OFF)

# Cache directories:
get_filename_component(ENGINE_DIR "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)   # engine
get_filename_component(ENGINE_EXTERN_DIR "${ENGINE_DIR}/extern" ABSOLUTE)       # extern
# ---------------------------------------------------



# ------------------- File Lists --------------------
file(GLOB SRC_FILES "src/*")
source_group("Source Files" FILES ${SRC_FILES})
# ---------------------------------------------------



# ----------------- Create Library ------------------
if (EMBER_LOGGER_BUILD_SHARED)
    add_library(${TARGET_NAME} SHARED ${SRC_FILES})
else()
    add_library(${TARGET_NAME} STATIC ${SRC_FILES})
endif()

# Source subdirectories:
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Enable utf-8 encoding for microsoft visual studio (needed for spdlog):
if (MSVC)
    target_compile_options(${TARGET_NAME} PUBLIC /utf-8)
endif()
# ---------------------------------------------------



# ---------------------- Macros ---------------------
# Library compilation logic:
if (EMBER_LOGGER_BUILD_SHARED)
    target_compile_definitions(${TARGET_NAME} PRIVATE LOGGER_EXPORTS)
else()
    target_compile_definitions(${TARGET_NAME} PUBLIC LOGGER_STATIC)
endif()
# ---------------------------------------------------



# ------------- Link External Libraries -------------
# spdlog (CMakeList target):
target_include_directories(${TARGET_NAME} PUBLIC "${ENGINE_EXTERN_DIR}/spdlog/include")
# ---------------------------------------------------