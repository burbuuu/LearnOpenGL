cmake_minimum_required(VERSION 3.20)

project(Breakout LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17) # this does nothing for MSVC, use target_compile_options below
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)


# Prefer user-selected build type; default to Debug for single-config generators
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build." FORCE)
endif()
message(STATUS "CMake build type: ${CMAKE_BUILD_TYPE}")

# --------------
# ---- GLFW ----

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(external/glfw)

# ----------------
# ---- OpenGL ----

find_package(OpenGL REQUIRED)

# --------------
# ---- GLAD ----

add_library(glad STATIC
    external/glad/src/glad.c
)

target_include_directories(glad
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/external/glad/include
)

target_link_libraries(glad
    PUBLIC
        OpenGL::OpenGL
)

# -------------------
# ----- GLM ---------
set(GLM_TEST_ENABLE OFF CACHE BOOL "" FORCE)
add_subdirectory(external/glm)

# ---------------------
# ---- STB ----
# Header-only interface
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE external/stb)

# Single implementation TU
add_library(stb_impl STATIC src/engine/stb_image.cpp)
target_link_libraries(stb_impl PUBLIC stb)

# ---------------------
# ---- Executable ---- 

add_executable(${PROJECT_NAME}
    src/engine/Game.cpp
    src/engine/ResourceManager.cpp
    src/engine/Shader.cpp
    src/engine/Texture.cpp
    src/main.cpp
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        glfw
        glad
        glm::glm
        stb_impl
)

# -------------------
# ---- Output dirs ---
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin/Debug
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin/Release
)


# -----------------------------
# ---- Resource management ----

set(RESOURCE_SRC_DIR "${CMAKE_SOURCE_DIR}/resources")
set(RESOURCE_DST_DIR "$<TARGET_FILE_DIR:${PROJECT_NAME}>/resources")
set(RESOURCE_STAMP "${CMAKE_BINARY_DIR}/resources.stamp")

file(GLOB_RECURSE RESOURCE_FILES
    CONFIGURE_DEPENDS
    "${RESOURCE_SRC_DIR}/*"
)

add_custom_command(
    OUTPUT ${RESOURCE_STAMP}
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${RESOURCE_SRC_DIR}
            ${RESOURCE_DST_DIR}
    COMMAND ${CMAKE_COMMAND} -E touch ${RESOURCE_STAMP}
    DEPENDS ${RESOURCE_FILES}
    COMMENT "Updating resources..."
)

add_custom_target(resources ALL
    DEPENDS ${RESOURCE_STAMP}
)

add_dependencies(${PROJECT_NAME} resources)

# -------------------
# ---- Warnings -----
if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()