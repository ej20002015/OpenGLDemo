#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include "Renderer.h"
#include "Program.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Texture.h"
#include "PerspectiveCamera.h"
#include "FreeCamera.h"

#include "glm/glm.hpp"
#include "glm/gtx/string_cast.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <iostream>
#include <math.h>

void processInput(GLFWwindow* window, const float deltaTime, FreeCamera& freeCamera);

int main()
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    float height = mode->height;
    float width = mode->width;

    window = glfwCreateWindow(width, height, "Demo", monitor, NULL);

    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    GLenum initStatus = glewInit();

    if (initStatus != GLEW_OK)
    {
        std::cerr << "ERROR: " << glewGetErrorString(initStatus) << std::endl;
        return 1;
    }

    std::cout << "GL VERSION: " << glGetString(GL_VERSION) << std::endl;

    Renderer::initDebug();

    //Set up glfw mouse input
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    //Set frame rate of window
    glfwSwapInterval(1);

    float vertexDataSquare[] =
    {
        -0.5f,  0.5f, 0.0f, 1.0f,
         0.5f,  0.5f, 1.0f, 1.0f,
        -0.5f, -0.5f, 0.0f, 0.0f,
         0.5f, -0.5f, 1.0f, 0.0f
    };

    float vertexDataCube[] =
    {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    uint32_t triangleIndexesSquare[] =
    {
        0, 1, 2,
        0, 2, 3
    };

    uint32_t triangleIndexesCube[] =
    {
        0, 1, 2,
        3, 4, 5,

        6, 7, 8,
        9, 10, 11,

        12, 13, 14,
        15, 16, 17,

        18, 19, 20,
        21, 22, 23,

        24, 25, 26,
        27, 28, 29,

        30, 31, 32,
        33, 34, 35
    };

    //Enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //Enable z-buffer
    glEnable(GL_DEPTH_TEST);

    VertexArray vertexArray;

    //Initialise vertex buffer with data
    VertexBuffer vertexBuffer(vertexDataCube, 5 * 36 * sizeof(float));
    //Give attributes of each vertex
    VertexBufferLayout vertexBufferLayout;
    //Vertex coordinates
    vertexBufferLayout.push<float>(3);
    //Vertex texture coordinates
    vertexBufferLayout.push<float>(2);
    vertexArray.addBuffer(vertexBuffer, vertexBufferLayout);

    //Initialise index buffer with data
    IndexBuffer indexBuffer(triangleIndexesCube, 3 * 2 * 6);

    //Set up MVP
    glm::mat4 modelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
    //modelMatrix = glm::rotate(modelMatrix, glm::radians(45.0f), glm::vec3(1.0f, 1.0f, 1.0f));


    //For free camera

    glm::vec3 cameraPosition(0.0f, 0.0f, 3.0f);

    PerspectiveCamera camera(glm::radians(60.0f), width / height, 0.001f, 100.0f, cameraPosition, cameraPosition + glm::vec3(0.0f, 0.0f, -1.0f));
    FreeCamera freeCamera(camera, width, height);

    glm::mat4 MVP = freeCamera.getProjectionViewMatrix() * modelMatrix;

    //Get ready to render

    Program program("res/shaders/Basic.shader");
    program.bind();

    Texture texture("res/textures/wood.jpg");
    texture.bind(0);
    program.setUniform1i("u_Texture", 0);
    program.setUniformMat4f("u_MVP", MVP);

    float rValue, gValue, bValue, camX, camZ, x = 0.0f, delta = 0.05f, radius = 10.0f;

    float currentFrame, deltaFrameTime = 0.0f, lastFrame = 0.0f;

    Renderer renderer;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {

        currentFrame = glfwGetTime();
        deltaFrameTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window, deltaFrameTime, freeCamera);

        /* Render here */
        renderer.clear();

        rValue = (sin(x) + 1.0f) * 0.5f;
        gValue = (sin(x + 1.0f) + 1.0f) * 0.5f;
        bValue = (sin(x + 2.0f) + 1.0f) * 0.5f;

        //camX = sin(x * 0.2) * radius;
        //camZ = cos(x * 0.2) * radius;

        MVP = freeCamera.getProjectionViewMatrix() * modelMatrix;

        program.setUniformMat4f("u_MVP", MVP);

        program.setUniform4f("u_Color", rValue, gValue, bValue, 1.0f);
        renderer.draw(vertexArray, indexBuffer, program);

        x += delta;

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window, const float deltaTime, FreeCamera& freeCamera)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        freeCamera.moveForward(deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        freeCamera.moveBackward(deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        freeCamera.strafeRight(deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        freeCamera.strafeLeft(deltaTime);

    double xPos, yPos;
    glfwGetCursorPos(window, &xPos, &yPos);
    freeCamera.changeCameraDirection(xPos, yPos);
}