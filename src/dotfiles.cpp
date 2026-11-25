//
// dotfiles.cpp — installer entry point
//
// The module registry is parsed at COMPILE time: modules/module.json is
// embedded into the binary with #embed, so a contributor only ever edits
// JSON (add a module dir + an entry) and rebuilds — never C++ code.
// Device configs (configs/*.json) stay runtime-parsed; they differ per
// machine and cannot be baked in.
//
#include <print>
#include <string_view>

#include "dotfiles/dotfiles.hpp"

#include "thirdparty/simdjson/simdjson.h"

namespace {

// Byte-exact copy of modules/module.json, NUL-terminated.
// Quoted #embed follows the same relative-to-this-file search as #include.
constexpr const char registry[] = {
#embed "../modules/modules.json"
  ,
  0,
};

// The actual compile-time parse. Anything that violates simdjson's
// compile-time rules (heterogeneous arrays, comments, top-level
// primitives) fails HERE, at build time — not on a user's machine.
//
// simdjson synthesizes one aggregate per JSON object and one std::array
// per JSON array, so every element of a given array must generate the
// SAME type: identical keys in identical order, identical value types,
// and no nested arrays (their length is part of the type). Hence the
// FLAT registry: two homogeneous arrays only — "platforms" (families as
// a comma-joined string) and "files" (every entry carrying its module).
constexpr auto doc = simdjson::compile_time::parse_json<registry>();

static_assert(doc.platforms.size() == 6, "six platforms expected");
static_assert(doc.files.size() == 32, "thirty-two file entries expected");

}  // namespace

int main(int argc, const char **argv) {
    std::println("dotfiles — embedded registry: {} file entries, {} platforms", doc.files.size(), doc.platforms.size());

    std::println("\nplatforms (resolution chains):");
    for (const auto &p : doc.platforms) {
        std::println("  {:14} <- {}", std::string_view {p.name}, std::string_view {p.families});
    }

    std::println("\nmodules:");
    std::string_view current {};
    for (const auto &f : doc.files) {
        const std::string_view module {f.module};
        if (module != current) {
            current = module;
            std::size_t count {};
            for (const auto &g : doc.files) {
                count += std::string_view {g.module} == module;
            }
            std::println("  {:12} {} files", module, count);
        }
        std::println("    {:24} -> {} [{}]",
                     std::string_view {f.source},
                     std::string_view {f.destination},
                     std::string_view {f.type});
    }

    constexpr auto config = reflex::cli::configuration {
      .completion = {.enabled = false},
    };

    return reflex::cli::run<rs_dotfiles::dotfiles, config>(rs_dotfiles::dotfiles {}, argc, argv);
}
