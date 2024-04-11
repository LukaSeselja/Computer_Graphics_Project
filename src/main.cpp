#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void mouse_callback(GLFWwindow *window, double xpos, double ypos);

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

void processInput(GLFWwindow *window);

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);

unsigned int loadCubemap(vector<std::string> faces);
unsigned int loadTexture(const char *path);

// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

// camera

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

struct PointLight {
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct DirLight {
    glm::vec3 direction;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

struct SpotLight {
    glm::vec3 position;
    glm::vec3 direction;
    float cutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

struct ProgramState {
    glm::vec3 clearColor = glm::vec3(0);
    bool ImGuiEnabled = false;
    Camera camera;
    bool CameraMouseMovementUpdateEnabled = true;
    bool bloomFlag = true;

    glm::vec3 fieldPosition = glm::vec3(0.0f);
    float fieldScale = 0.4f;

    glm::vec3 cornPosition = glm::vec3(7.4f, 2.64f, -19.2f);
    float cornScale = 0.04f;

    glm::vec3 hayPosition = glm::vec3(22.0f, 1.2f, -0.8f);
    float hayScale = 0.005f;

    glm::vec3 hay2Position = glm::vec3(22.5f, 1.0f, -1.1f);

    glm::vec3 hay3Position = glm::vec3(22.75f, 1.0f, -2.8f);

    glm::vec3 tractorPosition = glm::vec3(22.0f, 0.9f, -11.0f);
    float tractorScale = 0.5f;

    glm::vec3 cabinPosition = glm::vec3(11.0f, 3.33f, -7.0f);
    float cabinScale = 5.0f;

    glm::vec3 hayPilePosition = glm::vec3(23.5f, 1.26f, -2.0f);
    float hayPileScale = 0.18f;

    glm::vec3 fencePosition = glm::vec3(12.5f, 1.05f, -15.3f);
    float fenceScale = 0.5f;

    glm::vec3 fence2Position = glm::vec3(10.55f, 1.05f, -15.4f);

    glm::vec3 fence3Position = glm::vec3(8.6f, 1.05f, -15.48f);

    glm::vec3 fence4Position = glm::vec3(6.03f, 1.05f, -14.61f);

    glm::vec3 fence5Position = glm::vec3(5.97f, 1.05f, -13.62f);

    glm::vec3 fence6Position = glm::vec3(8.44f, 1.05f, -12.5f);

    glm::vec3 fence7Position = glm::vec3(10.4f, 1.05f, -12.4f);

    glm::vec3 fence8Position = glm::vec3(12.35f, 1.05f, -12.3f);

    glm::vec3 fence9Position = glm::vec3(11.85f, 1.05f, -13.32f);

    glm::vec3 gatePosition = glm::vec3(12.7f, 1.1f, -14.93f);
    float gateScale = 0.21f;

    glm::vec3 waterBowlPosition = glm::vec3(8.0f, 1.14f, -15.8f);
    float waterBowlScale = 0.6f;

    glm::vec3 sheepPosition = glm::vec3(7.8f, 1.0f, -14.9f);
    float sheepScale = 0.8f;

    glm::vec3 sheep2Position = glm::vec3(11.0f, 1.05f, -14.5f);

    glm::vec3 waterTowerPosition = glm::vec3(27.5f, 3.4f, -11.0f);
    float waterTowerScale = 0.15f;

    glm::vec3 lampPosition = glm::vec3(11.45f, 2.2, -9.9);
    float lampScale = 0.28f;

    PointLight pointLight;
    DirLight dirLight;
    SpotLight spotLight;
    SpotLight spotLight1;

    ProgramState()
            : camera(glm::vec3(0.0f, 0.0f, 3.0f)) {}

    void SaveToFile(std::string filename);

    void LoadFromFile(std::string filename);
};

void ProgramState::SaveToFile(std::string filename) {
    std::ofstream out(filename);
    out << clearColor.r << '\n'
        << clearColor.g << '\n'
        << clearColor.b << '\n'
        << ImGuiEnabled << '\n'
        << camera.Position.x << '\n'
        << camera.Position.y << '\n'
        << camera.Position.z << '\n'
        << camera.Front.x << '\n'
        << camera.Front.y << '\n'
        << camera.Front.z << '\n';
}

void ProgramState::LoadFromFile(std::string filename) {
    std::ifstream in(filename);
    if (in) {
        in >> clearColor.r
           >> clearColor.g
           >> clearColor.b
           >> ImGuiEnabled
           >> camera.Position.x
           >> camera.Position.y
           >> camera.Position.z
           >> camera.Front.x
           >> camera.Front.y
           >> camera.Front.z;
    }
}

ProgramState *programState;

void DrawImGui(ProgramState *programState);

int main() {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // MSAA
    glfwWindowHint(GLFW_SAMPLES, 4);

    // glfw window creation
    // --------------------
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);
    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
//    stbi_set_flip_vertically_on_load(true);

    programState = new ProgramState;
    programState->LoadFromFile("resources/program_state.txt");
    if (programState->ImGuiEnabled) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    // Init Imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;



    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    // enable face culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glFrontFace(GL_CW);

    // build and compile shaders
    // -------------------------
    Shader ourShader("resources/shaders/model_lighting.vs", "resources/shaders/model_lighting.fs");
    Shader skyboxShader("resources/shaders/skybox.vs", "resources/shaders/skybox.fs");
    Shader blendingShader("resources/shaders/blending.vs", "resources/shaders/blending.fs");
    Shader hdrShader("resources/shaders/hdrShader.vs", "resources/shaders/hdrShader.fs");
    Shader blurShader("resources/shaders/blur.vs", "resources/shaders/blur.fs");
    Shader bloomShader("resources/shaders/bloom.vs", "resources/shaders/bloom.fs");

    float skyboxVertices[] = {
            // positions
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

    float transparentVertices[] = {
            // positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
            0.0f, 0.5f, 0.0f, 0.0f, 0.0f,
            0.0f, -0.5f, 0.0f, 0.0f, 1.0f,
            1.0f, -0.5f, 0.0f, 1.0f, 1.0f,

            0.0f, 0.5f, 0.0f, 0.0f, 0.0f,
            1.0f, -0.5f, 0.0f, 1.0f, 1.0f,
            1.0f, 0.5f, 0.0f, 1.0f, 0.0f
    };

    float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    };

    // load models
    // -----------
    Model fieldModel("resources/objects/field_and_garden/scene.gltf");
    fieldModel.SetShaderTextureNamePrefix("material.");

    // corn model
    Model cornModel("resources/objects/corn_corn_corn/scene.gltf");
    cornModel.SetShaderTextureNamePrefix("material.");

    // hay bale model
    Model hayModel("resources/objects/hay_bale/scene.gltf");
    hayModel.SetShaderTextureNamePrefix("material.");

    // tractor model
    Model tractorModel("resources/objects/New_holland_T7_Tractor_SF/New_holland_T7_Tractor_SF.obj");
    tractorModel.SetShaderTextureNamePrefix("material.");

    // cabin model
    Model cabinModel("resources/objects/barn/scene.gltf");
    cabinModel.SetShaderTextureNamePrefix("material.");

    // hay pile model
    Model hayPileModel("resources/objects/small_garden_hay/scene.gltf");
    hayPileModel.SetShaderTextureNamePrefix("material.");

    // fence model
    Model fenceModel("resources/objects/fence_wood/scene.gltf");
    fenceModel.SetShaderTextureNamePrefix("material.");

    // gate model
    Model gateModel("resources/objects/gate_wood/scene.gltf");
    gateModel.SetShaderTextureNamePrefix("material.");

    // water bowl model
    Model waterBowlModel("resources/objects/water_bowl/scene.gltf");
    waterBowlModel.SetShaderTextureNamePrefix("material.");

    // sheep model
    Model sheepModel("resources/objects/sheep/scene.gltf");
    sheepModel.SetShaderTextureNamePrefix("material.");

    // water tower model
    Model waterTowerModel("resources/objects/old_water_tower/scene.gltf");
    waterTowerModel.SetShaderTextureNamePrefix("material.");

    // wall lamp model
    Model lampModel("resources/objects/wall_lamp/scene.gltf");
    lampModel.SetShaderTextureNamePrefix("material.");

    // Point light
    PointLight& pointLight = programState->pointLight;
    pointLight.position = glm::vec3(11.75f, 2.4f, -9.5f);
    pointLight.ambient = glm::vec3(0.5f);
    pointLight.diffuse = glm::vec3(3.0f);
    pointLight.specular = glm::vec3(1.0f);

    pointLight.constant = 1.0f;
    pointLight.linear = 0.7f;
    pointLight.quadratic = 1.8f;

    // Directional light
    DirLight& dirLight = programState->dirLight;
    dirLight.direction = glm::vec3(-1.4f, -0.9f, -1.7f);
    dirLight.ambient = glm::vec3(0.3f);
    dirLight.diffuse = glm::vec3(0.4f);
    dirLight.specular = glm::vec3(0.5f);

    // Spotlight
    SpotLight& spotLight = programState->spotLight;
    spotLight.ambient = glm::vec3(1.0, 0.9, 0.6);
    spotLight.diffuse = glm::vec3(0.9, 0.8, 0.5);
    spotLight.specular = glm::vec3(1.0, 0.9, 0.6);
    spotLight.constant = 1.0f;
    spotLight.linear = 0.09f;
    spotLight.quadratic = 0.032f;
    spotLight.cutOff = glm::cos(glm::radians(12.5f));
    spotLight.outerCutOff = glm::cos(glm::radians(17.5f));

    SpotLight& spotLight1 = programState->spotLight1;
    spotLight1.ambient = glm::vec3(1.0, 0.9, 0.6);
    spotLight1.diffuse = glm::vec3(0.9, 0.8, 0.5);
    spotLight1.specular = glm::vec3(1.0, 0.9, 0.6);
    spotLight1.constant = 1.0f;
    spotLight1.linear = 0.09f;
    spotLight1.quadratic = 0.032f;
    spotLight1.cutOff = glm::cos(glm::radians(12.5f));
    spotLight1.outerCutOff = glm::cos(glm::radians(17.5f));

    // configure floating point framebuffer
    // ------------------------------------
    unsigned int hdrFBO;
    glGenFramebuffers(1, &hdrFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    // create 2 floating point color buffers (1 for normal rendering, other for brightness threshold values)
    unsigned int colorBuffers[2];
    glGenTextures(2, colorBuffers);
    for (unsigned int i = 0; i < 2; i++)
    {
        glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT - 65, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        // attach texture to framebuffer
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
    }
    // create depth buffer (renderbuffer)
    unsigned int rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
    unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);
    // finally check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // ping-pong-framebuffer for blurring
    unsigned int pingpongFBO[2];
    unsigned int pingpongColorbuffers[2];
    glGenFramebuffers(2, pingpongFBO);
    glGenTextures(2, pingpongColorbuffers);
    for (unsigned int i = 0; i < 2; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
        glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);
        // also check if framebuffers are complete (no need for depth buffer)
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Framebuffer not complete!" << std::endl;
    }

    // skybox VAO
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    // grass VAO
    unsigned int transparentVAO, transparentVBO;
    glGenVertexArrays(1, &transparentVAO);
    glGenBuffers(1, &transparentVBO);
    glBindVertexArray(transparentVAO);
    glBindBuffer(GL_ARRAY_BUFFER, transparentVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices), transparentVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);
    // setup plane VAO
    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    // load texture
    unsigned int transparentTexture = loadTexture(FileSystem::getPath("resources/textures/grass.png").c_str());

