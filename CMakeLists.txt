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
        OpenGL::GL
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

# ------------------
# ---- irrKlang ----

# Define general paths
set(IRRKLANG_INCLUDE_DIR "external/irrKlang/include")
set(IRRKLANG_ROOT_DIR "external/irrKlang")

# Create an IMPORTED target for the irrKlang library
if (WIN32)
    add_library(irrklang STATIC IMPORTED)
else()
    add_library(irrklang SHARED IMPORTED)
endif()


target_include_directories(irrklang
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/${IRRKLANG_INCLUDE_DIR}
)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    # Windows Linker Library (.lib) and Runtime DLL (.dll)
    set(IRRKLANG_LIB_PATH "${IRRKLANG_ROOT_DIR}/lib/Winx64-visualStudio/irrKlang.lib")
    set(IRRKLANG_BIN_DIR "${IRRKLANG_ROOT_DIR}/bin/winx64-visualStudio")
    set(IRRKLANG_RUNTIME_FILES
        irrKlang.dll
        ikpMP3.dll
        ikpFlac.dll
    )
    
    # Set the path to the required linker library file
    set_target_properties(irrklang PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/${IRRKLANG_LIB_PATH}"
    )

elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    # Linux Shared Library (.so)
    # On Linux, the .so file is both the linker library and the runtime binary.
    set(IRRKLANG_LIB_NAME "libIrrKlang.so")
    set(IRRKLANG_BIN_DIR "${IRRKLANG_ROOT_DIR}/bin/linux-gcc-64")
    set(IRRKLANG_LIB_PATH "${IRRKLANG_BIN_DIR}/${IRRKLANG_LIB_NAME}")
    set(IRRKLANG_RUNTIME_FILES
        ${IRRKLANG_LIB_NAME}
        ikpMP3.so
        ikpFlac.so
    )
    
    # Set the path to the shared object file
    set_target_properties(irrklang PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/${IRRKLANG_LIB_PATH}"
        # Setting RPATH is critical on Linux so the executable finds the SO at runtime
    )

else()
    message(FATAL_ERROR "Unsupported platform for irrKlang binaries.")
endif()

# ---------------------
# ---- Executable ---- 

add_executable(${PROJECT_NAME}
    src/engine/ResourceManager.cpp
    src/engine/Shader.cpp
    src/engine/Texture.cpp
    src/engine/SpriteRenderer.cpp
    src/engine/SoundEngine.cpp
    src/Game.cpp
    src/main.cpp
    src/GameLevel.cpp
    src/GameObject.cpp
    src/BallObject.cpp
    src/Collision.cpp
    src/ParticleGenerator.cpp
    src/PostProcessor.cpp
    src/PowerUp.cpp
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
        irrklang
)

# -------------------------------------
# ---- Configuration Definitions ----

target_compile_definitions(${PROJECT_NAME}
    PRIVATE
        # Define NDEBUG in Release builds to disable assertions and enable optimizations
        $<$<CONFIG:Release>:NDEBUG>
        
        # Define DEBUG in Debug builds (useful for custom logging/features)
        $<$<CONFIG:Debug>:DEBUG>
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


# --------------------------------
# ---- irrKlang Binary & Plugin Copy ----
# Copy the DLL/SO and plugins next to the executable at runtime.

# Ensure this step only runs if we successfully defined paths for the current system
if (IRRKLANG_RUNTIME_FILES)
    
    # Define the destination directory for the runtime files (same as executable output)
    # The RUNTIME_OUTPUT_DIRECTORY properties are used implicitly by TARGET_FILE_DIR
    set(IRRKLANG_DST_DIR "$<TARGET_FILE_DIR:${PROJECT_NAME}>")

    foreach(FILE_NAME IN LISTS IRRKLANG_RUNTIME_FILES)
        set(IRRKLANG_SRC_BIN "${CMAKE_CURRENT_SOURCE_DIR}/${IRRKLANG_BIN_DIR}/${FILE_NAME}")
        set(IRRKLANG_DST_BIN "${IRRKLANG_DST_DIR}/${FILE_NAME}")
        
        # Use a POST_BUILD command to execute the copy after the executable is linked
        add_custom_command(
            TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    ${IRRKLANG_SRC_BIN}
                    ${IRRKLANG_DST_BIN}
            COMMENT "Copying irrKlang runtime file: ${FILE_NAME}"
        )
    endforeach()
else()
    message(STATUS "No irrKlang runtime binary copy necessary for this platform.")
endif()


# -------------------
# ---- Warnings -----
if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()