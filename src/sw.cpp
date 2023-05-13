#include <algorithm>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <sys/statvfs.h>

#include "sw.hpp"

namespace sw {

/* A generic stat table for CPU, Disk and Memory */
struct Stat {
    StatType type;
    uint32_t used;
    uint32_t total;
};

Stat get_cpu();
Stat get_memory();
/**
 * returns the disk information of the given stat
 */
Stat get_disk(std::string path);
/**
 * write_stats - writes the stats into the given string stream
 * @s - string stream to insert
 * @stat - stats to insert
 */
inline void write_stats(std::stringstream& s, Stat stat);

ChannelManager::ChannelManager(int width, int height, uint16_t I2C_Speed, uint8_t I2C_Address)
    : screen(new SSD1306(width, height))
{
    this->screen->OLEDbegin(I2C_Speed, I2C_Address);
    this->screen_buffer = new uint8_t[width * (height / 8) + 1];
    this->screen->buffer = this->screen_buffer;
    this->screen->OLEDFillScreen(0xF0, 0);
    this->screen->setTextColor(WHITE);
    this->screen->setCursor(0, 0);
    this->screen->OLEDInvert(true);
    this->screen->setTextWrap(true);
    bcm2835_delay(100);
    this->screen->OLEDclearBuffer();
    this->channels = std::vector<Listener>(10);

    channels.push_back(StatListener());
    index = 0;
}

ChannelManager::~ChannelManager()
{
    this->screen->OLEDPowerDown();
    bcm2835_close();
}

void ChannelManager::add_channel(Listener l)
{
    this->channels.push_back(l);
}

inline void ChannelManager::next()
{
    index = (index + 1) % channels.size();
}

inline void ChannelManager::prev()
{
    index = (index - 1) % channels.size();
}

void ChannelManager::display()
{
    this->screen->print("                   _x");
    std::stringstream s;
    s << "      CH " << index;
    this->screen->println(s.str().c_str());
    this->screen->setCursor(4, 20);
    this->screen->println(this->channels[index].listen().c_str());

    this->screen->OLEDupdate();
}

StatListener ::StatListener(std::string path)
    : Listener("Stats")
{
    this->path = path;
}

std::string StatListener::listen()
{
    std::stringstream s;
    write_stats(s, get_cpu());
    write_stats(s, get_memory());
    write_stats(s, get_disk(this->path));
    return s.str();
}

ProcessListener::ProcessListener(std::string name, std::string cmd, std::string args)
    : Listener(name)
{
    this->cmd = cmd;
    this->args = args;
}

ProcessListener::ProcessListener(std::string name, std::string args)
    : Listener(name)
{
    this->cmd = "/bin/bash sh/log.sh";
    this->args = args;
}

std::string ProcessListener::listen()
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
    return s.str();
}

Stat get_cpu()
{
    Stat s;
    s.type = StatType::CPU;
    std::ifstream load("/proc/loadavg");
    std::string line;
    std::getline(load, line);
    s.used = (uint32_t)(std::atof(line.substr(0, 5).c_str()) * 100);
    s.total = 100;
    load.close();
    return s;
}

Stat get_memory()
{
    Stat s;
    s.type = StatType::MEMORY;
    std::ifstream mem("/proc/meminfo");
    std::string line;

    int found = 0;
    if (mem.is_open()) {
        while (std::getline(mem, line) && found < 2) {
            if (line.compare(0, 9, "MemTotal:") == 0) {
                s.total = std::stoll(line.substr(10));
                found++;
            } else if (line.compare(0, 8, "MemFree:") == 0) {
                s.used = s.total - std::stoll(line.substr(9));
                found++;
            }
        }
    }
    mem.close();
    return s;
}

Stat get_disk(std::string path)
{
    Stat s;
    s.type = StatType::DISK;
    struct statvfs buf;
    if (statvfs(path.c_str(), &buf) != -1) {
        uint64_t used = (buf.f_blocks - buf.f_bfree) / 1024;
        uint64_t total = buf.f_blocks / 1024;
        s.used = (uint32_t)used;
        s.total = (uint32_t)total;
    }
    return s;
}

inline void write_stats(std::stringstream& s, Stat stat)
{
    static const std::string titles[] = { "CPU:  ", "MEM:  ", "DISK: " };
    s << titles[stat.type] << (double)stat.used / (double)stat.total * 100
      << "%\n";
}

}
