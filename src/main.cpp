#include "MemoryPackage.h"
#include "MutableRepository.h"
#include "Reactions/misc/owning/make.h"
#include "Reqirement.h"
#include "JsonProfile.h"
#include "IntervalSetFormatter.h"

#include <pubgrub/solve.hpp>

#include <sstream>

auto req(std::string name, pubgrub::interval_set<int> range) {
    return pubgrub::test::simple_req{name, range};
}

template <typename... Items>
auto reqs(Items... t) {
    return std::vector<Requirement>({t...});
}

struct explain_handler {
    std::stringstream message;

    void say(pubgrub::explain::dependency<Requirement> dep) {
        message << dep.dependent << " requires " << dep.dependency;
    }

    void say(pubgrub::explain::disallowed<Requirement> dep) {
        message << dep.requirement << " is not allowed";
    }

    void say(pubgrub::explain::unavailable<Requirement> un) {
        message << un.requirement << " is not available";
    }

    void say(pubgrub::explain::needed<Requirement> need) {
        message << need.requirement << " is needed";
    }

    void say(pubgrub::explain::conflict<Requirement> conf) {
        message << conf.a << " conflicts with " << conf.b;
    }

    void say(pubgrub::explain::compromise<Requirement> comp) {
        message << comp.left << " and " << comp.right << " agree on " << comp.result;
    }

    void say(pubgrub::explain::no_solution) { message << "There is no solution"; }

    void operator()(pubgrub::explain::separator) { message << '\n'; }

    template <typename What>
    void operator()(pubgrub::explain::conclusion<What> c) {
        message << "Thus: ";
        say(c.value);
        message << '\n';
    }

    template <typename What>
    void operator()(pubgrub::explain::premise<What> c) {
        message << "Known: ";
        say(c.value);
        message << '\n';
    }
};

int main() {
    auto profile = make<JsonProfile>("profiles/default.json");
    std::cout << std::format("Profile name: {}\n", profile->name());
    for (const auto &req : profile->packages()) {
        std::cout << std::format("Package: {} Range: {}\n", req.key, req.range);
    }
    return 0;

    auto repo = make<MutableRepository>();
    repo->add(make<MemoryPackage>("foo", 2));

    auto roots = reqs(req("foo", {1, 2}));

    // expected req("foo", {1, 2})
    try {
        auto solution = pubgrub::solve(roots, *repo);

        for (const auto& el : solution) {
            std::cout << el.key << " : " << el.range << std::endl;
        }
    } catch (const pubgrub::solve_failure_type_t<Requirement>& fail) {
        explain_handler ex;
        pubgrub::generate_explaination(fail, ex);
        std::cout << "Conflict! Explanation:" << std::endl;
        std::cout << ex.message.view() << std::endl;
    }
}
