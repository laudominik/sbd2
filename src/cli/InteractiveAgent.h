#pragma once

#include <iostream>

#include <cli/Agent.h>
#include <cli/IstreamAgent.h>

class InteractiveAgent : public IstreamAgent {
public:
    InteractiveAgent(): IstreamAgent(std::cin) {};
};
