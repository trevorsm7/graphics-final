#ifndef __DUNGEON_H__
#define __DUNGEON_H__

#include "OBJFile.h"
#include "TGAFile.h"
#include "ShaderProgram.h"
#include "Matrix4.h"
#include "Vector3.h"
//#include "Angel.h"
#include <string>
#include <vector>
#include <set>
#include <random>

class Angel;

class Dungeon
{
public:
    class Room
    {
        friend class Dungeon;
        friend std::ostream& operator<<(std::ostream&, const Dungeon::Room&);

    protected:
        float m_left, m_top, m_width, m_height;
        std::multiset<Vector3> m_segments[4]; // top, right, bottom, left
        std::vector<Room*> m_adjacent;
        bool m_isVisible;

    public:
        Room(float left, float top, float width, float height): m_left(left), m_top(top), m_width(width), m_height(height) {}

        bool isVisible() {return m_isVisible;}
        void setVisible(bool isVisible) {m_isVisible = isVisible;}
        void addAdjacent(Room *adjacent) {m_adjacent.push_back(adjacent);}
        void generateSegments();

        bool isOverlapping(Room *other);
        bool isAdjactent(Room *other);
        bool contains(Vector3& position);
        Vector3 collide(Vector3& position, Vector3& direction, float radius);

        void renderWall(Matrix4& frustum, Matrix4& view, Matrix4& lightFV, OBJFile& model, ShaderProgram& shader);
        void renderFloor(Matrix4& frustum, Matrix4& view, Matrix4& lightFV, OBJFile& model, ShaderProgram& shader);
    };

    class Random
    {
    protected:
        std::default_random_engine m_generator;
        std::uniform_real_distribution<float> m_distribution;

    public:
        Random(): m_distribution(0.0f, 1.0f) {}
        //generator.seed(<#(_Sseq &)__q#>); // do not seed, so we get repeatable results
        float randFloat() {return m_distribution(m_generator);}
        float randFloat(float max) {return floorf((max + 1.0f) * m_distribution(m_generator));}
        int randInt(int max) {return floorf((max + 1) * m_distribution(m_generator));}
    };

protected:
    std::vector<Room*> m_roomList;
    OBJFile *m_wallModel, *m_floorModel;
    TGAFile *m_wallTexture, *m_floorTexture;
    ShaderProgram *m_shader;
    Random m_random;

public:
    Dungeon(): m_wallModel(nullptr), m_floorModel(nullptr), m_wallTexture(nullptr), m_floorTexture(nullptr), m_shader(nullptr) {}
    ~Dungeon() {for (Room *room : m_roomList) delete room;}

    void setModels(OBJFile *wall, OBJFile *floor) {m_wallModel = wall; m_floorModel = floor;}
    void setTextures(TGAFile *wall, TGAFile *floor) {m_wallTexture = wall; m_floorTexture = floor;}
    void setShader(ShaderProgram *shader) {m_shader = shader;}

    void generateMap(std::vector<Angel>& angels);

    Room *getRoom(Vector3& position);
    Vector3 collide(Vector3& position, Vector3& direction, float radius);

    void update(Vector3& position);
    void render(Matrix4& frustum, Matrix4& view, Matrix4& lightFV);
};

std::ostream& operator<<(std::ostream& out, const Dungeon::Room& r);

#endif
