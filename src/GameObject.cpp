#include <QuartzCore/QuartzCore.h>
#include "GameObject.h"
#include "CubeMap.h"
#include "Angel.h"

void printGLError(const char* message)
{
    std::cout << message << "\n";
    GLenum error = glGetError();
    if (error != GL_NO_ERROR)
    {
        switch (error)
        {
        case (GL_INVALID_ENUM):
            std::cout << "GL_INVALID_ENUM\n";
            break;
        case (GL_INVALID_VALUE):
            std::cout << "GL_INVALID_VALUE\n";
            break;
        case (GL_INVALID_OPERATION):
            std::cout << "GL_INVALID_OPERATION\n";
            break;
        case (GL_INVALID_FRAMEBUFFER_OPERATION):
            std::cout << "GL_INVALID_FRAMEBUFFER_OPERATION\n";
            break;
        case (GL_OUT_OF_MEMORY):
            std::cout << "GL_OUT_OF_MEMORY\n";
            break;
        /*case ():
            std::cout << "\n";
            break;*/
        default:
            std::cout << "Some other GL error\n";
            break;
        }
    }
}

GameObject::GameObject()
{
    // Generate the shadow mapping texture
    glGenTextures(1, &m_shadowTexture);
    glBindTexture(GL_TEXTURE_2D, m_shadowTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, 0.0f);
    float borderColor[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    // Generate the shadow mapping frame buffer
    glGenFramebuffers(1, &m_shadowFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_shadowFramebuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_shadowTexture, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    // Load the shadow map shader (fragment shader is unused, but needs to be included anyway)
    m_shadowShader.loadVertexShader("shadow.vsh");
    m_shadowShader.loadFragmentShader("shadow.fsh");
    m_shadowShader.linkProgram();

    // Load the lighting shader
    m_lightingShader.loadVertexShader("lighting.vsh");
    m_lightingShader.loadFragmentShader("lighting.fsh");
    m_lightingShader.linkProgram();
    m_lightingShader.useProgram();
    glUniform1i(m_lightingShader.getUniformLocation("u_shadowMap"), 0);

    // Load the angel models
    m_angelModel1.loadOBJ("angel2.obj", m_lightingShader);
    m_angelModel2.loadOBJ("weeping_angel.obj", m_lightingShader);

    // Load the texture shader
    m_textureShader.loadVertexShader("texture.vsh");
    m_textureShader.loadFragmentShader("texture.fsh");
    m_textureShader.loadGeometryShader("texture.gsh");
    m_textureShader.linkProgram();
    m_textureShader.useProgram();
    glUniform1i(m_textureShader.getUniformLocation("u_ambientMap"), 0);
    glUniform1i(m_textureShader.getUniformLocation("u_normalMap"), 1);
    glUniform1i(m_textureShader.getUniformLocation("u_specularMap"), 2);
    glUniform1i(m_textureShader.getUniformLocation("u_shadowMap"), 3);

    // Load the unlit texture shader
    m_unlitShader.loadVertexShader("unlitTex.vsh");
    m_unlitShader.loadFragmentShader("unlitTex.fsh");
    m_unlitShader.linkProgram();
    m_unlitShader.useProgram();
    glUniform1i(m_unlitShader.getUniformLocation("u_ambientMap"), 0);
    glUniform1i(m_unlitShader.getUniformLocation("u_normalMap"), 1);
    glUniform1i(m_unlitShader.getUniformLocation("u_specularMap"), 2);

    // Load the wall textures
    m_wallTextures[0].openFile("wall.tga");
    m_wallTextures[0].createTexture();
    m_wallTextures[1].openFile("wall_n.tga");
    m_wallTextures[1].createTexture();
    m_wallTextures[2].openFile("wall_s.tga");
    m_wallTextures[2].createTexture();

    // Load dungeon stuff
    m_floorTextures[0].openFile("floor.tga");
    m_floorTextures[0].createTexture();
    m_floorTextures[1].openFile("floor_n.tga");
    m_floorTextures[1].createTexture();
    m_floorTextures[2].openFile("floor_s.tga");
    m_floorTextures[2].createTexture();

    // Load the wall and floor models
    m_wallModel.loadOBJ("wall.obj", m_textureShader);
    m_floorModel.loadOBJ("floor.obj", m_textureShader);

    // Set up dungeon
    m_dungeon.setModels(&m_wallModel, &m_floorModel);
    m_dungeon.setTextures(m_wallTextures, m_floorTextures);
    m_dungeon.setShader(&m_textureShader);
    m_dungeon.generateMap(m_angels);

    // Set up angels
    Matrix4 weepingMatrix = Matrix4().scale(0.7f/145.0f) * Matrix4().translate(0.0f, 70.0f, 74.0f) * Matrix4().rotateX(90.0f);
    Matrix4 attackingMatrix = Matrix4().scale(0.7f/128.0f) * Matrix4().translate(0.0f, 64.0f, 0.0f);
    Angel::setMatrices(weepingMatrix, attackingMatrix);
    Angel::setModels(&m_angelModel1, &m_angelModel2);
    Angel::setShader(&m_lightingShader);
}

void GameObject::update(double elapsedTime)
{
    //m_dungeon.update(m_position);

    float cosYaw = cosf(m_yawAngle * 3.141592653589793f / 180.0f);
    float sinYaw = sinf(m_yawAngle * 3.141592653589793f / 180.0f);
    Vector3 direction = Vector3(0.0f);

    // Apply impulses from input
    if (m_isDownW)
        direction += Vector3(-sinYaw, 0.0f, -cosYaw);

    if (m_isDownS)
        direction += Vector3(sinYaw, 0.0f, cosYaw);

    if (m_isDownA)
        direction += Vector3(-cosYaw, 0.0f, sinYaw);

    if (m_isDownD)
        direction += Vector3(cosYaw, 0.0f, -sinYaw);

    // Normalize (so diagonal isn't faster) and scale by speed*time
    // NOTE: normalize() handles divide by zero case
    direction.normalize() *= 2.0f * elapsedTime;

    // Clip velocity by collision detection with dungeon and objects
    if (!m_noClip && direction.length() > 0.0f)
    {
        direction = m_dungeon.collide(m_position, direction, 0.2f);
        for (int i = 0; i < m_angels.size(); ++i)
            direction = m_angels[i].collide(m_position, direction, 0.15f);
        // collision w/ objects (angels, doors?)
    }

    m_position += direction;

    Vector3 facing = Vector3(-sinYaw, 0.0f, -cosYaw);
    for (int i = 0; i < m_angels.size(); ++i)
        m_angels[i].update(m_position, facing, 0.2f, m_angels, m_dungeon, elapsedTime);
}

void GameObject::renderShadow(Matrix4& proj, Matrix4& view, Matrix4& lightFV)
{
    m_shadowShader.useProgram();
    m_dungeon.render(proj, view, lightFV);
    for (int i = 0; i < m_angels.size(); ++i)
        m_angels[i].render(proj, view, lightFV);
}

void GameObject::renderObjects(Matrix4& proj, Matrix4& view, Matrix4& lightFV)
{
    m_dungeon.setShader(m_isLit ? &m_textureShader : &m_unlitShader);
    if (m_isLit)
    {
        m_textureShader.useProgram();
        glActiveTexture(GL_TEXTURE0 + 3);
        glBindTexture(GL_TEXTURE_2D, m_shadowTexture);
    }
    m_dungeon.render(proj, view, lightFV);

    m_lightingShader.useProgram();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_shadowTexture);

    for (int i = 0; i < m_angels.size(); ++i)
        m_angels[i].render(proj, view, lightFV);
}

void GameObject::render(Matrix4& frustum)
{
    Matrix4 viewRotation = Matrix4().rotateX(m_pitchAngle) * Matrix4().rotateY(m_yawAngle);
    Vector3 eyePosition = (m_position + Vector3(0.0f, 0.6f, 0.0f)).negate();
    Matrix4 view = viewRotation * Matrix4().translate(eyePosition);
    Matrix4 lightFrustum = Matrix4().projection(60.0f, 1.0f, 0.1f, 10.0f);
    Matrix4 lightView = view * Matrix4().translate(-0.1f, 0.1f, -0.1f);
    Matrix4 lightFV = lightFrustum * lightView;

    // Update flashlight position (in eye space)
    // NOTE: using constant flashlight position for now (relative to camera)
    /*lightingShader.useProgram();
    Vector3 lightPosition = view.multiply(Vector3(g_xPosition, 0.5f, g_zPosition), 1.0f);
    //Vector3 lightDirection = view.multiply(Vector3(), 0.0f).normalize();
    glUniform3fv(lightingShader.getUniformLocation("u_lightPos"), 1, lightPosition.getPointer());*/

    // First pass for shadow map
    glBindFramebuffer(GL_FRAMEBUFFER, m_shadowFramebuffer);
    glViewport(0, 0, 1024, 1024); // shadowmap size
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_FRONT);
    
    renderShadow(lightFrustum, lightView, lightFV);

    // Second pass
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, m_windowWidth, m_windowHeight); // window framebuffer size
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);
    
    renderObjects(frustum, view, lightFV);
}

