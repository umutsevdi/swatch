/******************************************************************************

 * File: src/main.cpp
 *
 * Author: Umut Sevdi
 * Created: 04/26/23
 * Description: System capture

*****************************************************************************/

#include "writer.hpp"
#include <bcm2835.h>
#include <cstdio>
#include <iostream>

#include "SSD1306_OLED.hpp"

#define oledwidth 128
#define oledheight 64

/*  bcm2835I2CClockDivider enum , see readme. */
const uint16_t I2C_Speed = 626;
const uint8_t I2C_Address = 0x3C;

SSD1306 oled(oledwidth, oledheight);

void TestLoop(void);
void shutdown(void);
int main(int argc, char** argv)
{

    atexit(shutdown);
    if (!bcm2835_init()) {
        printf("Error 1201: init bcm2835 library\r\n");
        return -1;
    }

    std::cout << "Starting display\n";
    oled.OLEDbegin(I2C_Speed, I2C_Address); // initialize the OLED
    oled.OLEDFillScreen(0xF0, 0); // splash screen bars
    bcm2835_delay(500);
    TestLoop();

    return 0;
}

void shutdown()
{
    std::cout << "Gradual Shutdown\n";
    oled.OLEDPowerDown(); // Switch off display
    bcm2835_close(); // Close the library
}

void TestLoop()
{
    // Define a buffer to cover whole screen
    uint8_t screenBuffer[oledwidth * (oledheight / 8) + 1];
    oled.buffer = (uint8_t*)&screenBuffer; // set that to library buffer pointer

    oled.OLEDclearBuffer();
    oled.setTextColor(WHITE);
    oled.setCursor(0, 0);
    oled.OLEDInvert(true);
    bcm2835_delay(100);

    writer::BlockListener c_listener(writer::StatType::CPU);
    writer::BlockListener b_listener(writer::StatType::MEMORY);
    writer::BlockListener d_listener(writer::StatType::DISK);
    while (true) {
        writer::Block& c
            = c_listener.listen();
        writer::Block& b = b_listener.listen();
        writer::Block& d = d_listener.listen();
        oled.OLEDclearBuffer();
        oled.setCursor(0, 0);
        oled.print("                   _x");
        oled.print("      CHANNEL 0      ");
        oled.setCursor(4, 20);
        oled.print(c.str().c_str());
        oled.setCursor(4, 32);
        oled.print(b.str().c_str());
        oled.setCursor(4, 44);
        oled.print(d.str().c_str());
        oled.OLEDupdate();
        delay(1000);
    }
}
