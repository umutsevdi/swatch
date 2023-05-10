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
     */
    virtual Block& listen();

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
    BlockListener(StatType type, std::string path = "/");
    /**
     * listens selected stat, updates and returns the block reference
     */
    Block& listen() override;

private:
    StatBlock block;
    int disk_ctr;
    std::string path;
};

/******************************************************************************
                            PROCESS_LISTENER
*****************************************************************************/

struct ProcessBlock : Block {
    std::string lines;
    std::string str() override { return this->lines; };
};

class ProcessListener : Channel {
public:
    ProcessListener(std::string name, std::string cmd, std::string args);
    ProcessListener(std::string name, std::string args);
    /**
     * executes defined process and writes to the block
     */
    Block& listen() override;

private:
    std::string cmd;
    std::string args;
    ProcessBlock block;
};

}
#endif // !__WRITER__
