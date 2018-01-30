#include "Dungeon.h"
#include <fstream>
#include <algorithm>
#include <random>
#include <list>
#include "Angel.h"

void Dungeon::Room::generateSegments()
{
    // Start each list with the endpoints of the wall
    // Top plane
    m_segments[0].clear();
    m_segments[0].insert(Vector3(m_left, 0.0f, m_top));
    m_segments[0].insert(Vector3(m_left + m_width, 0.0f, m_top));

    // Right plane
    m_segments[1].clear();
    m_segments[1].insert(Vector3(m_left + m_width, 0.0f, m_top));
    m_segments[1].insert(Vector3(m_left + m_width, 0.0f, m_top + m_height));

    // Bottom plane
    m_segments[2].clear();
    m_segments[2].insert(Vector3(m_left, 0.0f, m_top + m_height));
    m_segments[2].insert(Vector3(m_left + m_width, 0.0f, m_top + m_height));

    // Left plane
    m_segments[3].clear();
    m_segments[3].insert(Vector3(m_left, 0.0f, m_top));
    m_segments[3].insert(Vector3(m_left, 0.0f, m_top + m_height));

    // Loop through adjacent rooms, breaking up wall where there are doorways/connections
    // For example wall A-----B initially has list [A, B]. If door C-D is added, the wall
    // looks like A--C D--B and the list becomes [A, C, D, B]
    for (Room *room : m_adjacent)
    {
        if (room->m_top + room->m_height == m_top)
        {
            m_segments[0].insert(Vector3(std::max(m_left, room->m_left), 0.0f, m_top));
            m_segments[0].insert(Vector3(std::min(m_left + m_width, room->m_left + room->m_width), 0.0f, m_top));
        }
        else if (room->m_left == m_left + m_width)
        {
            m_segments[1].insert(Vector3(m_left + m_width, 0.0f, std::max(m_top, room->m_top)));
            m_segments[1].insert(Vector3(m_left + m_width, 0.0f, std::min(m_top + m_height, room->m_top + room->m_height)));
        }
        else if (room->m_top == m_top + m_height)
        {
            m_segments[2].insert(Vector3(std::max(m_left, room->m_left), 0.0f, m_top + m_height));
            m_segments[2].insert(Vector3(std::min(m_left + m_width, room->m_left + room->m_width), 0.0f, m_top + m_height));
        }
        else if (room->m_left + room->m_width == m_left)
        {
            m_segments[3].insert(Vector3(m_left, 0.0f, std::max(m_top, room->m_top)));
            m_segments[3].insert(Vector3(m_left, 0.0f, std::min(m_top + m_height, room->m_top + room->m_height)));
        }
    }
}

bool Dungeon::Room::contains(Vector3& position)
{
    return position.getX() >= m_left && position.getX() < m_left + m_width && position.getZ() >= m_top && position.getZ() < m_top + m_height;
}

