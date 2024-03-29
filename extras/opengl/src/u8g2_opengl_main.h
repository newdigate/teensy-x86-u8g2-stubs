#ifndef TEENSY_ST7735_LINUX_ST7735_OPENGL_MAIN_H
#define TEENSY_ST7735_LINUX_ST7735_OPENGL_MAIN_H

#include <Arduino.h>
#include "u8g2_opengl.h"

void setup();
void loop();
volatile bool shouldClose = false;

void *arduinoThread(void *threadid) {
    long tid;
    tid = (long)threadid;
    while(!shouldClose and !arduino_should_exit) {
        loop();
        delay(1);
    }
//    cout << "Hello World! Thread ID, " << tid << endl;
    pthread_exit(NULL);
}
extern int st7735_main(int argc, char** argv);

int main(int argc, char** argv) {
    pthread_t thread;
    initialize_mock_arduino();
    st7735_main(argc, argv);
    setup();
    int rc = pthread_create(&thread, NULL, arduinoThread, (void *)0);
    while (!st7735_opengl_window::shouldClose() && !arduino_should_exit) {
        st7735_opengl_window::refreshWindow2();
        st7735_opengl_window::refresh();
        delay(1);
    }
    shouldClose = true;
}

#endif //TEENSY_ST7735_LINUX_ST7735_OPENGL_MAIN_H
