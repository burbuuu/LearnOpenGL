#include <Shader.hpp>
#include <fstream>
#include <sstream>
#include <iostream>

// Constructor
Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
    // 1. retrieve the vertex /fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    // Ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try 
    {
        // Open files
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        // Read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // close file handlers
        vShaderFile.close();
        fShaderFile.close();
        // convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    } 
    catch (const std::ifstream::failure& e) 
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        m_id = 0;
        return;
    }

    // Guard against empty files
    if (vertexCode.empty() || fragmentCode.empty())
    {
        std::cerr << "ERROR::SHADER::EMPTY_SOURCE\n";
        m_id = 0;
        return;
    }

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    // 2. Compile the shaders
    unsigned int vertex, fragment;
    int succes;
    char infoLog[512];

    // Vertex Shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    // Print compile errors if any
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &succes);
    if(!succes)
    {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Fragment shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    // Print compile errors if any
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &succes);
    if(!succes)
    {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // 3. Shader program
    m_id = glCreateProgram();
    glAttachShader(m_id, vertex);
    glAttachShader(m_id, fragment);
    glLinkProgram(m_id);
    // Print linking errors if any
    glGetProgramiv(m_id, GL_LINK_STATUS, &succes);
    if(!succes)
    {
        glGetProgramInfoLog(m_id, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // 4. Delete shaders. 
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader::cleanUp()
{

    if (m_id != 0)
    {
        glDeleteProgram(m_id);
        m_id = 0; // Prevent double deletion if context is still active later
    }
}

void Shader::use()
{
    if (m_id == 0) return;

    glUseProgram(m_id);
}

void Shader::setBool(const std::string& name, bool value) const
{
    if (m_id == 0) return;
    glUniform1i(glGetUniformLocation(m_id, name.c_str()), (int) value);
}

void Shader::setInt(const std::string& name, int value) const
{
    if (m_id == 0) return;
    glUniform1i(glGetUniformLocation(m_id, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const
{
    if (m_id == 0) return;
    glUniform1f(glGetUniformLocation(m_id, name.c_str()), value);
}