Vector3 Dungeon::Room::collide(Vector3& position, Vector3& direction, float radius)
{
    Vector3 directionClipped = direction;
    Vector3 up = Vector3(0.0f, 1.0f, 0.0f);
    Vector3 directionTangent = directionClipped.cross(up).normalize();

    Vector3 segmentNormal[4];
    segmentNormal[0] = Vector3(0.0f, 0.0f, 1.0f); // Top plane
    segmentNormal[1] = Vector3(-1.0f, 0.0f, 0.0f); // Right plane
    segmentNormal[2] = Vector3(0.0f, 0.0f, -1.0f); // Bottom plane
    segmentNormal[3] = Vector3(1.0f, 0.0f, 0.0f); // Left plane

    for (unsigned i = 0; i < 4; ++i)
    {
        if (directionClipped.length() == 0.0f)
            return directionClipped;

        // Compute the perpendicular distance from (pos+dir) to plane
        Vector3 corner = *(m_segments[i].begin());
        float distance = (position + directionClipped - corner).dot(segmentNormal[i]);
        if (distance >= radius)
            continue; // too far from plane; no collision possible

        float directionPerpendicular = segmentNormal[i].dot(directionClipped);

        // Check edges first (but only if facing edge)
        bool didCollideEdge = false;
        if (directionPerpendicular < 0.0f)
        {
            float distancePerpendicular = directionPerpendicular - (distance - radius);
            Vector3 collisionPoint = position + directionClipped * (distancePerpendicular / directionPerpendicular);

            auto it = m_segments[i].begin();
            while (it != m_segments[i].end())
            {
                Vector3 leftCorner = *(it++), rightCorner = *(it++);
                if (leftCorner == rightCorner)
                    continue;

                // If necessary, swap such that leftCorner is actually left of rightCorner
                if ((rightCorner - leftCorner).dot(directionTangent) < 0.0f)
                //if (segmentNormal[i].cross(rightCorner - leftCorner).getY() < 0.0f)
                {
                    Vector3 swap = leftCorner;
                    leftCorner = rightCorner;
                    rightCorner = swap;
                }

                //std::cout << "pos[" << position << "] clip[" << directionClipped << "] tan[" << directionTangent << "] leftCorner[" << leftCorner << "] rightCorner[" << rightCorner << "]\n";
                Vector3 segmentTangent = (rightCorner - leftCorner).normalize();
                float collisionDistLeft = (collisionPoint - leftCorner).dot(segmentTangent);
                float collisionDistRight = (rightCorner - collisionPoint).dot(segmentTangent);

                // If there is a collision, set the perpendicular component to be just touching the plane
                // NOTE: if we use -radius as the lower bound, we can treat the radius as a cube instead of a sphere,
                // allowing us to skip collisions with corners
                //std::cout << "collisionPoint[" << collisionPoint << "] cdl: " << collisionDistLeft << " cdr: " << collisionDistRight << std::endl;
                if (collisionDistLeft > 0.0f && collisionDistRight > 0.0f)
                //if (collisionDistLeft >= -radius && collisionDistRight >= -radius)
                {
                    directionClipped -= segmentNormal[i] * (distance - radius);
                    directionTangent = directionClipped.cross(up).normalize(); // only recompute when directionClipped changes
                    didCollideEdge = true;
                    //std::cout << "handling edge collision" << std::endl;
                    break;
                }
            }
        }

        // Do not check corners if we collided with an edge
        if (didCollideEdge)
            continue;

        // Loop through corners
        // NOTE: can we just keep colliding with as many corners as we run into?
        auto it = m_segments[i].begin();
        while (it != m_segments[i].end())
        {
            if (directionClipped.length() == 0.0f)
                return directionClipped;

            Vector3 leftCorner = *(it++), rightCorner = *(it++);
            if (leftCorner == rightCorner)
                continue;

            // If necessary, swap such that leftCorner is actually left of rightCorner
            if ((rightCorner - leftCorner).dot(directionTangent) < 0.0f)
            //if (segmentNormal[i].cross(rightCorner - leftCorner).getY() < 0.0f)
            {
                Vector3 swap = leftCorner;
                leftCorner = rightCorner;
                rightCorner = swap;
            }

            Vector3 directionNormalized = directionClipped; directionNormalized.normalize();

            // Ray to corner distance is within radius
            float rayDistLeft = (leftCorner - position).dot(directionTangent);
            //if (fabsf(rayDistLeft) < radius)
            if ((position - leftCorner).dot(rightCorner - leftCorner) < 0.0f && rayDistLeft < radius)
            {
                float collisionDistance = (leftCorner - position).dot(directionNormalized);
                collisionDistance -= sqrtf(radius * radius - rayDistLeft * rayDistLeft);
                if (/*collisionDistance > 0.0f && */collisionDistance < directionClipped.length())
                {
                    Vector3 collision = position + directionNormalized * collisionDistance;
                    Vector3 cornerNorm = (collision - leftCorner).normalize();
                    directionClipped -= cornerNorm * ((position + directionClipped - leftCorner).dot(cornerNorm) - radius);
                    directionTangent = directionClipped.cross(up).normalize(); // only recompute when directionClipped changes
                    continue;
                }
            }

            // Ray to corner distance is within radius
            float rayDistRight = (rightCorner - position).dot(directionTangent);
            //if (fabsf(rayDistRight) < radius)
            if ((position - rightCorner).dot(leftCorner - rightCorner) < 0.0f && rayDistRight > -radius)
            {
                float collisionDistance = (rightCorner - position).dot(directionNormalized);
                collisionDistance -= sqrtf(radius * radius - rayDistRight * rayDistRight);
                if (collisionDistance > 0.0f && collisionDistance < directionClipped.length())
                {
                    Vector3 collision = position + directionNormalized * collisionDistance;
                    Vector3 cornerNorm = (collision - rightCorner).normalize();
                    directionClipped -= cornerNorm * ((position + directionClipped - rightCorner).dot(cornerNorm) - radius);
                    directionTangent = directionClipped.cross(up).normalize(); // only recompute when directionClipped changes
                    //std::cout << "handling right corner collision" << std::endl;
                    continue;
                }
            }
        }
    }

    return directionClipped;
}

