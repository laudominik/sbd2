#include "RandomAgent.h"

#include <iostream>

#include <util/Constants.h>

Choice RandomAgent::makeChoice() {
    if(currentNumOfOps == numOfOps){
        std::cout << "EXIT" << std::endl;
        return {.op=EXIT};
    }
    auto y = random() % 4;
    auto op = static_cast<Operation>(y);

    Choice choice;
    choice.op = op;

    switch(op){
        case INSERT:
            choice.strArg = generateCarNumber();
            choice.uintArgKey = random() % sbd::constants::MAX_RECORD_KEY;
            std::cout << "INSERT " << choice.uintArgKey << " " << choice.strArg << std::endl;
            break;
        case UPDATE:
            choice.strArg = generateCarNumber();
            choice.uintArgKey = random() % sbd::constants::MAX_RECORD_KEY;
            choice.uintArgNewKey = random() % sbd::constants::MAX_RECORD_KEY;
            std::cout << "UPDATE " << choice.uintArgKey << " " << choice.uintArgNewKey << " " << choice.strArg << std::endl;
            break;
        case REMOVE:
            choice.uintArgKey = random() % sbd::constants::MAX_RECORD_KEY;
            std::cout << "REMOVE " << choice.uintArgKey << std::endl;
            break;
        case REORGANISE:
            std::cout << "REORGANISE" << std::endl;
            break;
        default:
            break;
    }

    currentNumOfOps++;
    return choice;
}

RandomAgent::RandomAgent(uint32_t numOfOps): numOfOps(numOfOps) {
    uint32_t seed = time(nullptr);
    std::cout << "[INFO] seed: " << seed << std::endl;
    srand(seed);
}
