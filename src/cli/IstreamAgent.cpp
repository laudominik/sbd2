#include "IstreamAgent.h"

#include <iostream>

Choice IstreamAgent::makeChoice() {
    while(true) {
        std::string command;
        std::cout << ">>";
        in >> command;

        if (command == "INSERT") {
            uint32_t key;
            std::string data;
            in >> key;
            in >> data;
            return {.op=INSERT, .strArg=data, .uintArgKey=key};
        } else if (command == "REMOVE") {
            uint32_t key;
            in >> key;
            return {.op=REMOVE, .uintArgKey=key};
        } else if (command == "UPDATE") {
            uint32_t key;
            uint32_t newKey;
            std::string data;
            in >> key;
            in >> newKey;
            in >> data;
            return {.op=UPDATE, .strArg=data, .uintArgKey=key, .uintArgNewKey=newKey};
        } else if (command == "REORGANISE") {
            return {.op=REORGANISE};
        } else if (command == "INORDER") {
            return {.op=INORDER};
        } else if (command == "EXIT") {
            return {.op=EXIT};
        } else {
            std::cout << "invalid command" << std::endl <<
                      "INSERT <key> <value>" << std::endl <<
                      "REMOVE <key>" << std::endl <<
                      "UPDATE <key> <newKey> <newValue>" << std::endl <<
                      "REORGANISE" << std::endl <<
                      "INORDER" << std::endl <<
                      "EXIT" << std::endl;
        }
    }
}