void Dungeon::Room::renderWall(Matrix4& frustum, Matrix4& view, Matrix4& lightFV, OBJFile& model, ShaderProgram& shader)
{
    Matrix4 rotation[4];
    rotation[0] = Matrix4().identity();
    rotation[1] = Matrix4().rotateY(90.0f);
    rotation[2] = Matrix4().translate(1.0f, 0.0f, 0.0f) * Matrix4().rotateY(180.0f);
    rotation[3] = Matrix4().translate(0.0f, 0.0f, 1.0f) * Matrix4().rotateY(270.0f);

    for (unsigned i = 0; i < 4; ++i)
    {
        auto it = m_segments[i].begin();
        while (it != m_segments[i].end())
        {
            Vector3 min = *(it++), max = *(it++);
            Vector3 n = max - min;
            float length = n.length();
            if (length > 0.0f)
            {
                n /= length;
                for (float s = 0.0f; s < length; s += 1.0f)
                {
                    Vector3 p = min + (n * s);
                    Matrix4 modelMatrix = Matrix4().translate(p) * rotation[i];
                    Matrix4 modelView = view * modelMatrix;
                    Matrix4 shadowMVP = lightFV * modelMatrix;
                    glUniformMatrix4fv(shader.getUniformLocation("u_shadowMVP"), 1, GL_FALSE, shadowMVP.getPointer());
                    model.render(frustum, modelView, shader);
                }
            }
        }
    }
}

void Dungeon::Room::renderFloor(Matrix4& frustum, Matrix4& view, Matrix4& lightFV, OBJFile& model, ShaderProgram& shader)
{
    // Render floor tiles for each enclosed square
    for (float z = m_top; z < m_top + m_height; ++z)
        for (float x = m_left; x < m_left + m_width; ++x)
        {
            Matrix4 modelMatrix = Matrix4().translate(x, 0.0f, z);
            Matrix4 modelView = view * modelMatrix;
            Matrix4 shadowMVP = lightFV * modelMatrix;
            glUniformMatrix4fv(shader.getUniformLocation("u_shadowMVP"), 1, GL_FALSE, shadowMVP.getPointer());
            model.render(frustum, modelView, shader);
        }
}

bool Dungeon::Room::isOverlapping(Room *other)
{
    float l1 = m_left, r1 = m_left + m_width;
    float t1 = m_top, b1 = m_top + m_height;
    float l2 = other->m_left, r2 = other->m_left + other->m_width;
    float t2 = other->m_top, b2 = other->m_top + other->m_height;

    return r1 > l2 && r2 > l1 && b1 > t2 && b2 > t1;
}

bool Dungeon::Room::isAdjactent(Room *other)
{
    float l1 = m_left, r1 = m_left + m_width;
    float t1 = m_top, b1 = m_top + m_height;
    float l2 = other->m_left, r2 = other->m_left + other->m_width;
    float t2 = other->m_top, b2 = other->m_top + other->m_height;

    return ((r1 == l2 || r2 == l1) && b1 > t2 && b2 > t1) ||
        (r1 > l2 && r2 > l1 && (b1 == t2 || b2 == t1));
}

std::ostream& operator<<(std::ostream& out, const Dungeon::Room& r)
{
    return out << "L=" << r.m_left << " T=" << r.m_top << " W=" << r.m_width << " H=" << r.m_height;
}

