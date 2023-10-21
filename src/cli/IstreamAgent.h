#pragma once

#include <istream>

#include <cli/Agent.h>

class IstreamAgent : public Agent{
public:
    explicit IstreamAgent(std::istream& in) : in(in) {}
    Choice makeChoice() override;

private:
    std::istream& in;
};