    // skybox textures
    stbi_set_flip_vertically_on_load(false);
    vector<std::string> faces
            {
                    FileSystem::getPath("resources/textures/skybox/right.jpg"),
                    FileSystem::getPath("resources/textures/skybox/left.jpg"),
                    FileSystem::getPath("resources/textures/skybox/top.jpg"),
                    FileSystem::getPath("resources/textures/skybox/bottom.jpg"),
                    FileSystem::getPath("resources/textures/skybox/front.jpg"),
                    FileSystem::getPath("resources/textures/skybox/back.jpg")
            };
    unsigned int cubemapTexture = loadCubemap(faces);

    vector<glm::vec3> vegetation
            {
                    glm::vec3(12.3f, 1.17f, -13.35f),
                    glm::vec3(12.1f, 1.17f, -13.36f),
                    glm::vec3(11.9f, 1.17f, -13.37f),
                    glm::vec3(11.7f, 1.17f, -13.38f),
                    glm::vec3(11.5f, 1.17f, -13.39f),
                    glm::vec3(11.3f, 1.17f, -13.4f),
                    glm::vec3(11.1f, 1.17f, -13.41f),
                    glm::vec3(10.9f, 1.17f, -13.42f),

                    glm::vec3(12.32f, 1.17f, -13.55f),
                    glm::vec3(12.34f, 1.17f, -13.75f),
                    glm::vec3(12.36f, 1.17f, -13.95f),
                    glm::vec3(12.38f, 1.17f, -14.15f),

                    glm::vec3(12.1f, 1.17f, -13.55f),
                    glm::vec3(11.9f, 1.17f, -13.56f),
                    glm::vec3(11.7f, 1.17f, -13.57f),
                    glm::vec3(11.5f, 1.17f, -13.58f),

                    glm::vec3(12.11f, 1.17f, -13.75f),
                    glm::vec3(11.91f, 1.17f, -13.76f),
            };

