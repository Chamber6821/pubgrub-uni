//
// Created by Ivan on 29.05.2023.
//

#pragma once

#include "Package.h"

class MemoryPackage : public Package {
    std::string              _name;
    int                      _version;
    std::vector<Requirement> _requirements;

public:
    MemoryPackage(std::string name, int version, std::vector<Requirement> requirements)
        : _name(std::move(name))
        , _version(version)
        , _requirements(std::move(requirements)) {}

    MemoryPackage(std::string name, int version)
        : MemoryPackage(name, version, {}) {}

    std::string              name() override { return _name; }
    int                      version() override { return _version; }
    std::vector<Requirement> requirements() override { return _requirements; }
};
