#pragma once

#include <cli/Agent.h>

class RandomAgent : public Agent {
public:
    explicit RandomAgent(uint32_t numOfOps);

    Choice makeChoice() override;
private:
    uint32_t numOfOps;
    uint32_t currentNumOfOps{};
};

