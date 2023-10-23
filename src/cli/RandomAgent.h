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

inline std::string generateCarNumber() {
    static constexpr auto regLen = 7u;
    static const char charset[] = { "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ" };
    std::string builder;
    builder.reserve(regLen);
    for (auto i = 0u; i < regLen; i++) {
        builder += charset[std::rand() % (sizeof(charset)-1)];
    }
    return builder;
}
