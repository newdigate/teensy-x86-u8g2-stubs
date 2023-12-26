//
// Created by Moolet on 18/01/2021.
//

#ifndef TEENSY_U8G2_OPENGL_H
#define TEENSY_U8G2_OPENGL_H

#include <iostream>
#include <Arduino.h>
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <U8g2lib.h>
#include <map>
#else
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#endif
static const char* vertexShaderCode = R"glsl(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec3 ourColor;
out vec2 TexCoord;

void main()
{
	gl_Position = vec4(aPos, 1.0);
	ourColor = aColor;
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}
)glsl";

static const char* fragmentShaderCode = R"glsl(
#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

// texture sampler
uniform sampler2D texture1;

void main()
{

	FragColor = texture(texture1, TexCoord);
}
)glsl";

class st7735_opengl_window {
public:
    static bool _initialized;
    static GLFWwindow *window;
    static uint16_t textureImage[128*128];
    static GLuint shader_program, vertex_shader, fragment_shader;
    static GLuint texture;
    static int16_t _frameSize;
    // vao and vbo handle
    static unsigned int VBO, VAO, EBO;
    static inline bool _drawFrame = false;

    static bool InitOpenGL(int16_t frameSize, bool drawFrame = false, GLFWkeyfun key_callback = nullptr, GLFWcharfun character_callback = nullptr) {
        _frameSize = frameSize;
        _drawFrame = drawFrame;
/* Initialize the library */
        if (!glfwInit()) {
            return false;
        }

    #ifdef __APPLE__
        /* We need to explicitly ask for a 3.2 context on OS X */
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true); // comment this line in a release build!
    #endif

        GLFWmonitor *monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode *mode = glfwGetVideoMode(monitor);

        /* Create a windowed mode window and its OpenGL context */
        int width = (_drawFrame)? 128+(_frameSize*2) : 128;
        int height = (_drawFrame)? 64+(_frameSize*2) : 64;
        st7735_opengl_window::window = glfwCreateWindow(width, height, "u8g2 128x64", NULL, NULL);
        if (!st7735_opengl_window::window) {
            glfwTerminate();
            return false;
        }

        //glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        glfwSetWindowCloseCallback(st7735_opengl_window::window, collectWindowClose);

        if (key_callback != nullptr)
            glfwSetKeyCallback(st7735_opengl_window::window, key_callback);

        if (character_callback != nullptr)
            glfwSetCharCallback(st7735_opengl_window::window, character_callback);

        /* Make the window's context current */
        glfwMakeContextCurrent(st7735_opengl_window::window);

        std::cout << "GL Version: " << (char *) glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;


        int length;

        // create and compiler vertex shader
        vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        length = strlen(vertexShaderCode);
        glShaderSource(vertex_shader, 1, &vertexShaderCode, &length);
        glCompileShader(vertex_shader);
        if(!check_shader_compile_status(vertex_shader)) {
            glfwDestroyWindow(st7735_opengl_window::window);
            glfwTerminate();
            return false;
        }

        // create and compiler fragment shader
        fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        length = strlen(fragmentShaderCode);
        glShaderSource(fragment_shader, 1, &fragmentShaderCode, &length);
        glCompileShader(fragment_shader);
        if(!check_shader_compile_status(fragment_shader)) {
            glfwDestroyWindow(st7735_opengl_window::window);
            glfwTerminate();
            return false;
        }

        // create program
        shader_program = glCreateProgram();

        // attach shaders
        glAttachShader(shader_program, vertex_shader);
        glAttachShader(shader_program, fragment_shader);

        // link the program and check for errors
        glLinkProgram(shader_program);
        check_program_link_status(shader_program);

