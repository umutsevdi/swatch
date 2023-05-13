/******************************************************************************

 * File: src/main.cpp
 *
 * Author: Umut Sevdi
 * Created: 04/26/23
 * Description: System capture

*****************************************************************************/
#include <bcm2835.h>
#include <iostream>

#include "sw.hpp"

#define OLED_WIDTH 128
#define OLED_HEIGHT 64

/*  bcm2835I2CClockDivider enum , see readme. */
const uint16_t I2C_SPEED = 626;
const uint8_t I2C_ADDRESS = 0x3C;

void loop(sw::ChannelManager& screen);
void server(sw::ChannelManager& screen);

int main(int argc, char** argv)
{
    if (!bcm2835_init()) {
        printf("Error 1201: init bcm2835 library\r\n");
        return -1;
    }
    std::cout << "Starting display\n";
    sw::ChannelManager screen(OLED_WIDTH, OLED_HEIGHT, I2C_SPEED, I2C_ADDRESS);
    screen.add_channel(sw::ProcessListener("Nextcloud", "-n"));
    screen.add_channel(sw::ProcessListener("Site", "-s"));
    bcm2835_delay(100);

    loop(screen);
    std::cout << "Gradual Shutdown\n";
    return 0;
}

void loop(sw::ChannelManager& screen)
{
    while (true) {
        screen.display();
        delay(1000);
    }
}

void server(sw::ChannelManager& screen)
{
    std::string msg;
    if (msg == "next") {

    } else if (msg == "prev") {
    }
}
