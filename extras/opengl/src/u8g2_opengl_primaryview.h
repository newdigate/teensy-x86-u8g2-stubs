#ifndef TEENSY_U8G2_OPENGL_PRIMARYVIEW_H
#define TEENSY_U8G2_OPENGL_PRIMARYVIEW_H

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#else
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#endif

#include "include/shader.h"
#include "include/model.h"
#include "include/camera.h"

#include "opengl_3d_resources.h"
// TODO : nic - 1) TexCoords -> FragColor1, 2) change in int aTexuteIndex -> float


static const char* vertexShaderCode22 = R"glsl(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 Normal;
layout (location = 3) in mat4 aInstanceMatrix;
layout (location = 7) in float aTexuteIndex;

out vec4 FragColor1;
out vec4 FragColor2;
out float TextureIndex;

uniform mat4 projection;
uniform mat4 view;
uniform vec4 aColor1;
uniform vec4 aColor2;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;

void main()
{
    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - aPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 viewDir = normalize(viewPos - aPos);
    vec3 reflectDir = reflect(-lightDir, Normal);

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(Normal, halfwayDir), 0.0), 32.0);

    FragColor1 =  vec4((ambient + diffuse + spec), 0.5f) * aColor1;
    FragColor2 =  vec4((ambient + diffuse + spec), 0.5f) * aColor2;
    TextureIndex = aTexuteIndex;
    gl_Position = projection * view * aInstanceMatrix * vec4(aPos, 1.0f);
}
)glsl";

static const char* fragmentShaderCode22 = R"glsl(
#version 330 core
out vec4 FragColor;

in vec4 FragColor1;
in vec4 FragColor2;
in float TextureIndex;

void main()
{
    FragColor = (TextureIndex * FragColor2) + ( (1.0f - TextureIndex) * FragColor1 );
}
)glsl";


class st7735_opengl_primaryview {
public:
    GLFWwindow *window;

    Shader *shader;
    Model *rock;
    // lighting
    glm::vec3 lightPos;;

    glm::mat4* modelMatrices;
    float *modelTextureIndex;

    const unsigned int numRows = 4;
    const unsigned int numCol = 6;
    const unsigned int numKeys = numRows * numCol;

    // settings
    const static unsigned int SCR_WIDTH ;
    const static unsigned int SCR_HEIGHT ;

    // camera
    static Camera camera;
    static float lastX;
    static float lastY ;
    static bool firstMouse;

    // timing
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    XR1Model *xr1_model;

    unsigned int buffer, buffer2;

    st7735_opengl_primaryview()
        : window(nullptr),
          shader(nullptr),
          rock(nullptr),
          modelMatrices(nullptr),
          xr1_model(nullptr),
          lightPos(1.2f, 5.0f, 2.0f)
    {
        modelTextureIndex = new float[numKeys] {0};
    }
    // glfw: whenever the window size changed (by OS or user resize) this callback function executes
    // ---------------------------------------------------------------------------------------------
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
    {
        // make sure the viewport matches the new window dimensions; note that width and
        // height will be significantly larger than specified on retina displays.
        glViewport(0, 0, width, height);
    }

    // glfw: whenever the mouse moves, this callback is called
    // -------------------------------------------------------
    static void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
    {/*
        float xpos = static_cast<float>(xposIn);
        float ypos = static_cast<float>(yposIn);

        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

        lastX = xpos;
        lastY = ypos;

        camera.ProcessMouseMovement(xoffset, yoffset);*/
    }

    // glfw: whenever the mouse scroll wheel scrolls, this callback is called
    // ----------------------------------------------------------------------
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
    {
        camera.ProcessMouseScroll(static_cast<float>(yoffset));
    }

