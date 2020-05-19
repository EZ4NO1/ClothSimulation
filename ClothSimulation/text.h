#pragma once
#include "include.h"
#include "Shader.h"
class text
{public:
    struct Character {
        unsigned int TextureID; // ID handle of the glyph texture
        glm::ivec2   Size;      // Size of glyph
        glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
        unsigned int Advance;   // Horizontal offset to advance to next glyph
    };
    std::map<GLchar, Character> Characters;
    unsigned int VAO, VBO;
    GLuint ShaderID;
    text(vec2 windowsize,const char* fontfile);
    text();
    void display(std::string text, vec2 pos, float scale, glm::vec3 color);
};