void Dungeon::generateMap(std::vector<Angel>& angels)
{
    // Delete any existing rooms before generating more
    while (!m_roomList.empty())
    {
        delete m_roomList.back();
        m_roomList.pop_back();
    }

    Room *room;
    std::list<Room*> buildOrder;
    float left, top, width, height;

    // Create starting room
    width = 3.0f + m_random.randFloat(2.0f);
    height = 3.0f + m_random.randFloat(2.0f);
    room = new Room(-width / 2.0f, -height / 2.0f, width, height);
    //std::cout << "Start: " << *room << std::endl;
    m_roomList.push_back(room);
    for (int i = 3 + m_random.randInt(3); i > 0; --i)
        buildOrder.push_back(room);

    while (!buildOrder.empty() && m_roomList.size() < 30)
    {
        Room *parent = buildOrder.front(); buildOrder.pop_front();
        int direction = m_random.randInt(3);
        bool isHallway = (m_random.randInt(3) != 0);
        switch (direction)
        {
        case 0: // top
            //width = isHallway ? 1.0f : m_random.randFloat(parent->m_width - 1.0f) + 1.0f;
            width = isHallway ? 1.0f : 2.0f + m_random.randFloat(3.0f);
            height = 2.0f + m_random.randFloat(3.0f);
            left = parent->m_left + m_random.randFloat(parent->m_width - 1.0f);//width);
            top = parent->m_top - height;
            break;
        case 1: // right
            width = 2.0f + m_random.randFloat(3.0f);
            //height = isHallway ? 1.0f : m_random.randFloat(parent->m_height - 1.0f) + 1.0f;
            height = isHallway ? 1.0f : 2.0f + m_random.randFloat(3.0f);
            left = parent->m_left + parent->m_width;
            top = parent->m_top + m_random.randFloat(parent->m_height - 1.0f);//height);
            break;
        case 2: // bottom
            //width = isHallway ? 1.0f : m_random.randFloat(parent->m_width - 1.0f) + 1.0f;
            width = isHallway ? 1.0f : 2.0f + m_random.randFloat(3.0f);
            height = 2.0f + m_random.randFloat(3.0f);
            left = parent->m_left + m_random.randFloat(parent->m_width - 1.0f);//width);
            top = parent->m_top + parent->m_height;
            break;
        case 3: // left
            width = 2.0f + m_random.randFloat(3.0f);
            //height = isHallway ? 1.0f : m_random.randFloat(parent->m_height - 1.0f) + 1.0f;
            height = isHallway ? 1.0f : 2.0f + m_random.randFloat(3.0f);
            left = parent->m_left - width;
            top = parent->m_top + m_random.randFloat(parent->m_height - 1.0f);//height);
            break;
        default:
            continue;
        }

        room = new Room(left, top, width, height);
        for (Room *other : m_roomList)
        {
            if (room->isOverlapping(other))
            {
                switch (direction)
                {
                case 0: // shorten top
                    room->m_top = other->m_top + other->m_height;
                    room->m_height = room->m_top - parent->m_top;
                    break;
                case 1: // shorten right
                    room->m_width = other->m_left - room->m_left;
                    break;
                case 2: // shorten bottom
                    room->m_height = other->m_top - room->m_top;
                    break;
                case 3: // shorten left
                    room->m_left = other->m_left + other->m_width;
                    room->m_width = room->m_left - parent->m_left;
                    break;
                }
            }
        }

        //std::cout << "Room: " << *room << std::endl;
        if (room->m_height <= 0.0f || room->m_width <= 0.0f)
        {
            //std::cout << "Discarding room: " << *room << std::endl;
            delete room;
        }
        else
        {
            // Random chance to spawn an angel
            if (m_random.randInt(5) == 0)
            {
                Vector3 pos(room->m_left + room->m_width / 2.0f, 0.0f, room->m_top + room->m_height / 2.0f);
                angels.push_back(Angel(pos));
            }

            for (unsigned i = isHallway ? m_random.randInt(3) : m_random.randInt(5); i > 0; --i)
                buildOrder.push_back(room);
            m_roomList.push_back(room);
        }
    }

    // Build adjacency list after generation is complete
    for (unsigned i = 0; i < m_roomList.size(); ++i)
    {
        for (unsigned j = i + 1; j < m_roomList.size(); ++j)
        {
            if (m_roomList[i]->isAdjactent(m_roomList[j]))
            {
                m_roomList[i]->addAdjacent(m_roomList[j]);
                m_roomList[j]->addAdjacent(m_roomList[i]);
            }
        }
    }

    // Pre-generate wall segments from adjacency lists
    for (Room *room : m_roomList)
        room->generateSegments();
}

Dungeon::Room *Dungeon::getRoom(Vector3& position)
{
    for (Room *room : m_roomList)
        if (room->contains(position))
            return room;

    return nullptr;
}

Vector3 Dungeon::collide(Vector3& position, Vector3& direction, float radius)
{
    Room *room = getRoom(position);
    if (room)
        return room->collide(position, direction, radius);

    return direction;
}

void Dungeon::update(Vector3& position)
{

}

void Dungeon::render(Matrix4& frustum, Matrix4& view, Matrix4& lightFV)
{
    if (m_wallModel == nullptr || m_floorModel == nullptr || m_wallTexture == nullptr || m_floorTexture == nullptr || m_shader == nullptr)
        throw std::runtime_error("Dungeon::render called with null assets");

    // View culling can be done here, w/e
    for (Room *room : m_roomList)
        room->setVisible(true);

    for (Room *room : m_roomList)
    {
        if (!room->isVisible())
            continue;

        // Bind the wall textures and render walls
        for (unsigned i = 0; i < 3; ++i)
        {
            glActiveTexture(GL_TEXTURE0 + i);
            m_wallTexture[i].bindTexture();
        }
        room->renderWall(frustum, view, lightFV, *m_wallModel, *m_shader);

        // Bind the floor textures and render floors
        for (unsigned i = 0; i < 3; ++i)
        {
            glActiveTexture(GL_TEXTURE0 + i);
            m_floorTexture[i].bindTexture();
        }
        room->renderFloor(frustum, view, lightFV, *m_floorModel, *m_shader);
    }
}
