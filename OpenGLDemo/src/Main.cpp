#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "importer.hpp"

#include "Renderer.h"
#include "Program.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Texture.h"
#include "PerspectiveCamera.h"
#include "FreeCamera.h"
#include "Framebuffer.h"
#include "Cubemap.h"
#include "DepthMap.h"

#include "glm/glm.hpp"
#include "glm/gtx/string_cast.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <iostream>
#include <math.h>
#include <algorithm>
#include <array>


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

    Framebuffer::FramebufferSpecification specification;
    specification.width = (uint32_t)width;
    specification.height = (uint32_t)height;
    specification.samples = 8;

    Framebuffer framebuffer(specification);
    framebuffer.bind();


    Framebuffer::FramebufferSpecification intermediateSpecification;
    intermediateSpecification.width = (uint32_t)width;
    intermediateSpecification.height = (uint32_t)height;
    intermediateSpecification.samples = 1;

    Framebuffer intermediateFramebuffer(intermediateSpecification);


    DepthMap depthMap(1024, 1024);

    //Set up glfw mouse input
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    //Set frame rate of window
    glfwSwapInterval(1);

    //DEFINE PLANE TO RENDER FRAMEBUFFER COLOUR TEXTURE TO
    float vertexDataSquare[] =
    {
        -1.0f, -1.0f,  0.0f, 0.0f, //bottom-left
         1.0f,  1.0f,  1.0f, 1.0f, //top-right
        -1.0f,  1.0f,  0.0f, 1.0f, //top-left
         1.0f, -1.0f,  1.0f, 0.0f  //bottom-right
    };

    uint32_t indexesSquare[] =
    {
        0, 1, 2,
        0, 3, 1
    };

    auto vertexBufferSquare = std::make_shared<VertexBuffer>(vertexDataSquare, 4 * 4 * sizeof(float));
    VertexBufferLayout squareLayout;
    squareLayout.push<float>(2);
    squareLayout.push<float>(2);
    VertexArray vertexArraySquare;
    vertexArraySquare.addBuffer(vertexBufferSquare, squareLayout);
    IndexBuffer indexBufferSquare(indexesSquare, 6);
    

    float vertexDataCube[] =
    {
        // back face
       -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f,  0.0f, -1.0f,// bottom-left
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.0f,  0.0f, -1.0f,// bottom-right    
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  0.0f, -1.0f,// top-right              
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  0.0f, -1.0f,// top-right
       -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  0.0f, -1.0f,// top-left
       -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f,  0.0f, -1.0f,// bottom-left                
       // front face
       -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  0.0f,  1.0f,// bottom-left
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f,  0.0f,  1.0f,// top-right
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  0.0f,  1.0f,// bottom-right        
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f,  0.0f,  1.0f,// top-right
       -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  0.0f,  1.0f,// bottom-left
       -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  0.0f,  0.0f,  1.0f,// top-left        
       // left face
       -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -1.0f,  0.0f,  0.0f,// top-right
       -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, -1.0f,  0.0f,  0.0f,// bottom-left
       -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, -1.0f,  0.0f,  0.0f,// top-left       
       -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, -1.0f,  0.0f,  0.0f,// bottom-left
       -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -1.0f,  0.0f,  0.0f,// top-right
       -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, -1.0f,  0.0f,  0.0f,// bottom-right
       // right face
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  1.0f,  0.0f,  0.0f,// top-left
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  1.0f,  0.0f,  0.0f,// top-right      
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f,  0.0f,  0.0f,// bottom-right          
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f,  0.0f,  0.0f,// bottom-right
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  1.0f,  0.0f,  0.0f,// bottom-left
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  1.0f,  0.0f,  0.0f,// top-left
       // bottom face          
       -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f, -1.0f,  0.0f,// top-right
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, -1.0f,  0.0f,// bottom-left
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f,  0.0f, -1.0f,  0.0f,// top-left        
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, -1.0f,  0.0f,// bottom-left
       -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f, -1.0f,  0.0f,// top-right
       -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f, -1.0f,  0.0f,// bottom-right
       // top face
       -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  1.0f,  0.0f,// top-left
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  1.0f,  0.0f,// top-right
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  1.0f,  0.0f,// bottom-right                 
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  1.0f,  0.0f,// bottom-right
       -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  1.0f,  0.0f,// bottom-left  
       -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  1.0f,  0.0f // top-left
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

    //Enable face culling
    glEnable(GL_CULL_FACE);

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
    auto vertexBufferCube = std::make_shared<VertexBuffer>(vertexDataCube, 8 * 36 * sizeof(float));
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
        glm::vec3(1.0f, 1.0f, -1.5f),
        glm::vec3(-1.0f, 1.0f, -1.5f),
    };

    VertexArray vertexArrayLight;
    auto vertexBufferLight = std::make_shared<VertexBuffer>(vertexDataCube, 8 * 36 * sizeof(float));
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
        pointLightPositions[i] *= 2.0f;
        pointLightModelMatrices[i] = glm::translate(glm::mat4(1.0f), pointLightPositions[i]);
        pointLightModelMatrices[i] = glm::scale(pointLightModelMatrices[i], glm::vec3(0.4f));
    }


    //For free camera

    glm::vec3 cameraInitialPosition(0.0f, 0.0f, 4.0f);

    PerspectiveCamera camera(glm::radians(60.0f), width / height, 0.001f, 100.0f, cameraInitialPosition, cameraInitialPosition + glm::vec3(0.0f, 0.0f, -1.0f));
    FreeCamera freeCamera(camera, width, height);

    //Get ready to render
    Program programLighting("res/shaders/LightingModel.shader");
    Program programLightSource("res/shaders/LightSource.shader");
    Program programFramebuffer("res/shaders/Framebuffer.shader");
    Program programCubemap("res/shaders/Cubemap.shader");
    Program programDepthMap("res/shaders/SimpleDepth.shader");

    //Create cubemap
    std::array<std::string, 6> filePaths;
    filePaths[0] = "res/textures/cubemaps/skybox/right.jpg";
    filePaths[1] = "res/textures/cubemaps/skybox/left.jpg";
    filePaths[2] = "res/textures/cubemaps/skybox/top.jpg";
    filePaths[3] = "res/textures/cubemaps/skybox/bottom.jpg";
    filePaths[4] = "res/textures/cubemaps/skybox/front.jpg";
    filePaths[5] = "res/textures/cubemaps/skybox/back.jpg";

    Cubemap cubemap(filePaths);

    float vertexDataCubemap[] =
    {
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    auto vertexBufferCubemap = std::make_shared<VertexBuffer>(vertexDataCubemap, 3 * 36 * sizeof(float));
    VertexBufferLayout vertexBufferCubemapLayout;
    vertexBufferCubemapLayout.push<float>(3);
    VertexArray vertexArrayCubemap;
    vertexArrayCubemap.addBuffer(vertexBufferCubemap, vertexBufferCubemapLayout);

    IndexBuffer indexBufferCubemap(triangleIndexesCube, 36);
    
    //Get diffuse and specular maps

    Texture diffuseMap("res/textures/container2.png", Texture::Type::DIFFUSE);
    diffuseMap.bind(0);

    Texture specularMap("res/textures/container2Specular.png", Texture::Type::SPECULAR);
    specularMap.bind(1);
    
    float rValue, gValue, bValue, camX, camZ, x = 0.0f, delta = 0.5f, radius = 10.0f;

    float currentFrame, deltaFrameTime = 0.0f, lastFrame = 0.0f;

    Renderer renderer;

    Model backpack("res/models/backpack/backpack.obj");
    glm::mat4 backpackViewMatrix = glm::scale(glm::mat4(1.0f), { 0.2f, 0.2f, 0.2f });

    Model woodenFloor("res/models/woodenFloor/woodenFloor.obj");
    glm::mat4 woodenFloorViewMatix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        currentFrame = glfwGetTime();
        deltaFrameTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window, deltaFrameTime, freeCamera);

        //Render to depth map
        depthMap.bind();
        glClear(GL_DEPTH_BUFFER_BIT);
        glm::mat4 lightProjectionMatrix = glm::perspective(glm::radians(60.0f), 1.0f, 0.001f, 100.0f);
        glm::mat4 lightViewMatrix = glm::lookAt(pointLightPositions[0], glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 lightViewProjectionMatrix = lightProjectionMatrix * lightViewMatrix;
        programDepthMap.bind();
        programDepthMap.setUniformMat4f("u_lightViewProjectionMatrix", lightViewProjectionMatrix);


        //render scene for depth map
        programDepthMap.setUniformMat4f("u_modelMatrix", backpackViewMatrix);
        renderer.draw(backpack, programDepthMap);

        programDepthMap.setUniformMat4f("u_modelMatrix", woodenFloorViewMatix);
        renderer.draw(woodenFloor, programDepthMap);

        /* Render here */
        framebuffer.bind();
        renderer.clear();

        glm::mat4 viewMatrix = freeCamera.getViewMatrix();
        glm::mat4 projectionMatrix = freeCamera.getProjectionMatrix();

        programLighting.bind();
        
        //Set uniform for shadow mapping
        programLighting.setUniformMat4f("u_lightViewProjectionMatrix", lightViewProjectionMatrix);

        depthMap.bindDepthAttachment(10);
        programLighting.setUniform1i("u_shadowMap", 10);

        //DRAW BACKPACK

        programLighting.setUniformMat4f("u_modelMatrix", backpackViewMatrix);
        programLighting.setUniformMat4f("u_viewMatrix", viewMatrix);
        programLighting.setUniformMat4f("u_projectionMatrix", projectionMatrix);

        //Set material for backpack
        programLighting.setUniform1f("u_material.shininess", 32.0f);

        //Set environment reflection and refraction uniforms
        cubemap.bind(5);
        programLighting.setUniform1i("u_environment", 5);
        programLighting.setUniform1f("u_refractiveIndexRatio", 1.0f / 1.52f);

        //SET LIGHT UNIFORMS

        glm::vec3 lightColour(1.0f);
        /*lightColour.x = (sin(x * 2.0f) + 1.0f) * 0.5f;
        lightColour.y = (sin(x * 0.7f) + 1.0f) * 0.5f;
        lightColour.z = (sin(x * 1.3f) + 1.0f) * 0.5f;*/
        
        glm::vec3 ambient = glm::vec3(0.2f) * lightColour;
        glm::vec3 diffuse = glm::vec3(0.5f) * lightColour;


        //Set uniforms for directional light
        //programLighting.setUniform3f("u_directionalLight.direction", -0.2f, -1.0f, -0.3f);


        ///*programLighting.setUniform3f("u_directionalLight.ambient", ambient.x, ambient.y, ambient.z);
        //programLighting.setUniform3f("u_directionalLight.diffuse", diffuse.x, diffuse.y, diffuse.z);
        //programLighting.setUniform3f("u_directionalLight.specular", 1.0f, 1.0f, 1.0f);*/

        //programLighting.setUniform3f("u_directionalLight.ambient", 0.0f, 0.0f, 0.0f);
        //programLighting.setUniform3f("u_directionalLight.diffuse", 0.0f, 0.0f, 0.0f);
        //programLighting.setUniform3f("u_directionalLight.specular", 0.0f, 0.0f, 0.0f);

        //Set uniforms for 4 point lights
        
        /*for (int i = 0; i < 4; ++i)
        {
            programLighting.setUniform3f("u_pointLight[" + std::to_string(i) + "].position", pointLightPositions[i].x, pointLightPositions[i].y, pointLightPositions[i].z);

            programLighting.setUniform3f("u_pointLight[" + std::to_string(i) + "].ambient", ambient.x, ambient.y, ambient.z);
            programLighting.setUniform3f("u_pointLight[" + std::to_string(i) + "].diffuse", diffuse.x, diffuse.y, diffuse.z);
            programLighting.setUniform3f("u_pointLight[" + std::to_string(i) + "].specular", 1.0f, 1.0f, 1.0f);
                                                       
            programLighting.setUniform1f("u_pointLight[" + std::to_string(i) + "].constantTerm", 1.0f);
            programLighting.setUniform1f("u_pointLight[" + std::to_string(i) + "].linearTerm", 0.35f);
            programLighting.setUniform1f("u_pointLight[" + std::to_string(i) + "].quadraticTerm", 0.0f);
        }*/

        programLighting.setUniform3f("u_pointLight.position", pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);

        programLighting.setUniform3f("u_pointLight.ambient", ambient.x, ambient.y, ambient.z);
        programLighting.setUniform3f("u_pointLight.diffuse", diffuse.x, diffuse.y, diffuse.z);
        programLighting.setUniform3f("u_pointLight.specular", 1.0f, 1.0f, 1.0f);

        programLighting.setUniform1f("u_pointLight.constantTerm", 1.0f);
        programLighting.setUniform1f("u_pointLight.linearTerm", 0.25f);
        programLighting.setUniform1f("u_pointLight.quadraticTerm", 0.0f);

        //Set uniforms for spotlight coming from the camera
        glm::vec3 cameraPosition = freeCamera.getPerspectiveCamera().getPosition();
        glm::vec3 cameraDirection = freeCamera.getDirection();

       /*programLighting.setUniform3f("u_spotLight.position", cameraPosition.x, cameraPosition.y, cameraPosition.z);
        programLighting.setUniform3f("u_spotLight.direction", cameraDirection.x, cameraDirection.y, cameraDirection.z);

        programLighting.setUniform3f("u_spotLight.ambient", ambient.x, ambient.y, ambient.z);
        programLighting.setUniform3f("u_spotLight.diffuse", diffuse.x, diffuse.y, diffuse.z);
        programLighting.setUniform3f("u_spotLight.specular", 1.0f, 1.0f, 1.0f);

        programLighting.setUniform1f("u_spotLight.constantTerm", 1.0f);
        programLighting.setUniform1f("u_spotLight.linearTerm", 0.22f);
        programLighting.setUniform1f("u_spotLight.quadraticTerm", 0.0f);

        programLighting.setUniform1f("u_spotLight.cutoff", glm::cos(glm::radians(15.5f)));
        programLighting.setUniform1f("u_spotLight.outerCutoff", glm::cos(glm::radians(21.5f)));*/
        
        //Set camera position
        programLighting.setUniform3f("u_cameraPosition", cameraPosition.x, cameraPosition.y, cameraPosition.z);
        programLighting.setUniform3f("u_cameraDirection", cameraDirection.x, cameraDirection.y, cameraDirection.z);

        //Draw model
        renderer.draw(backpack, programLighting);

        //DRAW WOODEN FLOOR
        programLighting.bind();

        programLighting.setUniformMat4f("u_modelMatrix", woodenFloorViewMatix);

        //Set material for wooden floor
        programLighting.setUniform1f("u_material.shininess", 0.5f);

        renderer.draw(woodenFloor, programLighting);

        //DRAW LIGHT
        programLightSource.bind();

        programLightSource.setUniform4f("u_lightColour", lightColour.x, lightColour.y, lightColour.z, 1.0f);

        programLightSource.setUniformMat4f("u_viewMatrix", viewMatrix);
        programLightSource.setUniformMat4f("u_projectionMatrix", projectionMatrix);

        //Draw the four point lights
        /*for (int i = 0; i < 4; ++i)
        {
            programLightSource.setUniformMat4f("u_modelMatrix", pointLightModelMatrices[i]);
            renderer.draw(vertexArrayLight, indexBufferLight, programLightSource);
        }*/

        programLightSource.setUniformMat4f("u_modelMatrix", pointLightModelMatrices[0]);
        renderer.draw(vertexArrayLight, indexBufferLight, programLightSource);

        //DRAW CUBEMAP
        glDepthFunc(GL_LEQUAL);
        cubemap.bind();

        programCubemap.bind();

        programCubemap.setUniform1i("u_cubemap", 0);
        glm::mat4 viewWithNoTranslation = glm::mat4(glm::mat3(viewMatrix));
        programCubemap.setUniformMat4f("u_viewMatrix", viewWithNoTranslation);
        programCubemap.setUniformMat4f("u_projectionMatrix", projectionMatrix);

        renderer.draw(vertexArrayCubemap, indexBufferCubemap, programCubemap);
        glDepthFunc(GL_LESS);

        //DRAW COLOUR ATTACHMENT TEXTURE OF FRAMEBUFFER TO SCREEN
        framebuffer.blit(intermediateFramebuffer);
        framebuffer.unbind();
        intermediateFramebuffer.unbind();

        renderer.clear();
        intermediateFramebuffer.bindColourAttachment(0);

        programFramebuffer.bind();
        programFramebuffer.setUniform1i("u_framebufferTexture", 0);

        renderer.draw(vertexArraySquare, indexBufferSquare, programFramebuffer);

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