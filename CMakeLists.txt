cmake_minimum_required(VERSION 3.20)

project(LearnOpenGL LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_CXX_EXTENSIONS OFF)

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

# ---------------------
# ---- Executable ---- 

add_executable(${PROJECT_NAME}
    src/hellowindow.cpp
    src/Shader.cpp
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        glfw
        glad
)

# -----------------------------
# ---- Resource managment -----

# Source and runtime resource directories
set(RESOURCE_SRC_DIR "${CMAKE_SOURCE_DIR}/resources")
set(RESOURCE_DST_DIR "$<TARGET_FILE_DIR:${PROJECT_NAME}>/resources")

# Stamp file for incremental copying
set(RESOURCE_STAMP "${CMAKE_BINARY_DIR}/resources.stamp")

# Track all resource files
file(GLOB_RECURSE RESOURCE_FILES
    "${RESOURCE_SRC_DIR}/*"
)

# Copy resources when they change
add_custom_command(
    OUTPUT ${RESOURCE_STAMP}
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${RESOURCE_SRC_DIR}
            ${RESOURCE_DST_DIR}
    COMMAND ${CMAKE_COMMAND} -E touch ${RESOURCE_STAMP}
    DEPENDS ${RESOURCE_FILES}
    COMMENT "Updating resources..."
    VERBATIM
)

# Resource update target
add_custom_target(${PROJECT_NAME}_resources ALL
    DEPENDS ${RESOURCE_STAMP}
)

# Ensure resources are ready before the executable
add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}_resources)

# -------------------
# ---- Warnings -----
if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()
