//
// Created by Nicholas Newdigate on 27/12/2023.
//

#ifndef TEENSY_U8G2_OPENGL_EXAMPLES_BASIC_XR1MODEL_H
#define TEENSY_U8G2_OPENGL_EXAMPLES_BASIC_XR1MODEL_H

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
    bool keysPressed[6][6];
    bool touchKeysPressed[13];

    bool ledStates[30];
    encoder_model encoders[5] = {
        {0x36,0,0,0,0,0},
        {0x37,0,0,0,0,0},
        {0x38,0,0,0,0,0},
        {0x39,0,0,0,0,0},
        {0x40,0,0,0,0,0}};
};


#endif //TEENSY_U8G2_OPENGL_EXAMPLES_BASIC_XR1MODEL_H
