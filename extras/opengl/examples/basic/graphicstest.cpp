#include "u8g2_opengl.h"
#include "u8g2_opengl_main.h"
#include "Wire.h"

U8G2_128X64_OPENGL<TwoWire> u8g2(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 14, /* reset=*/ 15, &Wire1);


void encoder_set(int addr, int16_t rmin, int16_t rmax, int16_t rstep, int16_t rval, uint8_t rloop) {
    Wire1.beginTransmission(addr);
    Wire1.write((uint8_t)(rval & 0xff)); Wire1.write((uint8_t)(rval >> 8));
    Wire1.write(0); Wire1.write(rloop);
    Wire1.write((uint8_t)(rmin & 0xff)); Wire1.write((uint8_t)(rmin >> 8));
    Wire1.write((uint8_t)(rmax & 0xff)); Wire1.write((uint8_t)(rmax >> 8));
    Wire1.write((uint8_t)(rstep & 0xff)); Wire1.write((uint8_t)(rstep >> 8));
    Wire1.endTransmission();
}

int encoder_addrs[5] = {
        0x36, 0x37, 0x38, 0x39, 0x40
};

void initEncoders() {
    for (int i=0; i<5; i++) {
        encoder_set(encoder_addrs[i], -3000, 3000, 1, 0, 0);
    }
}

void setup(void) {
    initEncoders();
    u8g2.begin();
    //u8g2.clearBuffer();					// clear the internal memory
    u8g2.setFont(u8g2_font_ncenB08_tr);	// choose a suitable font
    u8g2.drawStr(0,10,"Hello World!12345");	// write something to the internal memory
    u8g2.drawStr(0,18,"World Hello!");	// write something to the internal memory
    u8g2.drawFrame(0,0, 127, 63);
    u8g2.drawFrame(10,10, 107, 43);
    u8g2.drawFrame(20,20, 87, 23);
    u8g2.drawFrame(30,30, 67, 3);

    //u8g2.clear();
    //u8g2.drawStr(0,10,"nic");
    u8g2.sendBuffer();  // transfer internal memory to the display
}

void loop(void) {
    delay(1000);
}

int st7735_main(int argc, char** argv) {
    return 0;
}

unsigned __exidx_start;
unsigned __exidx_end;