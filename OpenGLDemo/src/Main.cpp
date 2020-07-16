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
#include <algorithm>


float clamp(float n, float lower, float upper) 
{
    return std::max(lower, std::min(n, upper));
}


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
    width = 1920.0f;
    height = 1080.0f;

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

    glm::vec3 cubePositions[5]
    {
        glm::vec3(10.0f, 1.0f, 2.0f),
        glm::vec3(-4.0f, 0.0f, 1.0f),
        glm::vec3(2.0f, -1.0f, -5.5f),
        glm::vec3(-1.0f, -4.0f, -1.5f),
        glm::vec3(1.0f, 4.0f, -1.5f),
    };

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

    glm::vec3 pointLightPositions[4]
    {
        glm::vec3(1.0f, 1.0f, 1.5f),
        glm::vec3(-1.0f, 1.0f, 1.5f),
        glm::vec3(1.0f, -1.0f, -1.5f),
        glm::vec3(-1.0f, -1.0f, -1.5f),
    };

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

    glm::mat4 cubeModelMatrices[5];
    for (int i = 0; i < 5; ++i)
    {
        cubePositions[i] *= 1.0f;
        cubeModelMatrices[i] = glm::translate(glm::mat4(1.0f), cubePositions[i]);
        cubeModelMatrices[i] = glm::scale(cubeModelMatrices[i], glm::vec3(1.0f));
    }

    glm::mat4 pointLightModelMatrices[4];
    for (int i = 0; i < 4; ++i)
    {
        pointLightPositions[i] *= 5.0f;
        pointLightModelMatrices[i] = glm::translate(glm::mat4(1.0f), pointLightPositions[i]);
        pointLightModelMatrices[i] = glm::scale(pointLightModelMatrices[i], glm::vec3(0.4f));
    }


    //For free camera

    glm::vec3 cameraInitialPosition(0.0f, 0.0f, 4.0f);

    PerspectiveCamera camera(glm::radians(60.0f), width / height, 0.001f, 100.0f, cameraInitialPosition, cameraInitialPosition + glm::vec3(0.0f, 0.0f, -1.0f));
    FreeCamera freeCamera(camera, width, height);

    //Get ready to render
    Program programLighting("res/shaders/Lighting.shader");
    Program programLightSource("res/shaders/LightSource.shader");
    
    //Get diffuse and specular maps

    Texture diffuseMap("res/textures/container2.png");
    diffuseMap.bind(0);

    Texture specularMap("res/textures/container2Specular.png");
    specularMap.bind(1);
    
    float rValue, gValue, bValue, camX, camZ, x = 0.0f, delta = 0.5f, radius = 10.0f;

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

        glm::mat4 viewMatrix = freeCamera.getViewMatrix();
        glm::mat4 projectionMatrix = freeCamera.getProjectionMatrix();

        //DRAW CUBE
        programLighting.bind();

        programLighting.setUniformMat4f("u_modelMatrix", modelMatrixCube);
        programLighting.setUniformMat4f("u_viewMatrix", viewMatrix);
        programLighting.setUniformMat4f("u_projectionMatrix", projectionMatrix);

        //Set material for cube
        programLighting.setUniform1f("u_material.shininess", 32.0f);
        programLighting.setUniform1i("u_material.diffuse", 0);
        programLighting.setUniform1i("u_material.specular", 1);

        //SET LIGHT UNIFORMS

        glm::vec3 lightColour(1.0f);
        /*lightColour.x = (sin(x * 2.0f) + 1.0f) * 0.5f;
        lightColour.y = (sin(x * 0.7f) + 1.0f) * 0.5f;
        lightColour.z = (sin(x * 1.3f) + 1.0f) * 0.5f;*/
        
        glm::vec3 ambient = glm::vec3(0.2f) * lightColour;
        glm::vec3 diffuse = glm::vec3(0.5f) * lightColour;


        //Set uniforms for directional light
        programLighting.setUniform3f("u_directionalLight.direction", -0.2f, -1.0f, -0.3f);


        programLighting.setUniform3f("u_directionalLight.ambient", ambient.x, ambient.y, ambient.z);
        programLighting.setUniform3f("u_directionalLight.diffuse", diffuse.x, diffuse.y, diffuse.z);
        programLighting.setUniform3f("u_directionalLight.specular", 1.0f, 1.0f, 1.0f);

        //Set uniforms for 4 point lights
        
        for (int i = 0; i < 4; ++i)
        {
            programLighting.setUniform3f("u_pointLight[" + std::to_string(i) + "].position", pointLightPositions[i].x, pointLightPositions[i].y, pointLightPositions[i].z);

            programLighting.setUniform3f("u_pointLight[" + std::to_string(i) + "].ambient", ambient.x, ambient.y, ambient.z);
            programLighting.setUniform3f("u_pointLight[" + std::to_string(i) + "].diffuse", diffuse.x, diffuse.y, diffuse.z);
            programLighting.setUniform3f("u_pointLight[" + std::to_string(i) + "].specular", 1.0f, 1.0f, 1.0f);
                                                       
            programLighting.setUniform1f("u_pointLight[" + std::to_string(i) + "].constantTerm", 1.0f);
            programLighting.setUniform1f("u_pointLight[" + std::to_string(i) + "].linearTerm", 0.09f);
            programLighting.setUniform1f("u_pointLight[" + std::to_string(i) + "].quadraticTerm", 0.032f);
        }

        //Set uniforms for spotlight coming from the camera
        glm::vec3 cameraPosition = freeCamera.getPerspectiveCamera().getPosition();
        glm::vec3 cameraDirection = freeCamera.getDirection();

        programLighting.setUniform3f("u_spotLight.position", cameraPosition.x, cameraPosition.y, cameraPosition.z);
        programLighting.setUniform3f("u_spotLight.direction", cameraDirection.x, cameraDirection.y, cameraDirection.z);

        programLighting.setUniform3f("u_spotLight.ambient", ambient.x, ambient.y, ambient.z);
        programLighting.setUniform3f("u_spotLight.diffuse", diffuse.x, diffuse.y, diffuse.z);
        programLighting.setUniform3f("u_spotLight.specular", 1.0f, 1.0f, 1.0f);

        programLighting.setUniform1f("u_spotLight.constantTerm", 1.0f);
        programLighting.setUniform1f("u_spotLight.linearTerm", 0.09f);
        programLighting.setUniform1f("u_spotLight.quadraticTerm", 0.032f);

        programLighting.setUniform1f("u_spotLight.cutoff", glm::cos(glm::radians(15.5f)));
        programLighting.setUniform1f("u_spotLight.outerCutoff", glm::cos(glm::radians(21.5f)));
        
        //Set camera position
        programLighting.setUniform3f("u_cameraPosition", cameraPosition.x, cameraPosition.y, cameraPosition.z);
        programLighting.setUniform3f("u_cameraDirection", cameraDirection.x, cameraDirection.y, cameraDirection.z);

        //Draw the four point lights
        for (int i = 0; i < 4; ++i)
        {
            programLighting.setUniformMat4f("u_modelMatrix", modelMatrixCube);
            renderer.draw(vertexArrayCube, indexBufferCube, programLighting);
        }

        //DRAW LIGHT
        programLightSource.bind();

        programLightSource.setUniform4f("u_lightColour", lightColour.x, lightColour.y, lightColour.z, 1.0f);

        programLightSource.setUniformMat4f("u_viewMatrix", viewMatrix);
        programLightSource.setUniformMat4f("u_projectionMatrix", projectionMatrix);

        //Draw the four point lights
        for (int i = 0; i < 4; ++i)
        {
            programLightSource.setUniformMat4f("u_modelMatrix", pointLightModelMatrices[i]);
            renderer.draw(vertexArrayLight, indexBufferLight, programLightSource);
        }

        x += delta * deltaFrameTime;

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