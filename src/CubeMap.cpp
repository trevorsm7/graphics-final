#include "CubeMap.h"

void CubeMap::loadDirectory(std::string dirname)
{
    TGAFile image;

    if (m_texture == 0)
        glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture);

    image.openFile(dirname + "/posX.tga");
    image.bufferTexture(GL_TEXTURE_CUBE_MAP_POSITIVE_X);

    image.openFile(dirname + "/posY.tga");
    image.bufferTexture(GL_TEXTURE_CUBE_MAP_POSITIVE_Y);

    image.openFile(dirname + "/posZ.tga");
    image.bufferTexture(GL_TEXTURE_CUBE_MAP_POSITIVE_Z);

    image.openFile(dirname + "/negX.tga");
    image.bufferTexture(GL_TEXTURE_CUBE_MAP_NEGATIVE_X);

    image.openFile(dirname + "/negY.tga");
    image.bufferTexture(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);

    image.openFile(dirname + "/negZ.tga");
    image.bufferTexture(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}