#pragma once

#include <string>
#include <cstdint>

enum Operation {
    INSERT,
    UPDATE,
    REMOVE,
    GET,
    REORGANISE,
    INORDER,
    EXIT
};

struct Choice {
    Operation op;
    std::string strArg;
    uint32_t uintArgKey;
    uint32_t uintArgNewKey;
};