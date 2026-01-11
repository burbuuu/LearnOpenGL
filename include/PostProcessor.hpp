#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "engine/Texture.hpp"
#include "engine/Shader.hpp"

class PostProcessor
{
public:
    // State 
    Shader PostProcessingShader;
    Texture2D Texture;
    unsigned int Width, Height;

    // Options
    bool Confuse, Chaos, Shake;

    // Constructor
    PostProcessor(Shader shader, unsigned int width, unsigned int height);

    // Render methods
    void BeginRender();         // prepares the postprocessor's framebuffer operations before rendering the game
    void EndRender();           // prepares the postprocessor's framebuffer operations before rendering the game
    void Render(float time);    // renders the PostProcessor texture quad (as a screen-encompassing large sprite)

private:
    // Render state
    unsigned int MSFBO, FBO; // MSFBO = Multisampled FBO. FBO is regular, used for blitting MS color-buffer to texture
    unsigned int RBO;        // RBO is used for multisampled color buffer
    unsigned int VAO;

    // Initialize quad rendering postprocessing texture
    void initRenderData();
};