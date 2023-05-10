/******************************************************************************

 * File: include/writer.hpp
 *
 * Author: Umut Sevdi
 * Created: 04/26/23
 * Description: writer.hpp - Writer object is responsible from writing the messages

*****************************************************************************/

#ifndef __WRITER__
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace writer {

struct Block {
    /**
     * str - returns string representation of the block to display
     * on OLED screen
     */
    virtual std::string str();
};

class Channel {
public:
    Channel(std::string name)
    {
        this->name = name;
    }
    /**
     * listen - updates the Block value and returns a reference
     * @path - optional value for disk. No effect for the rest
     */
    virtual Block& listen(std::string path = "/");

private:
    std::string name;
};

/******************************************************************************
                                STANDARD_LISTENER
*****************************************************************************/

enum StatType {
    CPU,
    MEMORY,
    DISK
};

struct StatBlock : Block {
    StatType type;
    uint32_t used;
    uint32_t total;
    std::string str() override;
};

class BlockListener : Channel {
public:
    BlockListener(StatType type);
    Block& listen(std::string path = "/") override;

private:
    StatBlock block;
    int disk_ctr;
};

class LogListener : Channel {
public:
    LogListener(std::string name, std::string server);
};

}
#endif // !__WRITER__
