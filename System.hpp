#ifndef __DARKRL__SYSTEM_HPP__
#define __DARKRL__SYSTEM_HPP__

#include <thread>

class System
{
public:
    System() = delete;

    static int CPUCores();
};

#endif
