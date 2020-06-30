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
    width = 680.0f;
    height = 420.0f;

    window = glfwCreateWindow(width, height, "Demo", NULL, NULL);

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

    float vertexDataCube[] =
    {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  1.0f,  0.0f
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

    //SET UP DATA FOR CUBE

    VertexArray vertexArrayCube;

    //Initialise vertex buffer with data
    VertexBuffer vertexBufferCube(vertexDataCube, 8 * 36 * sizeof(float));
    //Give attributes of each vertex
    VertexBufferLayout vertexBufferLayoutCube;
    //Vertex coordinates
    vertexBufferLayoutCube.push<float>(3);
    //Vertex texture coordinates
    vertexBufferLayoutCube.push<float>(2);
    //Vertex normal coordinates
    vertexBufferLayoutCube.push<float>(3);
    vertexArrayCube.addBuffer(vertexBufferCube, vertexBufferLayoutCube);

    //Initialise index buffer with data
    IndexBuffer indexBufferCube(triangleIndexesCube, 3 * 2 * 6);

    //SET UP DATA FOR LIGHT
    VertexArray vertexArrayLight;
    VertexBuffer vertexBufferLight(vertexDataCube, 8 * 36 * sizeof(float));
    VertexBufferLayout vertexBufferLayoutLight;
    vertexBufferLayoutLight.push<float>(3);
    vertexBufferLayoutLight.push<float>(2);
    vertexBufferLayoutLight.push<float>(3);
    vertexArrayLight.addBuffer(vertexBufferLight, vertexBufferLayoutLight);
    IndexBuffer indexBufferLight(triangleIndexesCube, 3 * 2 * 6);

    //Set up Model matrices
    glm::mat4 modelMatrixCube = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
    glm::vec3 lightPosition(1.0f, 1.0f, 1.5f);
    glm::mat4 modelMatrixLight = glm::translate(glm::mat4(1.0f), lightPosition);
    modelMatrixLight = glm::scale(modelMatrixLight, glm::vec3(0.4f));


    //For free camera

    glm::vec3 cameraInitialPosition(0.0f, 0.0f, 4.0f);

    PerspectiveCamera camera(glm::radians(60.0f), width / height, 0.001f, 100.0f, cameraInitialPosition, cameraInitialPosition + glm::vec3(0.0f, 0.0f, -1.0f));
    FreeCamera freeCamera(camera, width, height);

    //Get ready to render
    Program programLighting("res/shaders/Lighting.shader");
    Program programLightSource("res/shaders/LightSource.shader");
    

    /*Texture texture("res/textures/wood.jpg");
    texture.bind(0);
    program.setUniform1i("u_Texture", 0);*/
    
    //program.setUniformMat4f("u_MVP", MVP);

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

        /*rValue = (sin(x) + 1.0f) * 0.5f;
        gValue = (sin(x + 1.0f) + 1.0f) * 0.5f;
        bValue = (sin(x + 2.0f) + 1.0f) * 0.5f;*/

        //camX = sin(x * 0.2) * radius;
        //camZ = cos(x * 0.2) * radius;

        glm::mat4 viewMatrix = freeCamera.getViewMatrix();
        glm::mat4 projectionMatrix = freeCamera.getProjectionMatrix();

        //Draw cube
        programLighting.bind();

        programLighting.setUniformMat4f("u_modelMatrix", modelMatrixCube);
        programLighting.setUniformMat4f("u_viewMatrix", viewMatrix);
        programLighting.setUniformMat4f("u_projectionMatrix", projectionMatrix);

        programLighting.setUniform4f("u_objectColour", 1.0f, 0.5f, 0.31f, 1.0f);
        programLighting.setUniform4f("u_lightColour", 1.0f, 1.0f, 1.0f, 1.0f);
        programLighting.setUniform3f("u_lightPosition", lightPosition.x, lightPosition.y, lightPosition.z);
        glm::vec3 cameraPosition = freeCamera.getPerspectiveCamera().getPosition();
        programLighting.setUniform3f("u_cameraPosition", cameraPosition.x, cameraPosition.y, cameraPosition.z);

        renderer.draw(vertexArrayCube, indexBufferCube, programLighting);

        //Draw light
        programLightSource.bind();

        programLightSource.setUniformMat4f("u_modelMatrix", modelMatrixLight);
        programLightSource.setUniformMat4f("u_viewMatrix", viewMatrix);
        programLightSource.setUniformMat4f("u_projectionMatrix", projectionMatrix);

        renderer.draw(vertexArrayLight, indexBufferLight, programLightSource);


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