        float vertices[] = {
                // positions          // colors           // texture coords
                1.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f, // top right
                1.0f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.5f, // bottom right
                -1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.5f, // bottom left
                -1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f  // top left
        };
        if (_drawFrame) {
            float multiplier = 128.0f/(128.0f + 2.0f * _frameSize);
            vertices[0] *= multiplier;
            vertices[1] *= multiplier;
            vertices[2] *= multiplier;

            vertices[8] *= multiplier;
            vertices[9] *= multiplier;
            vertices[10] *= multiplier;

            vertices[16] *= multiplier;
            vertices[17] *= multiplier;
            vertices[18] *= multiplier;

            vertices[24] *= multiplier;
            vertices[25] *= multiplier;
            vertices[26] *= multiplier;
        }
        unsigned int indices[] = {
                0, 1, 3, // first triangle
                1, 2, 3  // second triangle
        };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // color attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        // texture coord attribute
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);


        // load and create a texture
        // -------------------------

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// set texture wrapping to GL_REPEAT (default wrapping method)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        // load image, create texture and generate mipmaps

    //    textureImage = new  {0};
        /*
            uint16_t count = 0;
            for (int i = 0; i < 128; i++) {
                for (int j = 0; j < 128; j++) {
                    count ++;
                    if (j % 2 == 0) {
                        textureImage[i * 128 + j ] = 0xFFFF;
                    }
                }
            }
        */
        _initialized = true;
        return true;
    }

    static inline bool shouldClose() {
        if (!_initialized) return false;

        return glfwWindowShouldClose(window);
    }
    
    static void refresh() {
        if (!_initialized) return;
        /*if (_surpressUpdate) return;
        if (_useFramebuffer && !_update_cont) return;
        if (!_needsUpdate) {
            glfwPollEvents();
            return;
        }
    */
        //unsigned long microsStart = micros();
        /*
        if (microsStart - lastUpdate < 100) {
            return;
        }
        */
        //lastUpdate = microsStart;
        glfwPollEvents();

        // use the shader program
        glUseProgram(shader_program);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 128, 128, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, textureImage);
        // render
        // ------
        glClearColor(0.2f, 0.3f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // bind Texture
        glBindTexture(GL_TEXTURE_2D, texture);

        // render container
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
        //int microsStop = micros();
        //Serial.printf("OpenGL update: %u (%i micros)\n", _updateCount, microsStop - microsStart);
    }
    // helper to check and display for shader compiler errors
    static bool check_shader_compile_status(GLuint obj) {
        GLint status;
        glGetShaderiv(obj, GL_COMPILE_STATUS, &status);
        if(status == GL_FALSE) {
            GLint length;
            glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &length);
            char log[length+1];
            glGetShaderInfoLog(obj, length, &length, &log[0]);
            std::cerr << &log[0];
            return false;
        }
        return true;
    }

    // helper to check and display for shader linker error
    static bool check_program_link_status(GLuint obj) {
        GLint status;
        glGetProgramiv(obj, GL_LINK_STATUS, &status);
        if(status == GL_FALSE) {
            GLint length;
            glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &length);
            char log[length+1];
            glGetProgramInfoLog(obj, length, &length, &log[0]);
            std::cerr << &log[0];
            return false;
        }
        return true;
    }
    
    static void collectWindowClose(GLFWwindow* window) {
        arduino_should_exit = true;
    }
};

bool  st7735_opengl_window::_initialized = false;
GLFWwindow *st7735_opengl_window::window = nullptr;
uint16_t st7735_opengl_window::textureImage[128*128] = {0};
GLuint st7735_opengl_window::shader_program;
GLuint st7735_opengl_window::vertex_shader;
GLuint st7735_opengl_window::fragment_shader;
GLuint st7735_opengl_window::texture;
unsigned int st7735_opengl_window::VBO;
unsigned int st7735_opengl_window::VAO;
unsigned int st7735_opengl_window::EBO;
int16_t st7735_opengl_window::_frameSize = 0;

struct encoder_model {
public:
    int addr;
    int16_t rmin;
    int16_t rmax;
    int16_t rstep;
    int16_t rval;
    uint8_t rloop;
};

template<typename Wire_T, typename Keypad_T, typename CapTouch_T>
class U8G2_128X64_OPENGL : public U8G2 {
public:

