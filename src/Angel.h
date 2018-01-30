#ifndef __ANGEL_H__
#define __ANGEL_H__

#include "OBJFile.h"
#include "Vector3.h"
#include "Matrix4.h"
#include "ShaderProgram.h"
#include "Dungeon.h"

class Angel
{
protected:
    static OBJFile *m_modelWeeping, *m_modelAttack;
    static Matrix4 m_matrixWeeping, m_matrixAttack;
    static ShaderProgram *m_shader;

    Vector3 m_position;
    float m_rotation;
    bool m_isAttacking;
    bool m_isActive;

public:
    Angel(Vector3& position): m_position(position), m_isAttacking(false), m_isActive(false), m_rotation(0.0f) {}

    static void setModels(OBJFile *weeping, OBJFile *attacking) {m_modelWeeping = weeping; m_modelAttack = attacking;}
    static void setMatrices(Matrix4& weeping, Matrix4& attacking) {m_matrixWeeping = weeping; m_matrixAttack = attacking;}
    static void setShader(ShaderProgram *shader) {m_shader = shader;}

    Vector3 collide(Vector3& position, Vector3& direction, float radius);
    void update(Vector3& position, Vector3& direction, float radius, std::vector<Angel>& angels, Dungeon& dungeon, double elapsedTime);
    void render(Matrix4& frustum, Matrix4& view, Matrix4& lightFV);
};

#endif
