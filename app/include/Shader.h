#ifndef DEF_HEADER_SHADER
#define DEF_HEADER_SHADER

#include <glimac/Program.hpp>
#include <glimac/glm.hpp>
#include <glimac/FilePath.hpp>
#include <unordered_map>

class Shader {
public:
    Shader(const char **argv, const char *vertexPath, const char *fragmentPath);

    ~Shader();

    void use();

    void uniformMatrix(std::string key, glm::mat4 & mat);
    void uniformMatrix(std::string key, glm::mat3 & mat);

    void uniformValue(std::string key, GLfloat value);
    void uniformValue(std::string key, GLint value);

    void uniformVector(std::string key, glm::vec3 vec);
    void uniformVector(std::string key, glm::vec4 vec);

    static Shader* oneColorShader(const char *argv[], glm::vec3 color);

private:
    glimac::Program program;
    std::unordered_map<std::string, GLint> locations;

    GLint getLocation(std::string & location);
};


#endif // DEF_HEADER_SHADER
