//
// Created by Nicholas Newdigate on 27/12/2023.
//

#ifndef TEENSY_U8G2_OPENGL_EXAMPLES_BASIC_XR1MODEL_H
#define TEENSY_U8G2_OPENGL_EXAMPLES_BASIC_XR1MODEL_H
#include <Keypad.h>
#include <Adafruit_TLC5947.h>

struct encoder_model {
public:
    int addr;
    int16_t rmin;
    int16_t rmax;
    int16_t rstep;
    int16_t rval;
    uint8_t rloop;
};

class XR1Model {
public:

    XR1Model(Keypad &keypad, Adafruit_TLC5947 &tlc5947) : _keypad(keypad), _tlc5947(tlc5947) {
    }

    bool keysPressed[6][6]{};
    bool touchKeysPressed[13]{};
    float ledStates[36]{};
    encoder_model encoders[5] = {
        {0x36,0,0,0,0,0},
        {0x37,0,0,0,0,0},
        {0x38,0,0,0,0,0},
        {0x39,0,0,0,0,0},
        {0x40,0,0,0,0,0}};

    Keypad &_keypad;
    Adafruit_TLC5947 &_tlc5947;

    void openGlKeypadButtonPress(uint8_t i, uint8_t j, bool value) {
        if (value) {
            _keypad.pressKey(i, j);
        } else {
            _keypad.unpressKey(i, j);
        }
        keysPressed[i][j] = value;
    }
    void setFastTouch( int pin, int value) {
        if (pin == 32) {
            touchKeysPressed[12] = value > 63;
            // std::cout << "touchKeysPressed[12]: pin " << pin << " - value " << value << " " << touchKeysPressed[12] << std::endl;
        }
    }
};


#endif //TEENSY_U8G2_OPENGL_EXAMPLES_BASIC_XR1MODEL_H
