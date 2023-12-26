#include "u8g2_opengl.h"
#include "u8g2_opengl_main.h"
#include "Wire.h"
#include "Keypad.h"
#include <Adafruit_MPR121.h>
Adafruit_MPR121 mpr121_a = Adafruit_MPR121();

const byte ROWS = 6;
const byte COLS = 6;

char keys[ROWS][COLS] = {
        {'a','b','c','d','e','f'},
        {'g','h','i','j','k','l'},
        {'m','n','o','p','q','r'},
        {'s','t','u','v','w','x'},
        {'y','z','1','2','3','4'},
        {'5','6','7','8','9','0'},
};

byte rowPins[ROWS] = {38, 37, 36, 35, 34, 33}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {2, 9, 12, 41, 40, 39}; //connect to the column pinouts of the keypad

Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

U8G2_128X64_OPENGL<TwoWire, Keypad, Adafruit_MPR121> u8g2(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 14, /* reset=*/ 15, &Wire1, &kpd, &mpr121_a);

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

int old_encoderValues[5] = {0,0,0,0,0};

void initEncoders() {
    for (int i=0; i<5; i++) {
        encoder_set(encoder_addrs[i], -3000, 3000, 1, -3000, 0);
    }
}

int16_t getEncoder(int address)
{
    std::cout << "getEncoder: " << address;
    Wire1.requestFrom(address, 2);

    int8_t lsbyte = (int8_t)Wire1.read();
    std::cout << " lsb: " << lsbyte;

    int8_t msbyte = (int8_t)Wire1.read();
    std::cout << " msb: " << msbyte;

    int16_t result =  lsbyte & 0x00FF | msbyte << 8;
    std::cout << std::endl;
    return result;
}

void updateEncoders() {
    for (int i=0;i<5;i++){
        if (i > 0) break;
        int16_t encoderValue = getEncoder(encoder_addrs[i]);
        if (encoderValue != old_encoderValues[i]) {
            Serial.printf("Encoder %d changed from %d to %d\n", i, old_encoderValues[i], encoderValue);
            old_encoderValues[i] = encoderValue;
        }
    }
}

void setup(void) {
    initEncoders();
    kpd.setHoldTime(150);
    kpd.setDebounceTime(0);

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

    if (kpd.getKeys()) {
        for (int i=0; i<LIST_MAX; i++)   // Scan the whole key list.
        {
            if (kpd.key[i].stateChanged)   // Only find keys that have changed state.
            {
                switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                    case PRESSED: {
                        Serial.print("button pressed: ");
                        Serial.println(kpd.key[i].kchar);
                        break;
                    }
                }
            }
        }
    }

    updateEncoders();
       delay(1000);
}

int st7735_main(int argc, char** argv) {
    return 0;
}

unsigned __exidx_start;
unsigned __exidx_end;