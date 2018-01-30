//
//  OBJFile.cpp
//  Renderer
//
//  Created by Trevor Smith on 8/3/13.
//  Copyright (c) 2013 Trevor Smith. All rights reserved.
//

#include "OBJFile.h"
#include "Vector3.h"
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <cmath>

OBJFile::~OBJFile()
{
    if (m_vertexArrayObject)
        glDeleteVertexArrays(1, &m_vertexArrayObject);
    if (m_vertexBuffer)
        glDeleteBuffers(1, &m_vertexBuffer);
    if (m_indexBuffer)
        glDeleteBuffers(1, &m_indexBuffer);
}

void OBJFile::loadOBJ(const char *filename, ShaderProgram& program)
{
    std::fstream file;
    file.open(filename, file.in);
    if (!file.is_open())
        return;

    unsigned vIndex = 0, tIndex = 0, nIndex = 0;
    std::vector<Vertex> vertices;
    std::vector<Face> faces;

    char line[120];

    while (!file.eof())
    {
        file.getline(line, sizeof(line));
        if (file.fail())
            break;

        switch (line[0])
        {
        case '#':
        case '\0':
        case '\r':
            break;
        case 'v':
        {
            // Determine vertex parameter type
            unsigned* index;
            unsigned indexCount;
            unsigned byteOffset;
            switch (line[1])
            {
            case ' ':
                index = &vIndex;
                indexCount = 3;
                byteOffset = offsetof(Vertex, p.p[0]);
                break;
            case 't':
                index = &tIndex;
                indexCount = 2;
                byteOffset = offsetof(Vertex, t.t[0]);
                break;
            case 'n':
                index = &nIndex;
                indexCount = 3;
                byteOffset = offsetof(Vertex, n.n[0]);
                break;
            default:
                std::cerr << "OBJFile: can't parse line [" << line << "]" << std::endl;
                continue;
            }

            // Resize buffer if index goes past size
            char* start = &line[2], * end;
            if (*index == vertices.size())
            {
                Vertex empty;
                vertices.push_back(empty);
            }

            // Get a pointer to the first index of the appropriate vertex parameter array (pos, tex, or norm)
            float* vPtr = reinterpret_cast<float*>(reinterpret_cast<char*>(&vertices[*index]) + byteOffset);

            unsigned i;
            for (i = 0; i < indexCount; ++i)
            {
                vPtr[i] = strtof(start, &end);
                if (start == end)
                    break;
                start = end;
            }

            if (i < indexCount)
                std::cerr << "OBJFile: vertex has less than 3 components [" << line << "]" << std::endl;

            ++(*index);
        }
            break;
        case 'f':
            if (line[1] == ' ')
            {
                char *start = &line[2], *end;
                Face f;

                unsigned i;
                for (i = 0; i < 3; ++i)
                {
                    // read vertex coordinate index; subtract 1 so that the first coordinate is 0 not 1
                    f.f[i] = static_cast<int>(strtol(start, &end, 10) - 1);
                    if (start == end)
                        break;
                    start = end;

                    if (*start == '/')
                    {
                        ++start;

                        // read texture coordinate index
                        if (*start != '/')
                        {
                            float t = static_cast<int>(strtol(start, &end, 10) - 1);
                            if (start == end)
                                break;
                            start = end;

                            if (t != f.f[i])
                                std::cerr << "OBJFile: vertex/texCoord index mismatch [" << line << "]" << std::endl;
                        }

                        // read normal coordinate index
                        if (*start == '/')
                        {
                            ++start;

                            float n = static_cast<int>(strtol(start, &end, 10) - 1);
                            if (start == end)
                                break;
                            start = end;

                            if (n != f.f[i])
                                std::cerr << "OBJFile: vertex/normal index mismatch [" << line << "]" << std::endl;
                        }
                    }
                }

                if (i < 3)
                    std::cerr << "OBJFile: face has less than 3 indices [" << line << "]" << std::endl;

                faces.push_back(f);
            }
            else
                std::cerr << "OBJFile: can't parse line [" << line << "]" << std::endl;
            break;
        default:
            std::cerr << "OBJFile: unparsed line [" << line << "]" << std::endl;
            break;
        }
    }

    file.close();

    #if 0
    // Loop through the whole set of vertices
    float minX, minY, minZ;
    float maxX, maxY, maxZ;
    maxX = maxY = maxZ = std::numeric_limits<float>::lowest();
    minX = minY = minZ = std::numeric_limits<float>::max();
    for (Vertex v : vertices)
    {
        // Recompute the min/max boundaries
        minX = fminf(v.p.p[0], minX);
        maxX = fmaxf(v.p.p[0], maxX);
        minY = fminf(v.p.p[1], minY);
        maxY = fmaxf(v.p.p[1], maxY);
        minZ = fminf(v.p.p[2], minZ);
        maxZ = fmaxf(v.p.p[2], maxZ);
    }

    // Compute scale to fill a 10 unit cube
    /*float scale = 10.0f / fmaxf(maxX - minX, fmaxf(maxY - minY, maxZ - minZ));
    m_resize *= Matrix4().scale(scale, scale, scale);

    // Compute translate to center of object
    float shiftX = (maxX + minX) / -2.0f;
    float shiftY = (maxY + minY) / -2.0f;
    float shiftZ = (maxZ + minZ) / -2.0f;
    m_resize *= Matrix4().translate(shiftX, shiftY, shiftZ);*/
    #endif

    m_numIndices = faces.size() * 3;

    // Generate vertex array object
    glGenVertexArrays(1, &m_vertexArrayObject);
    glBindVertexArray(m_vertexArrayObject);

    // Buffer the vertex data
    glGenBuffers(1, &m_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    // Set up vertex attribute buffer
    GLint a_vertex = program.getAttribLocation("a_vertex");
    glEnableVertexAttribArray(a_vertex);
    glVertexAttribPointer(a_vertex, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, p)));

    GLint a_texCoord = program.getAttribLocation("a_texCoord");
    if (tIndex > 0 && a_texCoord >= 0)
    {
        glEnableVertexAttribArray(a_texCoord);
        glVertexAttribPointer(a_texCoord, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, t)));
    }

    GLint a_normal = program.getAttribLocation("a_normal");
    if (nIndex > 0 && a_normal >= 0)
    {
        glEnableVertexAttribArray(a_normal);
        glVertexAttribPointer(a_normal, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, n)));
    }

    // Buffer the index data
    glGenBuffers(1, &m_indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.size() * sizeof(Face), faces.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void OBJFile::render(Matrix4& projection, Matrix4& modelView, ShaderProgram& program)
{
    // Bind the shader program
    program.useProgram();

    // Send matrix pointers to shader
    glUniformMatrix4fv(program.getUniformLocation("u_projection"), 1, GL_FALSE, projection.getPointer());
    glUniformMatrix4fv(program.getUniformLocation("u_modelView"), 1, GL_FALSE, modelView.getPointer());

    // Send vertices to shader
    glBindVertexArray(m_vertexArrayObject);
    glDrawElements(GL_TRIANGLES, m_numIndices, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