    U8G2_128X64_OPENGL(const u8g2_cb_t *rotation, uint8_t clock, uint8_t data, uint8_t cs, uint8_t dc, uint8_t reset = 255, Wire_T *wire = nullptr, Keypad_T *keypad = nullptr, CapTouch_T *touch = nullptr) :
        U8G2()
    {
        u8g2_Setup_opengl_128x64(&u8g2, rotation, u8x8_byte_arduino_4wire_sw_spi, u8x8_gpio_and_delay_arduino);
        //printf("u8g2 wire1: %lx\n", (unsigned long)wire);
        _wire = wire;
        _keypad = keypad;
        _touch = touch;
        if (_wire) {
            _wire->onDataSentToDevice = dataSentToDevice;
           /* if (_wire->onDataSentToDevice) {
                _wire->onDataSentToDevice(1);
            }
            */
        }

        if (st7735_opengl_window::InitOpenGL(0, false, key_callback, character_callback))
            return;
    }

    virtual ~U8G2_128X64_OPENGL() = default;

    static void dataSentToDevice(uint8_t address) {
        std::vector<uint8_t>* queue = _wire->getDeviceRequestQueue(address);
        if (queue) {
            if (queue->size() == 10) {
                encoder_model *selectedEncoder = nullptr;
                for (auto &index: encoders) {
                    if (index.addr == address) {
                        selectedEncoder = &index;
                        break;
                    }
                }
                if (selectedEncoder) {
                    selectedEncoder->rval  = (int16_t)((*queue)[1] << 8 | (*queue)[0]);
                    selectedEncoder->rloop = (int16_t)((*queue)[3] << 8 | (*queue)[2]);
                    selectedEncoder->rmin  = (int16_t)((*queue)[5] << 8 | (*queue)[4]);
                    selectedEncoder->rmax  = (int16_t)((*queue)[7] << 8 | (*queue)[6]);
                    selectedEncoder->rstep = (int16_t)((*queue)[9] << 8 | (*queue)[8]);
                }

                if (_wire) {
                    _wire->addRequestFromCallback(address, [] (uint8_t address, uint16_t count) -> int16_t {
                        for (auto &index: encoders) {
                            if (index.addr == address) {
                                auto lsb = (int8_t)(index.rval & 0x00FF);
                                auto msb = (int8_t)(index.rval >> 8);
                                _wire->addDeviceDataResponse(address, lsb);
                                _wire->addDeviceDataResponse(address, msb );

                                return 2;
                            }
                        }
                        return 0;
                    });
                }

                queue->clear();
            }
        }
    }

    void setDrawColor(uint8_t drawColor) {
        U8G2::setDrawColor(drawColor);
    }

    void Pixel(int16_t x, int16_t y, uint16_t color) {
        uint16_t index = y * 128 + x;
        if (st7735_opengl_window::textureImage[index] != color) {
            st7735_opengl_window::textureImage[index] = color;
        }
    }
    static void u8g2_Setup_opengl_128x64(u8g2_t *u8g2, const u8g2_cb_t *rotation, u8x8_msg_cb byte_cb, u8x8_msg_cb gpio_and_delay_cb)
    {
        uint8_t tile_buf_height;
        uint8_t *buf;
        //u8g2_SetupDisplay(u8g2, u8x8_d_opengl_generic, u8x8_cad_001, byte_cb, gpio_and_delay_cb);
        u8g2_SetupDisplay(u8g2, u8x8_d_ssd1305_128x32_noname, u8x8_cad_001, byte_cb, gpio_and_delay_cb);
        buf = u8g2_m_16_8_f(&tile_buf_height);
        u8g2_SetupBuffer(u8g2, buf, tile_buf_height, u8g2_ll_hvline_vertical_top_lsb, rotation);
    }