    bool Init(GLFWwindow *wnd, XR1Model *model) {
        xr1_model = model;
        camera.Position = glm::vec3(6.0, 20.0, 5.0);
        camera.Pitch = -84.0;
        camera.Yaw = 270;
        camera.ProcessMouseMovement(0, 0);
        window = wnd;
        glfwMakeContextCurrent(window);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        glfwSetCursorPosCallback(window, mouse_callback);
        glfwSetScrollCallback(window, scroll_callback);

        // tell GLFW to capture our mouse
        // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        // configure global opengl state
        // -----------------------------
        glEnable(GL_DEPTH_TEST);

        rock = new Model(tr909_key_intermediate_obj, tr909_key_intermediate_obj_len);
        /* Make the window's context current */

        const std::string vertexShaderCodeStr =  std::string(vertexShaderCode22);
        const std::string fragmentShaderCodeStr = std::string(fragmentShaderCode22);

        shader = new Shader(vertexShaderCodeStr,  fragmentShaderCodeStr );


        modelMatrices = new glm::mat4[numKeys];
        srand(static_cast<unsigned int>(glfwGetTime())); // initialize random seed
        float radius = 150.0;
        float offset = 25.0f;
        unsigned int i = 0;
        for (unsigned int c=0; c < numCol; c++)
            for (unsigned int r=0; r < numRows; r++)
            {
                glm::mat4 model = glm::mat4(1.0f);/*
                // 1. translation: displace along circle with 'radius' in range [-offset, offset]
                float angle = (float)i / (float)amount * 360.0f;
                float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
                float x = sin(angle) * radius + displacement;
                displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
                float y = displacement * 0.4f; // keep height of asteroid field smaller compared to width of x and z
                displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
                float z = cos(angle) * radius + displacement;
                //model = glm::translate(model, glm::vec3(x, y, z));

                // 2. scale: Scale between 0.05 and 0.25f
                float scale = static_cast<float>((rand() % 20) / 100.0 + 0.05);
                model = glm::scale(model, glm::vec3(scale));

                // 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
                float rotAngle = static_cast<float>((rand() % 360));
                //model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));
    */
                // 4. now add to list of matrices
                model = glm::translate(model, glm::vec3(c * 2.5, 0.0f, r * 2.5));

                modelMatrices[i] = model;
                i++;
            }


        glGenBuffers(1, &buffer2);
        glBindBuffer(GL_ARRAY_BUFFER, buffer2);
        glBufferData(GL_ARRAY_BUFFER, numKeys * sizeof(float), &modelTextureIndex[0], GL_STATIC_DRAW);
        for (unsigned int i = 0; i < rock->meshes.size(); i++)
        {
            unsigned int VAO = rock->meshes[i].VAO;
            glBindVertexArray(VAO);
            // set attribute pointers for matrix (4 times vec4)
            glEnableVertexAttribArray(7);
            glVertexAttribPointer(7, 1, GL_FLOAT, GL_FALSE, sizeof(float),  (void*)0);

            glVertexAttribDivisor(7, 1);

            glBindVertexArray(0);
        }

        // configure instanced array
        // -------------------------
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, numKeys * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);


        // set transformation matrices as an instance vertex attribute (with divisor 1)
        // note: we're cheating a little by taking the, now publicly declared, VAO of the model's mesh(es) and adding new vertexAttribPointers
        // normally you'd want to do this in a more organized fashion, but for learning purposes this will do.
        // -----------------------------------------------------------------------------------------------------------------------------------
        for (unsigned int i = 0; i < rock->meshes.size(); i++)
        {
            unsigned int VAO = rock->meshes[i].VAO;
            glBindVertexArray(VAO);
            // set attribute pointers for matrix (4 times vec4)
            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
            glEnableVertexAttribArray(4);
            glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
            glEnableVertexAttribArray(5);
            glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
            glEnableVertexAttribArray(6);
            glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

            glVertexAttribDivisor(3, 1);
            glVertexAttribDivisor(4, 1);
            glVertexAttribDivisor(5, 1);
            glVertexAttribDivisor(6, 1);
            //glVertexAttribDivisor(7, 1);

            glBindVertexArray(0);
        }

