//
// Created by Ivan on 30.05.2023.
//

#pragma once

#include "Package.h"
#include <nlohmann/json.hpp>

class JsonPackage : public Package {
    nlohmann::json tuple;

public:
    JsonPackage(nlohmann::json tuple)
        : tuple(std::move(tuple)) {}

    std::string name() override {
        try {
            return tuple[0].get<std::string>();
        } catch (...) {
            std::throw_with_nested(std::runtime_error("Failed to get name of JSON package"));
        }
    }

    int version() override {
        try {
            return tuple[1].get<int>();
        } catch (...) {
            std::throw_with_nested(std::runtime_error("Failed to get version of JSON package"));
        }
    }

    std::vector<Requirement> requirements() override {
        try {
            auto map = tuple[2].get<nlohmann::json::object_t>();

            std::vector<Requirement> requirements;
            for (const auto& [name, range] : map) {
                requirements.push_back(JsonRequirement(name, range));
            }

            return std::move(requirements);
        } catch (...) {
            std::throw_with_nested(std::runtime_error("Failed to get requirements of JSON package"));
        }
    }
};
