#define GLFW_INCLUDE_GLCOREARB
#include "GLFW/glfw3.h"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <vector>
#include <cmath>
#include <memory>

#ifdef _WIN32
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "opengl32.lib")
#endif

#include "ShaderProgram.h"
#include "Frustum.h"
#include "GameObject.h"

void errorCallback(int error, const char* description);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouseCallback(GLFWwindow *window, double xpos, double ypos);
void buttonCallback(GLFWwindow *window, int button, int action, int mods);
void resizeCallback(GLFWwindow *window, int width, int height);

// Global matrix objects
Frustum g_frustum;

std::unique_ptr<GameObject> g_gameObject;

// Global mouse/button/key/window state
float g_lastMouseX, g_lastMouseY;
bool g_mouseRotate = false;
bool g_mouseStarted = false;

int main(int argc, char* argv[])
{
    GLFWwindow* window;

    try
    {
        // Init GLFW
        glfwSetErrorCallback(errorCallback);
        if (!glfwInit())
            throw std::runtime_error("Failed to init GLFW");

        // Use OpenGL Core 4.1
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, 1);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        // Create window
        window = glfwCreateWindow(512, 512, "Don't Blink", NULL, NULL);
        if (window == nullptr)
            throw std::runtime_error("Failed to create window");
        glfwMakeContextCurrent(window);

        #if 0
        std::cout << "GL Context: " << glGetString(GL_VERSION) << std::endl;
        std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
        #endif

        // Allow to run faster than vsync
        //glfwSwapInterval(0);

        // Set GLFW callbacks
        glfwSetKeyCallback(window, keyCallback);
        glfwSetCursorPosCallback(window, mouseCallback);
        glfwSetMouseButtonCallback(window, buttonCallback);
        glfwSetFramebufferSizeCallback(window, resizeCallback);
        //glfwSetWindowSizeCallback(window, resizeCallback);

        // Set up OpenGL properties
        glEnable(GL_PRIMITIVE_RESTART);
        glPrimitiveRestartIndex(0xFFFFFFFF);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        // Initialize game object
        g_gameObject = std::unique_ptr<GameObject>(new GameObject());

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        resizeCallback(window, width, height);
        
        StereoCamera stereo(.33f, .206f, .0065f);
        //stereo.makeFrustum(.30f, .75f, 100.f);
        stereo.makeFrustum(.10f, .33f, 100.f);

        // Main loop
        while (!glfwWindowShouldClose(window))
        {
            // Get time in seconds since last update
            static double lastTime = glfwGetTime();
            double currentTime = glfwGetTime();
            double elapsedTime = currentTime - lastTime;
            lastTime = currentTime;
            // NOTE: 1/elapsedTime can be used for instantaneous FPS

            // Update game object
            g_gameObject->update(elapsedTime);
            //g_gameObject->render(g_frustum.getMatrix());
            g_gameObject->render(stereo);

            #if 0
            static int frameCount = 0;
            static double frameTime = 0.0;
            frameTime += elapsedTime;
            frameCount++;
            if (frameTime > 2.0)
            {
                std::cout << "FPS: " << frameCount/frameTime << std::endl;
                frameTime = 0.0;
                frameCount = 0;
            }
            #endif

            // Update window and events
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }
    catch (std::exception& err)
    {
        std::cerr << err.what() << std::endl;
    }

    glfwTerminate();

    return 0;
}

void errorCallback(int error, const char* description)
{
    std::cerr << "GLFW error (" << error << "): " << description << std::endl;
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    switch (key)
    {
    case GLFW_KEY_ESCAPE:
        glfwSetWindowShouldClose(window, GL_TRUE);
        break;
    case GLFW_KEY_1:
    case GLFW_KEY_2:
    case GLFW_KEY_3:
        if (action == GLFW_PRESS)
            g_gameObject->setModelIndex(key - GLFW_KEY_1);
        break;
    case GLFW_KEY_0:
        if (action == GLFW_PRESS)
        {
            static bool wireframeToggle = true;
            glPolygonMode(GL_FRONT_AND_BACK, wireframeToggle ? GL_LINE : GL_FILL);
            wireframeToggle = !wireframeToggle;
        }
        break;
    case GLFW_KEY_Q:
        if (action == GLFW_PRESS)
            g_gameObject->toggleIsLit();
        break;
    case GLFW_KEY_E:
        if (action == GLFW_PRESS)
            g_gameObject->toggleNoClip();
        break;
    case GLFW_KEY_R:
        if (action == GLFW_PRESS)
            g_gameObject->regenDungeon();
        break;
    case GLFW_KEY_W:
        g_gameObject->setDownW(action != GLFW_RELEASE);
        break;
    case GLFW_KEY_S:
        g_gameObject->setDownS(action != GLFW_RELEASE);
        break;
    case GLFW_KEY_A:
        g_gameObject->setDownA(action != GLFW_RELEASE);
        break;
    case GLFW_KEY_D:
        g_gameObject->setDownD(action != GLFW_RELEASE);
        break;
    }
}

void mouseCallback(GLFWwindow *window, double xpos, double ypos)
{
    if (g_mouseRotate)
    {
        //int width, height;
        //glfwGetWindowSize(window, &width, &height);

        if (g_mouseStarted)
        {
            g_gameObject->lookYaw(0.05f * (g_lastMouseX - xpos));
            g_gameObject->lookPitch(0.05f * (g_lastMouseY - ypos));
        }
        else
            g_mouseStarted = true; // drop first mouse event to prevent skipping

        g_lastMouseX = xpos;
        g_lastMouseY = ypos;
    }
}

void buttonCallback(GLFWwindow *window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS)
        {
            if (g_mouseRotate)
            {
                // Release cursor from mouse-look mode
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                g_mouseRotate = false;
            }
            else
            {
                // Grab/hide cursor for mouse-look mode
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                g_mouseRotate = true;
                g_mouseStarted = false;
            }
        }
    }
}

void resizeCallback(GLFWwindow *window, int width, int height)
{
    // Get the framebuffer size, not the window size!
    //glfwGetFramebufferSize(window, &width, &height);

    //std::cout << "Resize: " << width << "x" << height << std::endl;
    if (g_gameObject)
        g_gameObject->resize(width, height);
    g_frustum.setProjection(60.0f, width, height, 0.1f, 500.0f);
}
