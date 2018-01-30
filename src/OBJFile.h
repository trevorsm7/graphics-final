//
//  OBJFile.h
//  Renderer
//
//  Created by Trevor Smith on 8/3/13.
//  Modified for CSE 167 homework
//

#ifndef __OBJFILE_H__
#define __OBJFILE_H__

#define GLFW_INCLUDE_GLCOREARB
#include "GLFW/glfw3.h"
#include <iostream>
#include <memory>
#include <vector>
#include "Matrix4.h"
#include "ShaderProgram.h"
#include "TGAFile.h"

class OBJFile
{
protected:
    struct Position {float p[3];} __attribute__((__packed__));
    struct Normal {float n[3];} __attribute__((__packed__));
    struct TexCoord {float t[2];} __attribute__((__packed__));
    struct Vertex {Position p; TexCoord t; Normal n;} __attribute__((__packed__));
    struct Face {int f[3];} __attribute__((__packed__));

    //Matrix4 m_resize;

    GLuint m_vertexArrayObject;
    GLuint m_vertexBuffer;
    GLuint m_indexBuffer;
    unsigned m_numIndices;
    
public:
    OBJFile(): m_vertexArrayObject(0), m_vertexBuffer(0), m_indexBuffer(0) {}
    ~OBJFile();
    void loadOBJ(const char *filename, ShaderProgram& program);
    void render(Matrix4& projection, Matrix4& modelView, ShaderProgram& program);
};

#endif
