#include "FileAgent.h"

FileAgent::FileAgent(const std::string &filename) : in(filename), IstreamAgent(in) {
    if(!in.good()){
        perror("ifstream::open");
        exit(1);
    }
}

FileAgent::~FileAgent() {
    in.close();
}
