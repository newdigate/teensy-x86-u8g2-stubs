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

static const char* vertexShaderCode22 = R"glsl(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in mat4 aInstanceMatrix;

out vec2 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = projection * view * aInstanceMatrix * vec4(aPos, 1.0f);
}
)glsl";

static const char* fragmentShaderCode22 = R"glsl(
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;

void main()
{
    FragColor = texture(texture_diffuse1, TexCoords);
}
)glsl";


class st7735_opengl_primaryview {
public:
    GLFWwindow *window;

    Shader *shader;
    Model *rock;
    glm::mat4* modelMatrices;
    unsigned int texture;
    const unsigned int amount = 16;
    unsigned char *texturedata = nullptr;

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

    st7735_opengl_primaryview()
        : window(nullptr),
          shader(nullptr),
          rock(nullptr),
          modelMatrices(nullptr),
          texture(0) {
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

    bool Init(GLFWwindow *wnd, const std::string &modelFileName) {
        camera.Position = glm::vec3(0.0, 40.0, 0.0);
        camera.Pitch = -90.0;
        camera.Yaw = 90.0;
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

        rock = new Model(modelFileName);
        /* Make the window's context current */


        const std::string vertexShaderCodeStr =  std::string(vertexShaderCode22);
        const std::string fragmentShaderCodeStr = std::string(fragmentShaderCode22);

        shader = new Shader(vertexShaderCodeStr,  fragmentShaderCodeStr );
        modelMatrices = new glm::mat4[amount];
        srand(static_cast<unsigned int>(glfwGetTime())); // initialize random seed
        float radius = 150.0;
        float offset = 25.0f;
        for (unsigned int i = 0; i < amount; i++)
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
            model = glm::translate(model, glm::vec3(i * 5.0, 0, 0));

            modelMatrices[i] = model;
        }

        // configure instanced array
        // -------------------------
        unsigned int buffer;
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

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

            glBindVertexArray(0);
        }

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        // set the texture wrapping/filtering options (on the currently bound texture object)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // load and generate the texture
        int width, height, nrChannels;
        texturedata = stbi_load("uvmap.DDS", &width, &height, &nrChannels, 0);
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

    }

    void refresh() {
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
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // configure transformation matrices - TODO fix the hardcoded 640 x 480 below
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)640 / (float)480, 0.1f, 1000.0f);
        glm::mat4 view = camera.GetViewMatrix();
        shader->use();
        shader->setMat4("projection", projection);
        shader->setMat4("view", view);

        // draw planet
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
        model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
        shader->setMat4("model", model);

        // draw meteorites
        shader->setInt("texture_diffuse1", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture); // note: we also made the textures_loaded vector public (instead of private) from the model class.
        for (unsigned int i = 0; i < rock->meshes.size(); i++)
        {
            glBindVertexArray(rock->meshes[i].VAO);
            glDrawElementsInstanced(GL_TRIANGLES, static_cast<unsigned int>(rock->meshes[i].indices.size()), GL_UNSIGNED_INT, 0, amount);
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
