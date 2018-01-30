#include "Angel.h"

OBJFile *Angel::m_modelWeeping = nullptr, *Angel::m_modelAttack = nullptr;
Matrix4 Angel::m_matrixWeeping, Angel::m_matrixAttack;
ShaderProgram *Angel::m_shader = nullptr;

Vector3 Angel::collide(Vector3& position, Vector3& direction, float radius)
{
    Vector3 toAngel = m_position - position;

    // Return early if collision not possible
    if (direction.length() == 0.0f || toAngel.length() < direction.length() - 2.0f * radius || toAngel.dot(direction) < 0.0f)
        return direction;

    Vector3 up = Vector3(0.0f, 1.0f, 0.0f);
    Vector3 directionNormal = direction / direction.length();
    Vector3 directionTangent = direction.cross(up).normalize();
    float rayDistance = toAngel.dot(directionTangent);

    if (fabsf(rayDistance) < 2.0f * radius)
    {
        float collisionDistance = toAngel.dot(directionNormal);
        collisionDistance -= sqrtf(4.0f * radius * radius - rayDistance * rayDistance);
        if (/*collisionDistance > 0.0f &&*/ collisionDistance < direction.length())
        {
            Vector3 collision = position + directionNormal * collisionDistance;
            Vector3 collisionNormal = (collision - m_position).normalize();;
            float planeDistance = (position + direction - m_position).dot(collisionNormal);
            return direction - collisionNormal * (planeDistance - 2.0f * radius);
        }
    }

    return direction;
}

void Angel::update(Vector3& position, Vector3& direction, float radius, std::vector<Angel>& angels, Dungeon& dungeon, double elapsedTime)
{
    Vector3 angelToPlayer = position - m_position;
    float distanceToPlayer = angelToPlayer.length();
    if (distanceToPlayer < 2.0f)
        m_isActive = true;
    else if (distanceToPlayer > 8.0f)
        m_isActive = false;

    // Angel is active and player is looking away
    if (m_isActive && angelToPlayer.dot(direction) >= 0.0f)
    {
        if (angelToPlayer.getZ() > 0.0f)
            m_rotation = atan(-angelToPlayer.getX() / angelToPlayer.getZ()) * 180.0 / 3.14159;
        else if (angelToPlayer.getZ() < 0.0f)
            m_rotation = 180.0f + atan(-angelToPlayer.getX() / angelToPlayer.getZ()) * 180.0 / 3.14159;
        else
            m_rotation = angelToPlayer.getX() > 0.0f ? 90.0f : -90.0f;

        if (distanceToPlayer > radius)
        {
            float movement = 2.0f * elapsedTime; // move twice as fast as player
            movement = fminf(movement, distanceToPlayer - 2.0f * radius); // use same radius for angel?
            Vector3 moveVector = angelToPlayer.normalize() * movement;
            moveVector = dungeon.collide(m_position, moveVector, radius);
            for (int i = 0; i < angels.size(); ++i)
                if (&angels[i] != this)
                    moveVector = angels[i].collide(m_position, moveVector, radius);
            m_position += moveVector;
        }

        m_isAttacking = (position - m_position).length() < 0.5f;
    }
}

void Angel::render(Matrix4& frustum, Matrix4& view, Matrix4& lightFV)
{
    OBJFile *model = m_isAttacking ? m_modelAttack : m_modelWeeping;
    Matrix4& scale = m_isAttacking ? m_matrixAttack : m_matrixWeeping;

    Matrix4 modelMatrix = Matrix4().translate(m_position) * Matrix4().rotateY(m_rotation) * scale;
    Matrix4 shadowMVP = lightFV * modelMatrix;
    glUniformMatrix4fv(m_shader->getUniformLocation("u_shadowMVP"), 1, GL_FALSE, shadowMVP.getPointer());

    Matrix4 modelView = view * modelMatrix;
    model->render(frustum, modelView, *m_shader);
}