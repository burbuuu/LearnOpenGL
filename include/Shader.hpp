#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

class Shader
{
public:
    // Constructor reads and builds the shader
    Shader(const char *vertexPath, const char *fragmentPath);
    void cleanUp();

    // Use/Activate the shader
    void use();

    // Utility uniform functions
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;

private:
    // Program
    unsigned int m_id;
};