    static uint8_t u8x8_d_ssd1305_128x32_noname(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
    {
        if ( u8x8_d_opengl_generic(u8x8, msg, arg_int, arg_ptr) != 0 )
            return 1;

        switch(msg)
        {
            case U8X8_MSG_DISPLAY_SET_FLIP_MODE:
                if ( arg_int == 0 )
                {
                    //u8x8_cad_SendSequence(u8x8, u8x8_d_ssd1305_128x32_flip0_seq);
                    u8x8->x_offset = u8x8->display_info->default_x_offset;
                }
                else
                {
                    //u8x8_cad_SendSequence(u8x8, u8x8_d_ssd1305_128x32_flip1_seq);
                    u8x8->x_offset = u8x8->display_info->flipmode_x_offset;
                }
                break;
            case U8X8_MSG_DISPLAY_INIT:
                u8x8_d_helper_display_init(u8x8);
                //u8x8_cad_SendSequence(u8x8, u8x8_d_ssd1305_128x32_noname_init_seq);
                break;
            case U8X8_MSG_DISPLAY_SETUP_MEMORY:
                u8x8_d_helper_display_setup_memory(u8x8, &u8x8_opengl_128x64_noname_display_info);
                break;
            default:
                return 0;
        }
        return 1;
    }

    static uint8_t u8x8_d_opengl_generic(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
    {
        uint8_t y, x, c, cmax;
        uint8_t *ptr;
        switch(msg)
        {
            case U8X8_MSG_DISPLAY_DRAW_TILE:
                x = ((u8x8_tile_t *)arg_ptr)->x_pos;
                y = ((u8x8_tile_t *)arg_ptr)->y_pos * 8;
                x *= 8;
                x += u8x8->x_offset;
                do
                {
                    c = ((u8x8_tile_t *)arg_ptr)->cnt;
                    cmax = c;
                    ptr = ((u8x8_tile_t *)arg_ptr)->tile_ptr;
                    do
                    {
                        for (int i=0; i<8; i++) {
                            uint8_t value = *ptr;
                            for (int j = 0; j < 8; j++) {
                                bool color = bitRead(value, j);
                                uint16_t index = (((j + y) * 128) + (i + x + ((cmax-c) * 8)));
                                st7735_opengl_window::textureImage[index] = color? 0xFFFF : 0x0000;
                            }
                            ptr++;
                        }
                        c--;
                    } while( c > 0 );
                    arg_int--;
                } while( arg_int > 0 );


                u8x8_cad_EndTransfer(u8x8);
                break;


            default:
                return 0;
        }
        return 1;
    }

    constexpr static const u8x8_display_info_t u8x8_opengl_128x64_noname_display_info =
            {
                    /* chip_enable_level = */ 0, /* chip_disable_level = */ 1,
                    /* post_chip_enable_wait_ns = */ 20, /* pre_chip_disable_wait_ns = */ 10,
                    /* reset_pulse_width_ms = */ 100,    /* SSD1306: 3 us */
                    /* post_reset_wait_ms = */ 100, /* far east OLEDs need much longer setup time */
                    /* sda_setup_time_ns = */ 50,        /* SSD1306: 15ns, but cycle time is 100ns, so use 100/2 */
                    /* sck_pulse_width_ns = */ 50,    /* SSD1306: 20ns, but cycle time is 100ns, so use 100/2, AVR: below 70: 8 MHz, >= 70 --> 4MHz clock */
                    /* sck_clock_hz = */ 4000000UL,    /* since Arduino 1.6.0, the SPI bus speed in Hz. Should be  1000000000/sck_pulse_width_ns */
                    /* spi_mode = */ 0,        /* active high, rising edge */
                    /* i2c_bus_clock_100kHz = */ 4,
                    /* data_setup_time_ns = */ 40,
                    /* write_pulse_width_ns = */ 150,    /* SSD1306: cycle time is 300ns, so use 300/2 = 150 */
                    /* tile_width = */ 16,
                    /* tile_height = */ 8,
                    /* default_x_offset = */ 0,
                    /* flipmode_x_offset = */ 0,
                    /* pixel_width = */ 128,
                    /* pixel_height = */ 64
            };

    static encoder_model encoders[5];
    static std::map<uint16_t, std::vector<uint8_t >> _openglToKeyPadMap;
    static std::map<uint16_t, uint8_t> _openglToTouchMap;
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        if (_openglToKeyPadMap.count(key) == 1) {
            std::vector<uint8_t> &mapping = _openglToKeyPadMap[key];
            switch (action) {
                case GLFW_RELEASE:
                    if (_keypad) {
                        _keypad->unpressKey(mapping[0], mapping[1]);
                    }
                    break;
                case GLFW_PRESS:
                    if (_keypad) {
                        _keypad->pressKey(mapping[0], mapping[1]);
                    }
                    break;
            }
        }

        if (_openglToTouchMap.count(key) == 1) {
            uint8_t touchKey = _openglToTouchMap[key];
            switch (action) {
                case GLFW_RELEASE:
                    if (_touch) {
                        _touch->setTouched(touchKey, false);
                    }
                    break;
                case GLFW_PRESS:
                    if (_touch) {
                        _touch->setTouched(touchKey, true);
                        break;
                    }
            }
        }

        if (action == GLFW_RELEASE) {
            bool isEncoderChange = false;
            bool isEncoderIncreased = false;
            uint8_t encoderIndex = 0;

            switch(key) {
                case GLFW_KEY_1:
                case GLFW_KEY_3:
                case GLFW_KEY_5:
                case GLFW_KEY_7:
                case GLFW_KEY_9:
                {
                    isEncoderChange = true;
                    isEncoderIncreased = false;
                    encoderIndex = (key - GLFW_KEY_0)/2;
                    break;
                }
                case GLFW_KEY_2:
                case GLFW_KEY_4:
                case GLFW_KEY_6:
                case GLFW_KEY_8:
                {
                    isEncoderChange = true;
                    isEncoderIncreased = true;
                    encoderIndex = (key - GLFW_KEY_0)/2 - 1;
                    break;
                }
                case GLFW_KEY_0:
                {
                    isEncoderChange = true;
                    isEncoderIncreased = true;
                    encoderIndex = 4;
                    break;
                }
            }

            if (isEncoderChange){
                //Serial.printf("[encoder inc %d] ", encoderIndex);
                encoder_model &encoder = encoders[encoderIndex];
                if (isEncoderIncreased && encoder.rval < encoder.rmax - encoder.rstep )
                    encoder.rval+=encoder.rstep;
                else if (encoder.rval > encoder.rmin + encoder.rstep)
                    encoder.rval-=encoder.rstep;
                //Serial.printf("value: %d\n", encoder.rval);
                /*
                if (_wire != nullptr) {
                    _wire->addDeviceDataResponse(encoder.addr, encoder.rval & 0x00FF );
                    _wire->addDeviceDataResponse(encoder.addr, encoder.rval >> 8);
                }
                 */
            }
        } else if (action == GLFW_PRESS)
        {
            switch(key) {
                case GLFW_KEY_UP:
                case GLFW_KEY_DOWN:
                case GLFW_KEY_LEFT:
                case GLFW_KEY_RIGHT:
                case GLFW_KEY_ENTER: {
                    char *s = new char[_textCharacterInput.length()];
                    memcpy(s, _textCharacterInput.c_str(), _textCharacterInput.length());
                    Serial.queueSimulatedCharacterInput(s, _textCharacterInput.length());
                    delete [] s;

                    char c = '\n';
                    Serial.queueSimulatedCharacterInput(&c, 1);

                    _textCharacterInput.clear();

                    break;
                }
                default:
                    break;
            }

        }

    }

    static void character_callback(GLFWwindow* window, unsigned int codepoint) {
        /*
        char c = (char) codepoint;
        _textCharacterInput += c;
         */
    }

private:
    static std::string _textCharacterInput;
    static Wire_T *_wire;
    static Keypad_T *_keypad;
    static CapTouch_T *_touch;

};
template <typename Wire_T, typename Keypad_T, typename CapTouch_T> encoder_model U8G2_128X64_OPENGL<Wire_T,Keypad_T,CapTouch_T>::encoders[5] = {
        {0x36,0,0,0,0,0},
        {0x37,0,0,0,0,0},
        {0x38,0,0,0,0,0},
        {0x39,0,0,0,0,0},
        {0x40,0,0,0,0,0}};

template <typename Wire_T, typename Keypad_T, typename CapTouch_T> Wire_T *U8G2_128X64_OPENGL<Wire_T,Keypad_T,CapTouch_T>::_wire;
template <typename Wire_T, typename Keypad_T, typename CapTouch_T> Keypad_T *U8G2_128X64_OPENGL<Wire_T,Keypad_T,CapTouch_T>::_keypad;
template <typename Wire_T, typename Keypad_T, typename CapTouch_T> CapTouch_T *U8G2_128X64_OPENGL<Wire_T,Keypad_T,CapTouch_T>::_touch;

template <typename Wire_T, typename Keypad_T, typename CapTouch_T> std::string U8G2_128X64_OPENGL<Wire_T,Keypad_T,CapTouch_T>::_textCharacterInput;

template <typename Wire_T, typename Keypad_T, typename CapTouch_T> std::map<uint16_t, std::vector<uint8_t>> U8G2_128X64_OPENGL<Wire_T,Keypad_T,CapTouch_T>::_openglToKeyPadMap = {
        { GLFW_KEY_ESCAPE,  {3, 1}}, // 'j' ESCAPE_BTN_CHAR
        { GLFW_KEY_SPACE,   {2, 1}}, // 'i' SELECT_BTN_CHAR
        { GLFW_KEY_ENTER,   {5, 5}}, // '0' SOUND_BTN_CHAR
        { GLFW_KEY_X,       {5, 4}}, // '4' TEMPO_BTN_CHAR
        { GLFW_KEY_C,       {5, 3}},     // 'x' COPY_BTN_CHAR
        { GLFW_KEY_Z,       {5, 2}},    // 'r' FUNCTION_BTN_CHAR
        { GLFW_KEY_V,       {2, 0}},    // 'c' TRACK_BTN_CHAR
        { GLFW_KEY_B,       {1, 0}},    // 'b' PATTERN_BTN_CHAR
        { GLFW_KEY_N,       {0, 0}},    // 'a' PERFORM_BTN_CHAR

        { GLFW_KEY_Q,       {2, 0}},    // 'm' step 1 / 16
        { GLFW_KEY_W,       {2, 1}},    // 'n' step 2 / 16
        { GLFW_KEY_E,       {2, 2}},    // 'o' step 3 / 16
        { GLFW_KEY_R,       {2, 3}},    // 'p' step 4 / 16

        { GLFW_KEY_T,       {3, 0}},    // 's' step 5 / 16
        { GLFW_KEY_Y,       {3, 1}},    // 't' step 6 / 16
        { GLFW_KEY_U,       {3, 2}},    // 'u' step 7 / 16
        { GLFW_KEY_I,       {3, 3}},    // 'v' step 8 / 16

        { GLFW_KEY_A,       {4, 0}},    // 'y' step 9 / 16
        { GLFW_KEY_S,       {4, 1}},    // 'z' step 10 / 16
        { GLFW_KEY_D,       {4, 2}},    // '1' step 11 / 16
        { GLFW_KEY_F,       {4, 3}},    // '2' step 12 / 16

        { GLFW_KEY_G,       {5, 0}},    // '5' step 13 / 16
        { GLFW_KEY_H,       {5, 1}},    // '6' step 14 / 16
        { GLFW_KEY_J,       {5, 2}},    // '7' step 15 / 16
        { GLFW_KEY_K,       {5, 3}},    // '8' step 16 / 16
};

template <typename Wire_T, typename Keypad_T, typename CapTouch_T> std::map<uint16_t, uint8_t> U8G2_128X64_OPENGL<Wire_T,Keypad_T,CapTouch_T>::_openglToTouchMap = {
        { GLFW_KEY_P,               13},     // 'c'
        { GLFW_KEY_MINUS,           12},     // 'c#'
        { GLFW_KEY_LEFT_BRACKET,    11},     // 'd'
        { GLFW_KEY_EQUAL,           10},     // 'd#'
        { GLFW_KEY_RIGHT_BRACKET,   9},     // 'e'
        { GLFW_KEY_M,               8},     // 'f'
        { GLFW_KEY_K,               7},     // 'f#'
        { GLFW_KEY_COMMA,           6},     // 'g'
        { GLFW_KEY_L,               5},     // 'g#'
        { GLFW_KEY_PERIOD,          4},     // 'a'
        { GLFW_KEY_SEMICOLON,       3},    // 'a#'
        { GLFW_KEY_SLASH,           2},    // 'b'
        { GLFW_KEY_RIGHT_SHIFT,     1},    // 'c2'
};

#endif //TEENSY_U8G2_OPENGL_H
