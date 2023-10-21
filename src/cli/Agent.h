#pragma once

#include <cli/Choice.h>

class Agent {
public:
    virtual Choice makeChoice() = 0;
};
