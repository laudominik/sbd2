#pragma once

#include <cli/Agent.h>

class InteractiveAgent : public Agent {
    Choice makeChoice() override;
};
