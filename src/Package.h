//
// Created by Ivan on 29.05.2023.
//

#pragma once

#include "Reactions/misc/Interface.h"
#include "Reqirement.h"

#include <string>
#include <vector>

class Package : public Interface {
public:
    virtual std::string              name()         = 0;
    virtual int                      version()      = 0;
    virtual std::vector<Requirement> requirements() = 0;
};
