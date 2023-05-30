#include "IntervalSetFormatter.h"
#include "JsonProfile.h"
#include "JsonRepository.h"
#include "Reactions/command/CmEmpty.h"
#include "Reactions/exit/ExFlag.h"
#include "Reactions/fork/FkArgRegex.h"
#include "Reactions/fork/FkElse.h"
#include "Reactions/fork/FkWithName.h"
#include "Reactions/front/Console.h"
#include "Reactions/parameter/PrString.h"
#include "Reactions/reaction/RcExit.h"
#include "Reactions/reaction/RcFork.h"
#include "Reactions/reaction/RcGlued.h"
#include "Reactions/reaction/RcMapped.h"
#include "Reactions/reaction/RcNothing.h"
#include "Reactions/reaction/RcNothrow.h"
#include "Reactions/reaction/RcParameterized.h"
#include "Reactions/reaction/RcText.h"
#include "Reqirement.h"
#include <pubgrub/solve.hpp>
#include <sstream>

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

class Solution : public RcMapped {
public:
    Solution(std::shared_ptr<std::string> profileName, std::shared_ptr<std::string> repositoryName)
        : RcMapped([profileName, repositoryName]() {
            auto profile = JsonProfile{std::format("profiles/{}.json", *profileName)};
            auto repo    = JsonRepository{
                std::filesystem::path{std::format("repos/{}.json", *repositoryName)}};

            try {
                auto solution = pubgrub::solve(profile.packages(), repo);
                auto strings
                    = solution | std::views::transform([](const auto& package) {
                          return make<RsText>(std::format("{} : {}\n", package.key, package.range));
                      });
                return makeOut<RsGlued>(strings.begin(), strings.end());
            } catch (const pubgrub::solve_failure_type_t<Requirement>& fail) {
                explain_handler ex;
                pubgrub::generate_explaination(fail, ex);
                return makeOut<RsGlued>(
                    make<RsText>("Conflict! Explanation:\n"),
                    make<RsText>(ex.message.view())
                );
            }
        }) {}
};

class StringProperty : public FkWithName {
public:
    StringProperty(
        std::string_view             name,
        std::string_view             argName,
        std::shared_ptr<std::string> propMemory
    )
        : FkWithName(
            name,
            make<RcFork>(
                make<FkArgRegex>(
                    R"(^\s*\S+\s*$)",
                    make<RcParameterized<std::string>>(
                        make<PrString>(),
                        [propMemory](auto value) {
                            propMemory->assign(value);
                            return makeOut<RsEmpty>();
                        }
                    )
                ),
                make<FkArgRegex>(R"(^\s*$)", make<RcMapped>([propMemory]() {
                                     return makeOut<
                                         RsGlued>(make<RsText>(*propMemory), make<RsText>("\n"));
                                 })),
                make<FkElse>(make<RcGlued>(
                    make<RcText>("Invalid arguments, expected: "),
                    make<RcText>(std::string(name)),
                    make<RcText>(" <"),
                    make<RcText>(std::string(argName)),
                    make<RcText>(">\n")
                ))
            )
        ) {}

    StringProperty(std::string_view name, std::shared_ptr<std::string> propMemory)
        : StringProperty(name, "string", std::move(propMemory)) {}
};

int main() {
    auto profile = make<std::string>("default");
    auto repo    = make<std::string>("default");
    auto flag    = make<ExFlag>();

    make<Console>(
        make<RcNothrow>(make<RcFork>(
            make<FkWithName>("", make<RcNothing>()),
            make<FkWithName>("exit", make<RcExit>(flag)),
            make<FkWithName>("solve", make<Solution>(profile, repo)),
            make<StringProperty>("profile", "name", profile),
            make<StringProperty>("repo", "name", repo),
            make<FkElse>(make<RcText>("Unknown command!\n"))
        )),
        ">"
    )
        ->start(flag);
}
