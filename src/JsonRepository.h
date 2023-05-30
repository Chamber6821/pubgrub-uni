//
// Created by Ivan on 30.05.2023.
//

#pragma once

#include "JsonPackage.h"
#include "Repository.h"
#include <fstream>
#include <nlohmann/json.hpp>

class JsonRepository : public Repository {
    nlohmann::json packages;

public:
    explicit JsonRepository(nlohmann::json packages)
        : packages(std::move(packages)) {}

    explicit JsonRepository(std::istream& input)
        : JsonRepository(nlohmann::json::parse(input)) {}

    explicit JsonRepository(std::istream&& input)
        : JsonRepository(nlohmann::json::parse(input)) {}

    explicit JsonRepository(std::filesystem::path file)
        : JsonRepository(std::ifstream(file)) {}

    std::optional<Requirement> best_candidate(const Requirement& req) const noexcept override {
        auto satisfied = [&](auto json) {
            auto package = JsonPackage(json);
            return package.name() == req.key && req.range.contains(package.version());
        };
        auto max_version = std::find_if(packages.rbegin(), packages.rend(), satisfied);
        if (max_version == packages.rend())
            return std::nullopt;

        auto package = JsonPackage(*max_version);
        return Requirement{package.name(),
                           pubgrub::interval_set<int>{package.version(), package.version() + 1}};
    }
    std::vector<Requirement> requirements_of(const Requirement& req) const noexcept override {
        const auto& [name, range] = req;

        const auto version = (*range.iter_intervals().begin()).low;
        for (auto json : packages) {
            auto package = JsonPackage(json);
            if (package.name() == name && package.version() == version) {
                return package.requirements();
            }
        }

        assert(false && "Impossible?");
        std::terminate();
    }
};