    // shader configuration
    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);

    blendingShader.use();
    blendingShader.setInt("texture1", 0);

    // draw in wireframe
//    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);


        // render
        // ------
        glClearColor(programState->clearColor.r, programState->clearColor.g, programState->clearColor.b, 1.0f);
        glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // don't forget to enable shader before setting uniforms
        ourShader.use();

        // Point light
        ourShader.setVec3("pointLight.position", pointLight.position);
        ourShader.setVec3("pointLight.ambient", pointLight.ambient);
        ourShader.setVec3("pointLight.diffuse", pointLight.diffuse);
        ourShader.setVec3("pointLight.specular", pointLight.specular);

        ourShader.setFloat("pointLight.constant", pointLight.constant);
        ourShader.setFloat("pointLight.linear", pointLight.linear);
        ourShader.setFloat("pointLight.quadratic", pointLight.quadratic);

        // Directional light
        ourShader.setVec3("dirLight.direction", dirLight.direction);
        ourShader.setVec3("dirLight.ambient", dirLight.ambient);
        ourShader.setVec3("dirLight.diffuse", dirLight.diffuse);
        ourShader.setVec3("dirLight.specular", dirLight.specular);

        // Spotlight
        ourShader.setVec3("spotLight.direction", -0.17f, -0.3f, 1.0f);
        ourShader.setVec3("spotLight.ambient", spotLight.ambient);
        ourShader.setVec3("spotLight.diffuse", spotLight.diffuse);
        ourShader.setVec3("spotLight.specular", spotLight.specular);
        ourShader.setFloat("spotLight.constant", spotLight.constant);
        ourShader.setFloat("spotLight.linear", spotLight.linear);
        ourShader.setFloat("spotLight.quadratic", spotLight.quadratic);
        ourShader.setFloat("spotLight.cutOff", spotLight.cutOff);
        ourShader.setFloat("spotLight.outerCutOff", spotLight.outerCutOff);
        ourShader.setVec3("spotLight.position", programState->tractorPosition + glm::vec3(0.0f, 0.9f, 2.3f));

        ourShader.setVec3("spotLight1.direction", -0.03f, -0.3f, 1.0f);
        ourShader.setVec3("spotLight1.ambient", spotLight1.ambient);
        ourShader.setVec3("spotLight1.diffuse", spotLight1.diffuse);
        ourShader.setVec3("spotLight1.specular", spotLight1.specular);
        ourShader.setFloat("spotLight1.constant", spotLight1.constant);
        ourShader.setFloat("spotLight1.linear", spotLight1.linear);
        ourShader.setFloat("spotLight1.quadratic", spotLight1.quadratic);
        ourShader.setFloat("spotLight1.cutOff", spotLight1.cutOff);
        ourShader.setFloat("spotLight1.outerCutOff", spotLight1.outerCutOff);
        ourShader.setVec3("spotLight1.position", programState->tractorPosition + glm::vec3(0.33f, 0.9f, 2.3f));


        ourShader.setVec3("viewPosition", programState->camera.Position);
        ourShader.setFloat("material.shininess", 32.0f);
        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(programState->camera.Zoom),
                                                (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = programState->camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        // render the loaded models
        // field
        glm::mat4 field = glm::mat4(1.0f);
        field = glm::translate(field, programState->fieldPosition); // translate it down so it's at the center of the scene
        field = glm::scale(field, glm::vec3(programState->fieldScale));    // it's a bit too big for our scene, so scale it down
        field = glm::rotate(field, glm::radians(272.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        ourShader.setMat4("model", field);
        fieldModel.Draw(ourShader);

        // corn corn corn
        float zRowCoord = 0.0f;
        float yRowCoord = 0.0f;
        for (int i = 0; i < 9; ++i) {
            for (int j = 0; j < 30; ++j) {
                if (i == 8 && j > 25)
                    continue;
                glm::mat4 corn = glm::mat4(1.0f);
                corn = glm::translate(corn, programState->cornPosition + glm::vec3(float(j), yRowCoord, zRowCoord + j * 0.082f));
                corn = glm::scale(corn, glm::vec3(programState->cornScale));
                corn = glm::rotate(corn, glm::radians(275.0f), glm::vec3(1.0f, 0.0f, 0.0f));
                ourShader.setMat4("model", corn);
                cornModel.Draw(ourShader);
            }
            zRowCoord -= 1.3f;
            yRowCoord += 0.02f;
        }

        // hay bale
        glm::mat4 hay = glm::mat4(1.0f);
        hay = glm::translate(hay, programState->hayPosition);
        hay = glm::scale(hay, glm::vec3(programState->hayScale));
        hay = glm::rotate(hay, glm::radians(60.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        hay = glm::rotate(hay, glm::radians(-30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        ourShader.setMat4("model", hay);
        hayModel.Draw(ourShader);

        glm::mat4 hay2 = glm::mat4(1.0f);
        hay2 = glm::translate(hay2, programState->hay2Position);
        hay2 = glm::scale(hay2, glm::vec3(programState->hayScale));
        hay2 = glm::rotate(hay2, glm::radians(33.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        ourShader.setMat4("model", hay2);
        hayModel.Draw(ourShader);

        glm::mat4 hay3 = glm::mat4(1.0f);
        hay3 = glm::translate(hay3, programState->hay3Position);
        hay3 = glm::scale(hay3, glm::vec3(programState->hayScale));
        hay3 = glm::rotate(hay3, glm::radians(-37.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        ourShader.setMat4("model", hay3);
        hayModel.Draw(ourShader);

        // tractor
        glm::mat4 tractor = glm::mat4(1.0f);
        tractor = glm::translate(tractor, programState->tractorPosition);
        tractor = glm::scale(tractor, glm::vec3(programState->tractorScale));
        tractor = glm::rotate(tractor, glm::radians(-6.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        ourShader.setMat4("model", tractor);
        tractorModel.Draw(ourShader);

        // barn
        glm::mat4 cabin = glm::mat4(1.0f);
        cabin = glm::translate(cabin, programState->cabinPosition);
        cabin = glm::scale(cabin, glm::vec3(programState->cabinScale));
        cabin = glm::rotate(cabin, glm::radians(85.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        ourShader.setMat4("model", cabin);
        cabinModel.Draw(ourShader);

        // hay pile
        glm::mat4 hayPile = glm::mat4(1.0f);
        hayPile = glm::translate(hayPile, programState->hayPilePosition);
        hayPile = glm::scale(hayPile, glm::vec3(programState->hayPileScale));
        hayPile = glm::rotate(hayPile, glm::radians(-30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        hayPile = glm::rotate(hayPile, glm::radians(-10.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        ourShader.setMat4("model", hayPile);
        hayPileModel.Draw(ourShader);

        // fences
        glm::mat4 fence = glm::mat4(1.0f);
        fence = glm::translate(fence, programState->fencePosition);
        fence = glm::scale(fence, glm::vec3(programState->fenceScale));
        fence = glm::rotate(fence, glm::radians(66.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        fence = glm::rotate(fence, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        ourShader.setMat4("model", fence);
        fenceModel.Draw(ourShader);

        glm::mat4 fence2 = glm::mat4(1.0f);
        fence2 = glm::translate(fence2, programState->fence2Position);
        fence2 = glm::scale(fence2, glm::vec3(programState->fenceScale));
        fence2 = glm::rotate(fence2, glm::radians(66.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        fence2 = glm::rotate(fence2, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        ourShader.setMat4("model", fence2);
        fenceModel.Draw(ourShader);

        glm::mat4 fence3 = glm::mat4(1.0f);
        fence3 = glm::translate(fence3, programState->fence3Position);
        fence3 = glm::scale(fence3, glm::vec3(programState->fenceScale));
        fence3 = glm::rotate(fence3, glm::radians(66.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        fence3 = glm::rotate(fence3, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        ourShader.setMat4("model", fence3);
        fenceModel.Draw(ourShader);

        glm::mat4 fence4 = glm::mat4(1.0f);
        fence4 = glm::translate(fence4, programState->fence4Position);
        fence4 = glm::scale(fence4, glm::vec3(programState->fenceScale));
        fence4 = glm::rotate(fence4, glm::radians(-25.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        fence4 = glm::rotate(fence4, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        ourShader.setMat4("model", fence4);
        fenceModel.Draw(ourShader);

        glm::mat4 fence5 = glm::mat4(1.0f);
        fence5 = glm::translate(fence5, programState->fence5Position);
        fence5 = glm::scale(fence5, glm::vec3(programState->fenceScale));
        fence5 = glm::rotate(fence5, glm::radians(-25.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        fence5 = glm::rotate(fence5, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        ourShader.setMat4("model", fence5);
        fenceModel.Draw(ourShader);

        glm::mat4 fence6 = glm::mat4(1.0f);
        fence6 = glm::translate(fence6, programState->fence6Position);
        fence6 = glm::scale(fence6, glm::vec3(programState->fenceScale));
        fence6 = glm::rotate(fence6, glm::radians(66.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        fence6 = glm::rotate(fence6, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        ourShader.setMat4("model", fence6);
        fenceModel.Draw(ourShader);

        glm::mat4 fence7 = glm::mat4(1.0f);
        fence7 = glm::translate(fence7, programState->fence7Position);
        fence7 = glm::scale(fence7, glm::vec3(programState->fenceScale));
        fence7 = glm::rotate(fence7, glm::radians(66.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        fence7 = glm::rotate(fence7, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        ourShader.setMat4("model", fence7);
        fenceModel.Draw(ourShader);

        glm::mat4 fence8 = glm::mat4(1.0f);
        fence8 = glm::translate(fence8, programState->fence8Position);
        fence8 = glm::scale(fence8, glm::vec3(programState->fenceScale));
        fence8 = glm::rotate(fence8, glm::radians(66.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        fence8 = glm::rotate(fence8, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        ourShader.setMat4("model", fence8);
        fenceModel.Draw(ourShader);

        glm::mat4 fence9 = glm::mat4(1.0f);
        fence9 = glm::translate(fence9, programState->fence9Position);
        fence9 = glm::scale(fence9, glm::vec3(programState->fenceScale));
        fence9 = glm::rotate(fence9, glm::radians(-25.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        fence9 = glm::rotate(fence9, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        ourShader.setMat4("model", fence9);
        fenceModel.Draw(ourShader);

        // gate
        glm::mat4 gate = glm::mat4(1.0f);
        gate = glm::translate(gate, programState->gatePosition);
        gate = glm::scale(gate, glm::vec3(programState->gateScale));
        gate = glm::rotate(gate, glm::radians(-2.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        gate = glm::rotate(gate, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        ourShader.setMat4("model", gate);
        gateModel.Draw(ourShader);

        // water bowl
        glm::mat4 waterBowl = glm::mat4(1.0f);
        waterBowl = glm::translate(waterBowl, programState->waterBowlPosition);
        waterBowl = glm::scale(waterBowl, glm::vec3(programState->waterBowlScale));
        waterBowl = glm::rotate(waterBowl, glm::radians(-93.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        waterBowl = glm::rotate(waterBowl, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        ourShader.setMat4("model", waterBowl);
        waterBowlModel.Draw(ourShader);

        // sheep
        glm::mat4 sheep = glm::mat4(1.0f);
        sheep = glm::translate(sheep, programState->sheepPosition);
        sheep = glm::scale(sheep, glm::vec3(programState->sheepScale));
        sheep = glm::rotate(sheep, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
//        sheep = glm::rotate(sheep, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        ourShader.setMat4("model", sheep);
        sheepModel.Draw(ourShader);

        glm::mat4 sheep2 = glm::mat4(1.0f);
        sheep2 = glm::translate(sheep2, programState->sheep2Position);
        sheep2 = glm::scale(sheep2, glm::vec3(programState->sheepScale));
//        sheep2 = glm::rotate(sheep2, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
//        sheep2 = glm::rotate(sheep2, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        ourShader.setMat4("model", sheep2);
        sheepModel.Draw(ourShader);

        // water tower
        glm::mat4 waterTower = glm::mat4(1.0f);
        waterTower = glm::translate(waterTower, programState->waterTowerPosition);
        waterTower = glm::scale(waterTower, glm::vec3(programState->waterTowerScale));
        ourShader.setMat4("model", waterTower);
        waterTowerModel.Draw(ourShader);

        // wall lamp
        glm::mat4 lamp = glm::mat4(1.0f);
        lamp = glm::translate(lamp, programState->lampPosition);
        lamp = glm::scale(lamp, glm::vec3(programState->lampScale));
        ourShader.setMat4("model", lamp);
        lampModel.Draw(ourShader);

        // draw skybox
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.use();
        view = glm::mat4(glm::mat3(programState->camera.GetViewMatrix())); // remove translation from the view matrix
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);
        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default

        // draw grass
        blendingShader.use();
        blendingShader.setVec3("dirLight.direction", 0.35, -1.45, -1.1);
        blendingShader.setVec3("dirLight.ambient",  0.05f, 0.05f, 0.05f);
        blendingShader.setVec3("dirLight.diffuse", 0.25f, 0.25f, 0.25f);
        blendingShader.setVec3("dirLight.specular", 0.3f, 0.3f, 0.3f);
        blendingShader.setVec3("viewPosition", programState->camera.Position);

        projection = glm::perspective(glm::radians(programState->camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        view = programState->camera.GetViewMatrix();
        blendingShader.setMat4("projection", projection);
        blendingShader.setMat4("view", view);
        glBindVertexArray(transparentVAO);
        glBindTexture(GL_TEXTURE_2D, transparentTexture);

        for (auto i : vegetation)
        {
            float angle = 0.0f;
            for (int j = 0; j < 12; ++j) {
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, i);
                model = glm::scale(model, glm::vec3(0.3f));
                model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
                model = glm::translate(model, glm::vec3(-0.5f, 0.0f, 0.0f));
                blendingShader.setMat4("model", model);
                glDrawArrays(GL_TRIANGLES, 0, 6);
                angle += 30.0f;
            }
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // 2. blur bright fragments with two-pass Gaussian Blur
        // ----------------------------------------------------
        bool horizontal = true, first_iteration = true;
        unsigned int amount = 10;
        blurShader.use();
        for (unsigned int i = 0; i < amount; i++)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
            blurShader.setInt("horizontal", horizontal);
            glBindTexture(GL_TEXTURE_2D, first_iteration ? colorBuffers[1] : pingpongColorbuffers[!horizontal]);  // bind texture of other framebuffer (or scene if first iteration)
            glBindVertexArray(quadVAO);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            glBindVertexArray(0);
            horizontal = !horizontal;
            if (first_iteration)
                first_iteration = false;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // 3. now render floating point color buffer to 2D quad and tonemap HDR colors to default framebuffer's (clamped) color range
        // --------------------------------------------------------------------------------------------------------------------------
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        bloomShader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[!horizontal]);
        bloomShader.setInt("bloom", programState->bloomFlag);
        bloomShader.setFloat("exposure", 0.2f);
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);

        if (programState->ImGuiEnabled)
            DrawImGui(programState);



        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    programState->SaveToFile("resources/program_state.txt");
    delete programState;
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // deallocate
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &skyboxVBO);

    glDeleteVertexArrays(1, &transparentVAO);
    glDeleteBuffers(1, &transparentVBO);

    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        programState->camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        programState->camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        programState->camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        programState->camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        programState->camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        programState->camera.ProcessKeyboard(DOWN, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    if (programState->CameraMouseMovementUpdateEnabled)
        programState->camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    programState->camera.ProcessMouseScroll(yoffset);
}

void DrawImGui(ProgramState *programState) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();


    {
        static float f = 0.0f;
        ImGui::Begin("Hello window");
        ImGui::Text("Hello text");
        ImGui::SliderFloat("Float slider", &f, 0.0, 1.0);
        ImGui::ColorEdit3("Background color", (float *) &programState->clearColor);
        ImGui::DragFloat3("Field position", (float*)&programState->fieldPosition);
        ImGui::DragFloat("Field scale", &programState->fieldScale, 0.05, 0.1, 4.0);

        ImGui::DragFloat("pos X", &programState->pointLight.position.x, 0.05, -20.0, 20.0);
        ImGui::DragFloat("pos Y", &programState->pointLight.position.y, 0.05, -20.0, 20.0);
        ImGui::DragFloat("pos Z", &programState->pointLight.position.z, 0.05, -20.0, 20.0);

        ImGui::DragFloat("amb X", &programState->pointLight.ambient.x, 0.05, -20.0, 20.0);
        ImGui::DragFloat("amb Y", &programState->pointLight.ambient.y, 0.05, -20.0, 20.0);
        ImGui::DragFloat("amb Z", &programState->pointLight.ambient.z, 0.05, -20.0, 20.0);
        ImGui::End();
    }

    {
        ImGui::Begin("Camera info");
        const Camera& c = programState->camera;
        ImGui::Text("Camera position: (%f, %f, %f)", c.Position.x, c.Position.y, c.Position.z);
        ImGui::Text("(Yaw, Pitch): (%f, %f)", c.Yaw, c.Pitch);
        ImGui::Text("Camera front: (%f, %f, %f)", c.Front.x, c.Front.y, c.Front.z);
        ImGui::Checkbox("Camera mouse update", &programState->CameraMouseMovementUpdateEnabled);
        ImGui::End();
    }

    // ovaj deo koda je pozajmljen od kolege Marka Moljkovića (mljkvc)
    // ---------------------------------------------------------------
    // fps info
    {
        ImVec4 textColor;
        float fps = ImGui::GetIO().Framerate;
        if (fps > 55.0f)
            textColor = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
        else if (fps >= 40.0f && fps <= 55.0f)
            textColor = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
        else
            textColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);

        ImGuiIO &io = ImGui::GetIO();
        ImVec2 window_pos = ImVec2(io.DisplaySize.x - 85, 5);
        ImGui::SetNextWindowPos(window_pos);
        ImGui::SetNextWindowSize(ImVec2(0, 0));
        ImGui::Begin("FPS", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground);
        ImGui::TextColored(textColor, "FPS: %.1f", fps);
        ImGui::End();
    }
    // ---------------------------------------------------------------
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_F1 && action == GLFW_PRESS) {
        programState->ImGuiEnabled = !programState->ImGuiEnabled;
        if (programState->ImGuiEnabled) {
            programState->CameraMouseMovementUpdateEnabled = false;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
    }
    if (key == GLFW_KEY_B && action == GLFW_PRESS)
        programState->bloomFlag = !programState->bloomFlag;
}

unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}