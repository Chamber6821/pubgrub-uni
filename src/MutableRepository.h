//
// Created by Ivan on 29.05.2023.
//

#pragma once

#include "Repository.h"

#include <memory>

class MutableRepository : public Repository {
    std::vector<std::shared_ptr<Package>> packages{};

public:
    void add(std::shared_ptr<Package> package) { packages.push_back(std::move(package)); }

    std::optional<Requirement> best_candidate(const Requirement& req) const noexcept {
        auto max_version
            = std::find_if(packages.rbegin(), packages.rend(), [&](auto pkg) {
                  return pkg->name() == req.key && req.range.contains(pkg->version());
              });
        if (max_version == packages.rend()) {
            return std::nullopt;
        }
        return Requirement{(*max_version)->name(),
                           pubgrub::interval_set<int>{(*max_version)->version(),
                                                      (*max_version)->version() + 1}};
    }

    std::vector<Requirement> requirements_of(const Requirement& req) const noexcept {
        const auto& [name, range] = req;
        const auto version        = (*range.iter_intervals().begin()).low;
        for (auto pkg : packages) {
            if (pkg->name() == name && pkg->version() == version) {
                std::vector<Requirement> ret;
                return pkg->requirements();
            }
        }
        assert(false && "Impossible?");
        std::terminate();
    }
};
