#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

#include "ShaderProgram.h"
#include "OBJFile.h"
#include "CubeMap.h"
#include "Matrix4.h"
#include "StereoCamera.h"
#include "Dungeon.h"
#include "Angel.h"
#include <vector>

class GameObject
{
protected:
    ShaderProgram m_shadowShader;
    GLuint m_shadowTexture;
    GLuint m_shadowFramebuffer;

    ShaderProgram m_lightingShader;
    OBJFile m_angelModel1, m_angelModel2;

    ShaderProgram m_textureShader, m_unlitShader;
    TGAFile m_wallTextures[3], m_floorTextures[3];
    OBJFile m_wallModel, m_floorModel;

    Dungeon m_dungeon;
    std::vector<Angel> m_angels;

    bool m_isDownW = false, m_isDownA = false, m_isDownS = false, m_isDownD = false;
    float m_pitchAngle = 0.0f, m_yawAngle = 0.0f;
    Vector3 m_position = Vector3(0.0f);
    unsigned m_modelIndex = 0;
    bool m_isLit = true;
    bool m_noClip = false;

    float m_windowWidth, m_windowHeight;

public:
    GameObject();

    void setModelIndex(unsigned index) {m_modelIndex = index;}
    void setDownW(bool down) {m_isDownW = down;}
    void setDownA(bool down) {m_isDownA = down;}
    void setDownS(bool down) {m_isDownS = down;}
    void setDownD(bool down) {m_isDownD = down;}
    void lookPitch(float pitch) {m_pitchAngle = fmaxf(-60.0f, fminf(60.0f, m_pitchAngle + pitch));}
    void lookYaw(float yaw) {m_yawAngle += yaw;}
    void toggleIsLit() {m_isLit = !m_isLit;}
    void toggleNoClip() {m_noClip = !m_noClip;}
    void regenDungeon() {m_angels.clear(); m_dungeon.generateMap(m_angels);}
    void resize(float width, float height) {m_windowWidth = width; m_windowHeight = height;}

    void update(double elapsedTime);
    void render(Matrix4& frustum);
    void render(StereoCamera& stereo);
    
    void renderShadow(Matrix4& proj, Matrix4& view, Matrix4& lightFV);
    void renderObjects(Matrix4& proj, Matrix4& view, Matrix4& lightFV);
};

#endif
