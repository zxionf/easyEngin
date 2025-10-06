#pragma once
#include <string>
#include <unordered_map>
#include <GL/glew.h>
#include <glm/glm.hpp>

class Shader {
public:
    Shader();
    ~Shader();
    
    void loadFromFile(const std::string& vertexPath, const std::string& fragmentPath);
    void loadFromSource(const std::string& vertexSource, const std::string& fragmentSource);
    
    void use() const;
    void unuse() const;
    
    // Uniform…Ë÷√∑Ω∑®
    void setUniform(const std::string& name, bool value) const;
    void setUniform(const std::string& name, int value) const;
    void setUniform(const std::string& name, float value) const;
    void setUniform(const std::string& name, const glm::vec2& value) const;
    void setUniform(const std::string& name, const glm::vec3& value) const;
    void setUniform(const std::string& name, const glm::vec4& value) const;
    void setUniform(const std::string& name, const glm::mat3& value) const;
    void setUniform(const std::string& name, const glm::mat4& value) const;
    
private:
    GLuint compileShader(GLenum type, const std::string& source);
    GLint getUniformLocation(const std::string& name) const;
    
    GLuint programID;
    mutable std::unordered_map<std::string, GLint> uniformLocationCache;
};