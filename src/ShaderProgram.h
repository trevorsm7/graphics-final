#ifndef __SHADERPROGRAM_H__
#define __SHADERPROGRAM_H__

#include <string>
#include <unordered_map>
#include <OpenGL/gl3.h>

class ShaderProgram
{
    typedef std::unordered_map<std::string, GLint> attr_map;

protected:
    GLuint m_vertexShader;
    GLuint m_fragmentShader;
    GLuint m_geometryShader;
    GLuint m_tessControlShader;
    GLuint m_tessEvaluationShader;
    GLuint m_program;
    attr_map m_attributes;
    attr_map m_uniforms;

protected:
    static GLuint loadShader(std::string filename, GLenum shaderType);

public:
    ShaderProgram(): m_vertexShader(0), m_fragmentShader(0), m_geometryShader(0), m_tessControlShader(0),
                     m_tessEvaluationShader(0), m_program(0) {}
    ~ShaderProgram()
    {
        glDeleteShader(m_vertexShader);
        glDeleteShader(m_fragmentShader);
        glDeleteShader(m_geometryShader);
        glDeleteShader(m_tessControlShader);
        glDeleteShader(m_tessEvaluationShader);
        glDeleteProgram(m_program);
    }

    void loadVertexShader(std::string filename)
    {
        GLuint shader = loadShader(filename, GL_VERTEX_SHADER);
        glDeleteShader(m_vertexShader); // only replace old shader if compile doesn't throw exception
        m_vertexShader = shader;
    }

    void loadFragmentShader(std::string filename)
    {
        GLuint shader = loadShader(filename, GL_FRAGMENT_SHADER);
        glDeleteShader(m_fragmentShader); // only replace old shader if compile doesn't throw exception
        m_fragmentShader = shader;
    }

    void loadGeometryShader(std::string filename)
    {
        GLuint shader = loadShader(filename, GL_GEOMETRY_SHADER);
        glDeleteShader(m_geometryShader); // only replace old shader if compile doesn't throw exception
        m_geometryShader = shader;
    }

    void loadTessControlShader(std::string filename)
    {
        GLuint shader = loadShader(filename, GL_TESS_CONTROL_SHADER);
        glDeleteShader(m_tessControlShader); // only replace old shader if compile doesn't throw exception
        m_tessControlShader = shader;
    }

    void loadTessEvaluationShader(std::string filename)
    {
        GLuint shader = loadShader(filename, GL_TESS_EVALUATION_SHADER);
        glDeleteShader(m_tessEvaluationShader); // only replace old shader if compile doesn't throw exception
        m_tessEvaluationShader = shader;
    }

    void linkProgram();

    void useProgram() {glUseProgram(m_program);}

    // lookup attributes in cache first before querying GL (reset cache when program is compiled!)
    GLint getAttribLocation(const GLchar* name)
    {
        GLint attribute;

        auto iterator = m_attributes.find(name);
        if (iterator == m_attributes.end())
        {
            attribute = glGetAttribLocation(m_program, name);
            m_attributes[name] = attribute;
        }
        else
            attribute = iterator->second;

        return attribute;
    }

    GLint getUniformLocation(const GLchar* name)
    {
        GLint uniform;

        auto iterator = m_uniforms.find(name);
        if (iterator == m_uniforms.end())
        {
            uniform = glGetUniformLocation(m_program, name);
            m_uniforms[name] = uniform;
        }
        else
            uniform = iterator->second;

        return uniform;
    }
};

#endif
