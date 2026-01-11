/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Application.hpp"
#include "Screens/Logo.hpp"
#include "engine/ResourceManager.hpp"

#include <iostream>

// GLFW function declarations
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// The Width of the screen
const unsigned int SCREEN_WIDTH = 800;
// The height of the screen
const unsigned int SCREEN_HEIGHT = 600;

// No globals: we'll store the active screen pointer in the GLFW window user pointer.

// Application will be created inside main to control lifetime relative to GLFW

int main(int argc, char *argv[])
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_RESIZABLE, false);

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Breakout", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // OpenGL configuration
    // --------------------
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // initialize application and first screen inside a scope so App
    // is destroyed before we call glfwTerminate (so GL context is valid during destruction)
    // -------------------------------------------------------------------------------
    {
        Application App(SCREEN_WIDTH, SCREEN_HEIGHT);
        App.Init();
        auto logo = std::make_unique<Logo>(&App, SCREEN_WIDTH, SCREEN_HEIGHT);
        App.SetScreen(std::move(logo));
        glfwSetWindowUserPointer(window, &App);

        // main loop
        // ---------
        float deltaTime = 0.0f;
        float lastFrame = 0.0f;

        while (!glfwWindowShouldClose(window))
        {
            float currentFrame = glfwGetTime();
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;
            glfwPollEvents();

            // manage user input, update and render via Application
            App.Update(deltaTime);
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            App.Render();

            glfwSwapBuffers(window);
        }

        // delete all resources as loaded using the resource manager
        ResourceManager::Clear();
    }
    glfwTerminate();
    return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    // Escape -> Close
    // when a user presses the escape key, we set the WindowShouldClose property to true, closing the application
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    if (key >= 0 && key < 1024 && app)
    {
        if (action == GLFW_PRESS)
        {
            app->GetContext().input->Keys[key] = true;
            app->GetContext().input->KeysProcessed[key] = false; // For single press actions this is needed

        }
        else if (action == GLFW_RELEASE)
        {
            app->GetContext().input->Keys[key] = false;
            app->GetContext().input->KeysProcessed[key] = false; // Reset the process status
        }
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}