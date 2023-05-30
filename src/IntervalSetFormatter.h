//
// Created by Ivan on 30.05.2023.
//

#pragma once

#include <format>
#include <pubgrub/interval.hpp>
#include <sstream>

template <>
struct std::formatter<pubgrub::interval_set<int>> : std::formatter<std::string> {
    auto format(auto interval, format_context& ctx) {
        std::stringstream str;
        str << interval;
        return formatter<string>::format(str.str(), ctx);
    }
};
