#include "writer.hpp"
#include <cstring>
#include <iomanip>
#include <sys/statvfs.h>

std::string titles[] = { "CPU:  ", "MEM:  ", "DISK: " };

namespace writer {

void fetch_cpu(StatBlock* b);
void fetch_memory(StatBlock* b);
void fetch_disk(StatBlock* b, std::string path);

std::string writer::StatBlock::str()
{
    std::stringstream s;
    s << std::fixed << std::setprecision(2) << titles[this->type]
      << (double)this->used / (double)this->total * 100 << "%";
    return s.str();
}

BlockListener ::BlockListener(StatType type, std::string path)
    : Channel("Stats")
{
    this->disk_ctr = 20;
    this->block.type = type;
    this->path = path;
}

Block& BlockListener::listen()
{
    if (this->block.type == StatType::CPU) {
        fetch_cpu(&this->block);
    } else if (this->block.type == StatType::MEMORY) {
        fetch_memory(&this->block);
    } else if (this->block.type == StatType::DISK) {
        if (this->disk_ctr-- == 0 || this->block.total == 0) {
            fetch_disk(&this->block, this->path);
            this->disk_ctr = 20;
        }
    }
    return this->block;
}

ProcessListener::ProcessListener(std::string name, std::string cmd, std::string args)
    : Channel(name)
{
    this->cmd = cmd;
    this->args = args;
    this->block = {};
}

ProcessListener::ProcessListener(std::string name, std::string args)
    : Channel(name)
{
    this->cmd = "/bin/bash sh/log.sh";
    this->args = args;
    this->block = {};
}

Block& ProcessListener::listen()
{
    std::string in = (this->cmd + " " + this->args);
    std::stringstream s;
    char buffer[128];
    memcpy(buffer, in.c_str(), 128);

    FILE* f = popen(buffer, "r");
    while (fgets(buffer, sizeof buffer, f) != NULL) {
        s << buffer;
    }
    pclose(f);
    this->block.lines = s.str();
    return this->block;
}

void fetch_cpu(StatBlock* b)
{
    std::ifstream load("/proc/loadavg");
    std::string line;
    std::getline(load, line);
    b->used = (uint32_t)(std::atof(line.substr(0, 5).c_str()) * 100);
    b->total = 100;
    load.close();
}

void fetch_memory(StatBlock* b)
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

void fetch_disk(StatBlock* b, std::string path)
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
