#ifndef __CUBEMAP_H__
#define __CUBEMAP_H__

#define GLFW_INCLUDE_GLCOREARB
#include "GLFW/glfw3.h"
#include <string>
#include "TGAFile.h"

class CubeMap
{
protected:
    GLuint m_texture;

public:
    CubeMap(): m_texture(0) {}
    ~CubeMap() {if (m_texture) glDeleteTextures(1, &m_texture);}

    void loadDirectory(std::string dirname);
    void bindTexture() {glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture);}
};

#endif
