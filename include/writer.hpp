/******************************************************************************

 * File: include/writer.hpp
 *
 * Author: Umut Sevdi
 * Created: 04/26/23
 * Description: writer.hpp - Writer object is responsible from writing the messages

*****************************************************************************/

#ifndef __WRITER__
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace writer {

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
};

}
#endif // !__WRITER__
