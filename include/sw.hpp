/******************************************************************************

 * File: include/writer.hpp
 *
 * Author: Umut Sevdi
 * Created: 04/26/23
 * Description: writer.hpp - Writer object is responsible from writing the
 * messages

 *****************************************************************************/

#ifndef __SWATCH_HPP__
#define __SWATCH_HPP__
#include <memory>
#include <string>
#include <vector>

#include <SSD1306_OLED.hpp>
namespace sw {

class Listener {
public:
    Listener(std::string name)
    {
        this->name = name;
    }
    /* listen - updates the Block value and returns a reference */
    virtual std::string listen();
    std::string name;
};

class ChannelManager {
public:
    ChannelManager(int width, int height, uint16_t I2C_Speed, uint8_t I2C_Address);
    ~ChannelManager();
    ChannelManager(ChannelManager&&) = default;
    ChannelManager(const ChannelManager&) = delete;
    ChannelManager& operator=(ChannelManager&&) = default;
    ChannelManager& operator=(const ChannelManager&) = delete;

    void add_channel(Listener l);
    /* switch to next channel */
    void next();
    /* switch to previous channel */
    void prev();
    void display();

private:
    uint8_t* screen_buffer;
    int index;
    std::unique_ptr<SSD1306> screen;
    std::vector<Listener> channels;
};

/******************************************************************************
                                STANDARD_LISTENER
*****************************************************************************/

enum StatType {
    CPU,
    MEMORY,
    DISK
};

class StatListener : public Listener {
public:
    StatListener(std::string path = "/");
    /**
     * listens selected stat, updates and returns the block reference
     */
    std::string listen() override;

private:
    std::string path;
};

/******************************************************************************
                            PROCESS_LISTENER
*****************************************************************************/

class ProcessListener : public Listener {
public:
    ProcessListener(std::string name, std::string cmd, std::string args);
    ProcessListener(std::string name, std::string args);
    /**
     * executes defined process and writes to the block
     */
    std::string listen() override;

private:
    std::string cmd;
    std::string args;
};

}

#endif // !__SWATCH_HPP___