        /*
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        // set the texture wrapping/filtering options (on the currently bound texture object)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // load and generate the texture
        int width, height, nrChannels;
        texturedata = stbi_load("/Users/nicholasnewdigate/Development/github/newdigate/teensy-x86-u8g2-stubs/extras/opengl/resources/textures/tr-keycap.png", &width, &height, &nrChannels, 0);
        if (texturedata)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texturedata);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::cout << "Failed to load texture" << std::endl;
        }
        stbi_image_free(texturedata);


        glGenTextures(1, &texture2);
        glBindTexture(GL_TEXTURE_2D, texture2);
        // set the texture wrapping/filtering options (on the currently bound texture object)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        texturedata2 = stbi_load("/Users/nicholasnewdigate/Development/github/newdigate/teensy-x86-u8g2-stubs/extras/opengl/resources/textures/tr-keycap2.png", &width, &height, &nrChannels, 0);
        if (texturedata2)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texturedata2);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::cout << "Failed to load texture2" << std::endl;
        }
        stbi_image_free(texturedata2);
        */
    }

    void refresh() {
        for (uint8_t i=0; i<24; i++) {
            float ledPWM = xr1_model->_tlc5947.getPWM(i) / 4095.0f;
            xr1_model->ledStates[i] =  ledPWM;
            modelTextureIndex[i] = xr1_model->ledStates[i];
        }

        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // configure transformation matrices - TODO fix the hardcoded 640 x 480 below
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)640 / (float)480, 0.1f, 1000.0f);
        glm::mat4 view = camera.GetViewMatrix();
        shader->use();
        shader->setMat4("projection", projection);
        shader->setMat4("view", view);
        shader->setVec4("aColor1",0.7f, 0.7f, 0.7f, 0.8f);
        shader->setVec4("aColor2",1.0f, 0.7f, 0.7f, 1.0f);
        shader->setVec3("lightPos", lightPos);
        shader->setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        shader->setVec3("viewPos", camera.Position);
        // draw planet
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
        model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
        shader->setMat4("model", model);
        /*
        // draw meteorites
        shader->setInt("texture_diffuse1", 0);
        shader->setInt("texture_diffuse2", 1);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture); // note: we also made the textures_loaded vector public (instead of private) from the model class.
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2); // note: we also made the textures_loaded vector public (instead of private) from the model class.
        */
        for (unsigned int i = 0; i < rock->meshes.size(); i++)
        {
            glBindVertexArray(rock->meshes[i].VAO);

            glBindBuffer(GL_ARRAY_BUFFER, buffer2);
            glBufferData(GL_ARRAY_BUFFER, numKeys * sizeof(float), &modelTextureIndex[0], GL_STATIC_DRAW);

            glDrawElementsInstanced(GL_TRIANGLES, static_cast<unsigned int>(rock->meshes[i].indices.size()), GL_UNSIGNED_INT, 0, numKeys);

            glBindVertexArray(0);

        }


        rock->Draw(*shader);
    }

    // process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
    // ---------------------------------------------------------------------------------------------------------
    void processInput(GLFWwindow *window)
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera.ProcessKeyboard(FORWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera.ProcessKeyboard(BACKWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera.ProcessKeyboard(LEFT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera.ProcessKeyboard(RIGHT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
            camera.ProcessMouseMovement(1, 0);
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
            camera.ProcessMouseMovement(-1, 0);
        if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
            camera.ProcessMouseMovement(0, -1);
        if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
            camera.ProcessMouseMovement(0, 1);
    }


};

// settings
const unsigned int st7735_opengl_primaryview::SCR_WIDTH = 640;
const unsigned int st7735_opengl_primaryview::SCR_HEIGHT = 480;

// camera
Camera st7735_opengl_primaryview::camera;
float st7735_opengl_primaryview::lastX = (float)SCR_WIDTH / 2.0;
float st7735_opengl_primaryview::lastY = (float)SCR_HEIGHT / 2.0;
bool st7735_opengl_primaryview::firstMouse = true;

#endif //TEENSY_U8G2_OPENGL_PRIMARYVIEW_H
