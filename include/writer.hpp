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

/******************************************************************************
                                STANDARD_LISTENER
*****************************************************************************/

enum BlockType {
    CPU,
    MEMORY,
    DISK
};

struct Block {
    BlockType type;
    uint32_t used;
    uint32_t total;
    /**
     * str - returns string representation of the block to display
     * on OLED screen
     */
    std::string str();
};

class BlockListener {
public:
    BlockListener(BlockType type);
    /**
     * listen - updates the Block value and returns a reference
     * @path - optional value for disk. No effect for the rest
     */
    Block& listen(std::string path = "/");

private:
    Block block;
    int disk_ctr;
};

struct ProcBlock {
    std::vector<std::string> msg;
    /**
     * str - returns string representation of the block to display
     * on OLED screen
     */
    std::string str();
};

/******************************************************************************
                                PROCESS_LISTENER
*****************************************************************************/

class ProcListener {
public:
    ProcListener();
    /**
     * listen - updates the Block value and returns a reference
     * @path - optional value for disk. No effect for the rest
     */
    ProcBlock& listen(std::string path = "/");

private:
    Block block;
    int disk_ctr;
};
}

/******************************************************************************
                                GENERIC_LISTENER
*****************************************************************************/

class Channel {
public:
    Channel();
    ~Channel();
    /* Prepares and returns the text to display on screen. */
    std::string show();

private:
    std::string name;
};

class DisplayController {
public:
    DisplayController();
    ~DisplayController();
    void next();
    void previous();
    void add_channel(Channel c);

private:
    Channel* current;
    std::vector<Channel> channels;
};

/**
ChannelHandler _current:<-[string]-show Channel <-[Block]- Listener?.listen()
     (next, previous)    Channel[]
 */

#endif // !__WRITER__