void GameObject::render(StereoCamera& stereo)
{
    Matrix4 viewRotation = Matrix4().rotateX(m_pitchAngle) * Matrix4().rotateY(m_yawAngle);
    Vector3 eyePosition = (m_position + Vector3(0.0f, 0.6f, 0.0f)).negate();
    Matrix4 view = viewRotation * Matrix4().translate(eyePosition);
    Matrix4 lightFrustum = Matrix4().projection(60.0f, 1.0f, 0.1f, 10.0f);
    Matrix4 lightView = view * Matrix4().translate(-0.1f, 0.1f, -0.1f);
    Matrix4 lightFV = lightFrustum * lightView;

    // Prepare shadow buffer for rendering
    glBindFramebuffer(GL_FRAMEBUFFER, m_shadowFramebuffer);
    glViewport(0, 0, 1024, 1024); // shadowmap size
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_FRONT);
    
    // First pass for shadow map
    renderShadow(lightFrustum, lightView, lightFV);

    // Prepare screen buffer for rendering
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, m_windowWidth, m_windowHeight); // window framebuffer size
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);
    
    // Second pass for left eye
    glColorMask(true, false, false, false);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    renderObjects(stereo.getLeftMatrix(), view, lightFV);

    // Third pass for right eye
    glColorMask(false, true, true, true);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    renderObjects(stereo.getRightMatrix(), view, lightFV);
    
    glColorMask(true, true, true, true);
}
