//
// Created by Ivan on 30.05.2023.
//

#pragma once

#include "JsonRequirement.h"
#include "Profile.h"
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>

class JsonProfile : public Profile {
    std::string    _name;
    nlohmann::json object;

public:
    JsonProfile(std::string name, nlohmann::json object)
        : _name(std::move(name))
        , object(std::move(object)) {}

    JsonProfile(std::string name, std::istream& input)
        : JsonProfile(std::move(name), nlohmann::json::parse(input)) {}

    JsonProfile(std::string name, std::istream&& input)
        : JsonProfile(std::move(name), nlohmann::json::parse(input)) {}

    explicit JsonProfile(std::filesystem::path file)
        : JsonProfile(file.filename().string(), std::ifstream(file)) {}

    std::string              name() override { return _name; }
    std::vector<Requirement> packages() override {
        try {
            if (not object.is_object())
                throw std::runtime_error("Root of JSON is not a object");
            auto map = object.get<nlohmann::json::object_t>();

            std::vector<Requirement> requirements;
            for (const auto& [name, range] : map) {
                requirements.push_back(JsonRequirement(name, range));
            }

            return std::move(requirements);
        } catch (...) {
            std::throw_with_nested(std::runtime_error("Failed to parse JSON as Profile"));
        }
    }
};
