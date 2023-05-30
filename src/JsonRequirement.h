//
// Created by Ivan on 30.05.2023.
//

#pragma once

#include "Reqirement.h"
#include <nlohmann/json.hpp>

Requirement JsonRequirement(std::string name, nlohmann::json range) {
    try {
        if (not range.is_array())
            throw std::runtime_error(std::format("Value for '{}' is not a array", name));
        auto arr = range.get<nlohmann::json::array_t>();
        if (arr.size() != 2)
            throw std::runtime_error(std::format("Length of value for '{}' is not 2", name));
        if (not arr[0].is_number_integer() or not arr[0].is_number_unsigned())
            throw std::runtime_error(
                std::format("Left side for '{}' is not unsigned integer", name));
        auto left = arr[0].get<int>();
        if (not arr[1].is_number_integer() or not arr[1].is_number_unsigned())
            throw std::runtime_error(
                std::format("Left side for '{}' is not unsigned integer", name));
        auto right = arr[1].get<int>();
        return {name, pubgrub::interval_set{left, right}};
    } catch (...) {
        std::throw_with_nested(std::runtime_error("Failed to parse requirement from JSON"));
    }
}
