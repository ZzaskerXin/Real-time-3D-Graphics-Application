#pragma once
#include <GL/glew.h>
#include <iostream>

namespace Debug
{
    inline void glErrorCheck(const char* file, int line) {
        GLenum errorNumber = glGetError();
        while (errorNumber != GL_NO_ERROR) {
            // Task 2: Edit this print statement to be more descriptive
            // std::cout << errorNumber << std::endl;
            std::cout << "OpenGL Error (" << errorNumber << "): ";
            std::cout << " in file " << file << " at line " << line << std::endl;
            errorNumber = glGetError();
        }
    }
}


#define glErrorCheck() glErrorCheck(__FILE__, __LINE__)
