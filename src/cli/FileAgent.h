#pragma once

#include <fstream>

#include <cli/Agent.h>
#include <cli/IstreamAgent.h>

class FileAgent : public IstreamAgent {
public:
    explicit FileAgent(const std::string& filename);
    ~FileAgent();
private:
    std::ifstream in;
};
