//
// Created by Ivan on 30.05.2023.
//

#pragma once

#include "Package.h"
#include "Reactions/misc/Interface.h"

#include <memory>
#include <set>
#include <string>

class Profile : public Interface {
public:
    virtual std::string              name()     = 0;
    virtual std::vector<Requirement> packages() = 0;
};
