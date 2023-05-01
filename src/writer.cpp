#include "writer.hpp"
#include <iomanip>
#include <sys/statvfs.h>

std::string titles[] = { "CPU:  ", "MEM:  ", "DISK: " };

namespace writer {

void fetch_cpu(Block* b);
void fetch_memory(Block* b);
void fetch_disk(Block* b, std::string path);

std::string writer::Block::str()
{
    std::stringstream s;
    s << std::fixed << std::setprecision(2) << titles[this->type]
      << (double)this->used / (double)this->total * 100 << "%";
    return s.str();
}

BlockListener ::BlockListener(BlockType type)
{
    this->disk_ctr = 20;
    this->block = {
        .type = type,
        .used = 0,
        .total = 0
    };
}

Block& BlockListener::listen(std::string path)
{
    if (this->block.type == BlockType::CPU) {
        fetch_cpu(&this->block);
    } else if (this->block.type == BlockType::MEMORY) {
        fetch_memory(&this->block);
    } else if (this->block.type == BlockType::DISK) {
        if (this->disk_ctr-- == 0 || this->block.total == 0) {
            fetch_disk(&this->block, path);
            this->disk_ctr = 20;
        }
    }
    return this->block;
}

void fetch_cpu(Block* b)
{
    std::ifstream load("/proc/loadavg");
    std::string line;
    std::getline(load, line);
    b->used = (uint32_t)(std::atof(line.substr(0, 5).c_str()) * 100);
    b->total = 100;
    load.close();
}

void fetch_memory(Block* b)
{
    std::ifstream mem("/proc/meminfo");
    std::string line;

    int found = 0;
    if (mem.is_open()) {
        while (std::getline(mem, line) && found < 2) {
            if (line.compare(0, 9, "MemTotal:") == 0) {
                b->total = std::stoll(line.substr(10));
                found++;
            } else if (line.compare(0, 8, "MemFree:") == 0) {
                b->used = b->total - std::stoll(line.substr(9));
                found++;
            }
        }
    }
    mem.close();
}

void fetch_disk(Block* b, std::string path)
{
    struct statvfs buf;
    if (statvfs(path.c_str(), &buf) == -1) {
        return;
    }
    uint64_t used = (buf.f_blocks - buf.f_bfree) / 1024;
    uint64_t total = buf.f_blocks / 1024;
    b->used = (uint32_t)used;
    b->total = (uint32_t)total;
}
}
