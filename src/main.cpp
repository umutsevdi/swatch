/******************************************************************************

 * File: src/main.cpp
 *
 * Author: Umut Sevdi
 * Created: 04/26/23
 * Description: System capture

*****************************************************************************/

#include "writer.hpp"
#include <bcm2835.h>
#include <iostream>

#define oledwidth 128
#define oledheight 64

/*  bcm2835I2CClockDivider enum , see readme. */
const uint16_t I2C_Speed = 626;
const uint8_t I2C_Address = 0x3C;

sw::Listener* current;

void loop(sw::Screen& screen);
void server(sw::Screen& screen);

int main(int argc, char** argv)
{
    if (!bcm2835_init()) {
        printf("Error 1201: init bcm2835 library\r\n");
        return -1;
    }
    std::cout << "Starting display\n";
    sw::Screen screen(oledwidth, oledheight, I2C_Speed, I2C_Address);
    screen.add_channel(sw::ProcessListener("Nextcloud", "-n"));
    screen.add_channel(sw::ProcessListener("Site", "-s"));
    bcm2835_delay(100);

    loop(screen);
    std::cout << "Gradual Shutdown\n";
    return 0;
}

void loop(sw::Screen& screen)
{
    while (true) {
        screen.display();
        delay(1000);
    }
}

void server(sw::Screen& screen)
{
    std::string msg;
    if (msg == "next") {

    } else if (msg == "prev") {
    }
}
