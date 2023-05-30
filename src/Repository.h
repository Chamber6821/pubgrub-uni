//
// Created by Ivan on 29.05.2023.
//

#pragma once

#include "Package.h"
#include "Reactions/misc/Interface.h"
#include "Reqirement.h"

#include <algorithm>

class Repository : public Interface {
public:
    virtual std::optional<Requirement> best_candidate(const Requirement& req) const noexcept  = 0;
    virtual std::vector<Requirement>   requirements_of(const Requirement& req) const noexcept = 0;
};
