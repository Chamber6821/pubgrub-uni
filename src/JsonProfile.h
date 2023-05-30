//
// Created by Ivan on 30.05.2023.
//

#pragma once

#include "Profile.h"
#include "Reqirement.h"
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
                if (not range.is_array())
                    throw std::runtime_error(std::format("Value for '{}' is not a array", name));
                auto arr = range.get<nlohmann::json::array_t>();
                if (arr.size() != 2)
                    throw std::runtime_error(
                        std::format("Length of value for '{}' is not 2", name));

                if (not arr[0].is_number_integer() or not arr[0].is_number_unsigned())
                    throw std::runtime_error(
                        std::format("Left side for '{}' is not unsigned integer", name));
                auto left = arr[0].get<int>();

                if (not arr[1].is_number_integer() or not arr[1].is_number_unsigned())
                    throw std::runtime_error(
                        std::format("Left side for '{}' is not unsigned integer", name));
                auto right = arr[1].get<int>();

                requirements.emplace_back(name, pubgrub::interval_set{left, right});
            }

            return std::move(requirements);
        } catch (...) {
            std::throw_with_nested(std::runtime_error("Failed to parse JSON as Profile"));
        }
    }
};
